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
                Handshake = Handshake.XOnXOff,  // XON/XOFFソフトウェアフロー制御
                ReadTimeout = 500,
                WriteTimeout = 500,
                Encoding = System.Text.Encoding.ASCII,
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
        }
        catch (Exception ex)
        {
            ErrorOccurred?.Invoke(this, $"Send error: {ex.Message}");
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
