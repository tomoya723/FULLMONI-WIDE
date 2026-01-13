using System.IO.Ports;

namespace FullmoniTerminal.Services;

/// <summary>
/// シリアルポート通信サービス
/// FULLMONI-WIDE ファームウェアとのUART通信を管理
/// </summary>
public class SerialPortService : IDisposable
{
    private SerialPort? _serialPort;
    private bool _disposed;

    /// <summary>
    /// データ受信時のイベント
    /// </summary>
    public event EventHandler<string>? DataReceived;

    /// <summary>
    /// 接続状態変更時のイベント
    /// </summary>
    public event EventHandler<bool>? ConnectionChanged;

    /// <summary>
    /// エラー発生時のイベント
    /// </summary>
    public event EventHandler<string>? ErrorOccurred;

    /// <summary>
    /// 接続状態
    /// </summary>
    public bool IsConnected => _serialPort?.IsOpen ?? false;

    /// <summary>
    /// 現在接続中のポート名
    /// </summary>
    public string? CurrentPortName => _serialPort?.PortName;

    /// <summary>
    /// 現在のボーレート
    /// </summary>
    public int CurrentBaudRate => _serialPort?.BaudRate ?? 115200;

    /// <summary>
    /// COMポートの再接続を試みる
    /// </summary>
    /// <param name="portName">ポート名</param>
    /// <param name="baudRate">ボーレート</param>
    /// <param name="maxRetries">最大リトライ回数</param>
    /// <param name="retryDelayMs">リトライ間隔(ms)</param>
    /// <returns>接続成功時true</returns>
    public async Task<bool> ReconnectAsync(string portName, int baudRate, int maxRetries = 20, int retryDelayMs = 500)
    {
        // 現在の接続を閉じる
        Disconnect();

        // リトライループ
        for (int i = 0; i < maxRetries; i++)
        {
            await Task.Delay(retryDelayMs);

            // ポートが存在するか確認
            var availablePorts = GetAvailablePorts();
            if (availablePorts.Contains(portName))
            {
                // 接続を試みる
                if (Connect(portName, baudRate))
                {
                    return true;
                }
            }
        }

        return false;
    }

    /// <summary>
    /// 利用可能なCOMポート一覧を取得
    /// </summary>
    public static string[] GetAvailablePorts()
    {
        return SerialPort.GetPortNames().OrderBy(p => p).ToArray();
    }

