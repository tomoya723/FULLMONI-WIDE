using System.IO;

namespace FullmoniTerminal.Services;

/// <summary>
/// ファームウェアアップデートサービス
/// XON/XOFFフロー制御でバイナリデータをBootloaderに転送
/// </summary>
public class FirmwareUpdateService
{
    private readonly SerialPortService _serialService;
    
    // XON/XOFFフロー制御
    private const byte XON = 0x11;   // DC1 - Resume
    private const byte XOFF = 0x13; // DC3 - Pause
    
    // 送信制御
    private const int ChunkSize = 512;     // 一度に送信するバイト数
    private const int SendDelayMs = 5;     // チャンク間の待機時間
    
    private volatile bool _xoffReceived;
    private volatile bool _transferCancelled;

    /// <summary>
    /// 進捗変更イベント (0-100%)
    /// </summary>
    public event EventHandler<int>? ProgressChanged;

    /// <summary>
    /// ステータスメッセージイベント
    /// </summary>
    public event EventHandler<string>? StatusChanged;

    /// <summary>
    /// 転送完了イベント (true=成功, false=失敗)
    /// </summary>
    public event EventHandler<bool>? UpdateCompleted;

    /// <summary>
    /// 詳細ログイベント
    /// </summary>
    public event EventHandler<string>? LogMessage;

    public FirmwareUpdateService(SerialPortService serialService)
    {
        _serialService = serialService;
    }

    /// <summary>
    /// BINファイルを読み込む
    /// </summary>
    public static byte[] LoadBinFile(string filePath)
    {
        if (!File.Exists(filePath))
        {
            throw new FileNotFoundException("ファイルが見つかりません", filePath);
        }

        var data = File.ReadAllBytes(filePath);
        
        if (data.Length == 0)
        {
            throw new InvalidDataException("ファイルが空です");
        }

        // ファームウェアヘッダマジックの検証（オプション）
        // BL_FW_HEADER_MAGIC = 0x52584657 ("RXFW" in little-endian)
        if (data.Length >= 4)
        {
            uint magic = BitConverter.ToUInt32(data, 0);
            if (magic != 0x52584657)
            {
                // 警告のみ - マジックがなくても続行可能
                System.Diagnostics.Debug.WriteLine($"警告: ファームウェアヘッダマジックが見つかりません (0x{magic:X8})");
            }
        }

        return data;
    }