    /// <summary>
    /// シリアルポートに接続
    /// </summary>
    /// <param name="portName">COMポート名</param>
    /// <param name="baudRate">ボーレート（デフォルト: 115200）</param>
    /// <returns>接続成功時true</returns>
    public bool Connect(string portName, int baudRate = 115200)
    {
        try
        {
            if (_serialPort?.IsOpen == true)
            {
                Disconnect();
            }

            _serialPort = new SerialPort
            {
                PortName = portName,
                BaudRate = baudRate,
                DataBits = 8,
                Parity = Parity.None,
                StopBits = StopBits.One,
                Handshake = Handshake.None,     // USB CDC ではフロー制御不要
                DtrEnable = true,               // DTRを有効化（USB CDCで受信有効化に必要な場合がある）
                RtsEnable = true,               // RTSも有効化
                ReadTimeout = 500,
                WriteTimeout = 5000,            // ファームウェア転送用に長めに設定
                WriteBufferSize = 65536,        // 書き込みバッファを大きく（64KB）
                ReadBufferSize = 524288,        // 読み込みバッファを大きく（512KB）- 画像転送用
                ReceivedBytesThreshold = 1,     // 1バイトでもあればイベント発火
                Encoding = System.Text.Encoding.Latin1, // バイナリ転送対応（0x00-0xFF全て保持）
                NewLine = "\r\n"
            };

            _serialPort.DataReceived += SerialPort_DataReceived;
            _serialPort.ErrorReceived += SerialPort_ErrorReceived;
            _serialPort.Open();

            ConnectionChanged?.Invoke(this, true);
            return true;
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, $"Connection failed: {ex.Message}");
            return false;
        }
    }

    /// <summary>
    /// シリアルポートから切断
    /// </summary>
    public void Disconnect()
    {
        try
        {
            if (_serialPort != null)
            {
                if (_serialPort.IsOpen)
                {
                    _serialPort.DataReceived -= SerialPort_DataReceived;
                    _serialPort.ErrorReceived -= SerialPort_ErrorReceived;
                    _serialPort.Close();
                }
                _serialPort.Dispose();
                _serialPort = null;
            }
            ConnectionChanged?.Invoke(this, false);
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, $"Disconnect error: {ex.Message}");
        }
    }

    /// <summary>
    /// コマンド送信
    /// </summary>
    /// <param name="command">送信するコマンド文字列</param>
    public void SendCommand(string command)
    {
        if (_serialPort?.IsOpen != true)
        {
            ErrorOccurred?.Invoke(this, "Port is not open");
            return;
        }

        try
        {
            // コマンドに改行を付加して送信
            _serialPort.Write(command + "\r");
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, $"Send error: {ex.Message}");
        }
    }

    /// <summary>
    /// 生データ送信
    /// </summary>
    /// <param name="data">送信するバイト配列</param>
    public void SendRaw(byte[] data)
    {
        if (_serialPort?.IsOpen != true)
        {
            ErrorOccurred?.Invoke(this, "Port is not open");
            return;
        }

        try
        {
            _serialPort.Write(data, 0, data.Length);
            _serialPort.BaseStream.Flush();  // 確実に送信
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, $"Send error: {ex.Message}");
        }
    }

    /// <summary>
    /// 受信バッファをクリア
    /// </summary>
    public void DiscardBuffers()
    {
        if (_serialPort?.IsOpen != true)
        {
            return;
        }

        try
        {
            _serialPort.DiscardInBuffer();
            _serialPort.DiscardOutBuffer();
        }
        catch
        {
            // Ignore
        }
    }

    /// <summary>
    /// 1文字送信（エコーバック用）
    /// </summary>
    public void SendChar(char c)
    {
        if (_serialPort?.IsOpen != true)
        {
            return;
        }

        try
        {
            _serialPort.Write(new char[] { c }, 0, 1);
        }
        catch
        {
            // Ignore
        }
    }

    /// <summary>
    /// 受信可能なバイト数を取得
    /// </summary>
    public int BytesToRead => _serialPort?.BytesToRead ?? 0;

    /// <summary>
    /// BaseStreamから直接データを読み取る（バイナリ転送用）
    /// イベントベースよりも確実にデータを受信できる
    /// </summary>
    /// <param name="buffer">受信バッファ</param>
    /// <param name="offset">バッファ内のオフセット</param>
    /// <param name="count">読み取る最大バイト数</param>
    /// <returns>実際に読み取ったバイト数</returns>
    public int ReadDirect(byte[] buffer, int offset, int count)
    {
        if (_serialPort?.IsOpen != true)
        {
            return 0;
        }

        try
        {
            int bytesToRead = _serialPort.BytesToRead;
            if (bytesToRead <= 0)
            {
                return 0;
            }

            int toRead = Math.Min(bytesToRead, count);
            return _serialPort.Read(buffer, offset, toRead);
        }
        catch
        {
            return 0;
        }
    }

    /// <summary>
    /// BaseStreamから指定バイト数を読み取る（タイムアウト付き）
    /// BytesToReadがある場合はSerialPort.Readで一括読み取り
    /// </summary>
    /// <param name="buffer">受信バッファ</param>
    /// <param name="offset">バッファ内のオフセット</param>
    /// <param name="count">読み取る最大バイト数</param>
    /// <param name="timeoutMs">タイムアウト(ms)</param>
    /// <returns>実際に読み取ったバイト数</returns>
    public async Task<int> ReadDirectAsync(byte[] buffer, int offset, int count, int timeoutMs = 100)
    {
        if (_serialPort?.IsOpen != true)
        {
            return 0;
        }

        try
        {
            // まずBytesToReadをチェック - データがあれば即座に読み取り
            int available = _serialPort.BytesToRead;
            if (available > 0)
            {
                int toRead = Math.Min(available, count);
                int bytesRead = _serialPort.Read(buffer, offset, toRead);
                return bytesRead;
            }

            // データがない場合はタイムアウトまで待機
            var sw = System.Diagnostics.Stopwatch.StartNew();
            while (sw.ElapsedMilliseconds < timeoutMs)
            {
                available = _serialPort.BytesToRead;
                if (available > 0)
                {
                    int toRead = Math.Min(available, count);
                    int bytesRead = _serialPort.Read(buffer, offset, toRead);
                    return bytesRead;
                }
                await Task.Delay(1);
            }

            return 0;
        }
        catch (Exception)
        {
            return 0;
        }
    }

    /// <summary>
    /// DataReceivedイベントを一時的に無効化（直接読み取りモード用）
    /// </summary>
    public void SuspendDataReceivedEvent()
    {
        if (_serialPort != null)
        {
            _serialPort.DataReceived -= SerialPort_DataReceived;
        }
    }

    /// <summary>
    /// ドライババッファ内のデータバイト数を取得
    /// </summary>
    public int GetBytesToRead()
    {
        if (_serialPort?.IsOpen != true)
        {
            return -1;
        }
        try
        {
            return _serialPort.BytesToRead;
        }
        catch
        {
            return -1;
        }
    }

    /// <summary>
    /// DataReceivedイベントを再開
    /// </summary>
    public void ResumeDataReceivedEvent()
    {
        if (_serialPort != null)
        {
            _serialPort.DataReceived -= SerialPort_DataReceived;  // 重複防止
            _serialPort.DataReceived += SerialPort_DataReceived;
        }
    }

    private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
    {
        if (_serialPort == null || !_serialPort.IsOpen)
        {
            return;
        }

        try
        {
            var data = _serialPort.ReadExisting();
            if (!string.IsNullOrEmpty(data))
            {
                DataReceived?.Invoke(this, data);
            }
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, $"Read error: {ex.Message}");
        }
    }

    private void SerialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
    {
        ErrorOccurred?.Invoke(this, $"Serial error: {e.EventType}");
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (!_disposed)
        {
            if (disposing)
            {
                Disconnect();
            }
            _disposed = true;
        }
    }

    ~SerialPortService()
    {
        Dispose(false);
    }
}