    /// <summary>
    /// ファームウェアを非同期で転送（ブートローダー切り替え含む）
    /// </summary>
    public async Task SendFirmwareAsync(byte[] firmwareData, bool switchToBootloader = true, CancellationToken cancellationToken = default)
    {
        if (!_serialService.IsConnected)
        {
            throw new InvalidOperationException("シリアルポートが接続されていません");
        }

        _xoffReceived = false;
        _transferCancelled = false;

        // 受信イベントを一時的にフックしてXON/XOFFを監視
        void DataReceivedHandler(object? sender, string data)
        {
            foreach (var c in data)
            {
                if (c == (char)XOFF)
                {
                    _xoffReceived = true;
                    Log("RX: XOFF (pause)");
                }
                else if (c == (char)XON)
                {
                    _xoffReceived = false;
                    Log("RX: XON (resume)");
                }
                else if (c >= 0x20 && c <= 0x7E)
                {
                    // 印字可能文字
                    Log($"RX: '{c}' (0x{(int)c:X2})");
                }
                else
                {
                    Log($"RX: 0x{(int)c:X2}");
                }
            }
        }

        _serialService.DataReceived += DataReceivedHandler;

        try
        {
            // ブートローダーモードに切り替え
            if (switchToBootloader)
            {
                Log("=== ブートローダーモードへ切り替え ===");
                
                // 0. UARTの同期を取るためにダミー文字を送信（最初の1文字が欠落する問題対策）
                Log("TX: (sync)");
                _serialService.SendCommand("");  // 空コマンド（改行のみ）を送信
                await Task.Delay(100, cancellationToken);
                
                // 1. fwupdateコマンドを送信
                Log("TX: fwupdate");
                UpdateStatus("fwupdateコマンド送信中...");
                _serialService.SendCommand("fwupdate");
                
                // 2. Firmwareの応答を待つ（"Type 'yes' to confirm"）
                Log("Firmwareの確認プロンプトを待機中...");
                await Task.Delay(1000, cancellationToken);
                
                // 3. "yes"を送信して確認
                Log("TX: yes");
                _serialService.SendCommand("yes");
                
                // 4. リブートを待つ（Firmwareがブートローダーにリブートするまで）
                Log("デバイスのリブートを待機中...");
                UpdateStatus("デバイスがブートローダーにリブート中...");
                await Task.Delay(3000, cancellationToken);
            }

            UpdateStatus($"転送開始: {firmwareData.Length:N0} bytes");

            var totalBytes = firmwareData.Length;
            var sentBytes = 0;
            var lastProgress = -1;

            // XONを待つ（Bootloaderが初期XONを送信する）
            UpdateStatus("Bootloaderからの準備完了信号を待機...");
            await WaitForXonAsync(TimeSpan.FromSeconds(10), cancellationToken);
            UpdateStatus("転送中...");

            while (sentBytes < totalBytes)
            {
                cancellationToken.ThrowIfCancellationRequested();
                
                if (_transferCancelled)
                {
                    throw new OperationCanceledException("転送がキャンセルされました");
                }

                // XOFFを受信したら待機
                while (_xoffReceived)
                {
                    Log("TX: 待機中 (XOFF)");
                    await Task.Delay(10, cancellationToken);
                }

                // チャンクサイズを計算
                var remaining = totalBytes - sentBytes;
                var chunkSize = Math.Min(ChunkSize, remaining);
                
                // データを送信
                var chunk = new byte[chunkSize];
                Array.Copy(firmwareData, sentBytes, chunk, 0, chunkSize);
                _serialService.SendRaw(chunk);
                
                sentBytes += chunkSize;

                // 進捗更新
                var progress = (int)((sentBytes * 100L) / totalBytes);
                if (progress != lastProgress)
                {
                    lastProgress = progress;
                    ProgressChanged?.Invoke(this, progress);
                    
                    // 10%ごとにログ出力
                    if (progress % 10 == 0)
                    {
                        Log($"TX: {sentBytes:N0}/{totalBytes:N0} bytes ({progress}%)");
                    }
                }

                // 少し待機（バッファオーバーフロー防止）
                await Task.Delay(SendDelayMs, cancellationToken);
            }

            UpdateStatus($"転送完了: {sentBytes:N0} bytes - 検証中...");
            
            // 転送完了後、Bootloaderが検証・再起動するのを待つ
            await Task.Delay(5000, cancellationToken);
            
            UpdateStatus("更新完了");
            UpdateCompleted?.Invoke(this, true);
        }
        catch (OperationCanceledException)
        {
            UpdateStatus("転送がキャンセルされました");
            UpdateCompleted?.Invoke(this, false);
            throw;
        }
        catch (Exception ex)
        {
            UpdateStatus($"エラー: {ex.Message}");
            UpdateCompleted?.Invoke(this, false);
            throw;
        }
        finally
        {
            _serialService.DataReceived -= DataReceivedHandler;
        }
    }

    /// <summary>
    /// 転送をキャンセル
    /// </summary>
    public void CancelTransfer()
    {
        _transferCancelled = true;
    }

    private async Task WaitForXonAsync(TimeSpan timeout, CancellationToken cancellationToken)
    {
        var deadline = DateTime.Now.Add(timeout);
        
        // 最初はXOFF状態と仮定（初期XONを受信するまで待つ）
        _xoffReceived = true;
        
        while (DateTime.Now < deadline)
        {
            cancellationToken.ThrowIfCancellationRequested();
            
            if (!_xoffReceived)
            {
                return; // XON受信
            }
            
            await Task.Delay(100, cancellationToken);
        }
        
        // タイムアウトしても続行（Bootloaderがすでに待機中の可能性）
        UpdateStatus("警告: XON待機タイムアウト - 転送を開始します");
        _xoffReceived = false;
    }

    private void UpdateStatus(string message)
    {
        StatusChanged?.Invoke(this, message);
        Log(message);
    }

    private void Log(string message)
    {
        LogMessage?.Invoke(this, message);
    }
}
