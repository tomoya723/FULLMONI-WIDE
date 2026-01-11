using System.IO;

namespace FullmoniTerminal.Services;

/// <summary>
/// ファームウェアアップデートサービス
/// Size prefix + ACK プロトコルでバイナリデータをBootloaderに転送
/// 
/// プロトコル:
/// 1. Bootloaderが "READY\r\n" を送信
/// 2. PC が Size (4byte LE) を送信
/// 3. Bootloader が '.' (ACK) を返す
/// 4. PC が データを送信 (64byte単位)
/// 5. Bootloader が 128byte受信ごとに '.' (ACK) を返す
/// 6. 完了時 Bootloader が "OK <size>\r\n" を送信して自動リセット
/// </summary>
public class FirmwareUpdateService
{
    private readonly SerialPortService _serialService;

    // ACK文字
    private const byte ACK_CHAR = (byte)'.';

    // 送信制御
    private const int ChunkSize = 64;          // USB CDC パケットサイズ
    private const int AckIntervalBytes = 128;  // ACK受信間隔 (Flash書き込み単位)
    private const int AckTimeoutMs = 5000;     // ACK待ちタイムアウト

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
    /// ファームウェアを非同期で転送
    /// </summary>
    /// <param name="firmwareData">ファームウェアデータ</param>
    /// <param name="switchToBootloader">fwupdateコマンドでBootloaderに切り替えるか（既にBootloaderならfalse）</param>
    /// <param name="cancellationToken">キャンセルトークン</param>
    public async Task SendFirmwareAsync(byte[] firmwareData, bool switchToBootloader = true, CancellationToken cancellationToken = default)
    {
        if (!_serialService.IsConnected)
        {
            throw new InvalidOperationException("シリアルポートが接続されていません");
        }

        _transferCancelled = false;
        var receivedData = new List<byte>();
        var ackReceived = new TaskCompletionSource<bool>();

        // 受信イベントハンドラ
        void DataReceivedHandler(object? sender, string data)
        {
            foreach (var c in data)
            {
                receivedData.Add((byte)c);
                
                if (c == (char)ACK_CHAR)
                {
                    Log($"RX: ACK ('.')");
                    ackReceived.TrySetResult(true);
                }
                else if (c >= 0x20 && c <= 0x7E)
                {
                    Log($"RX: '{c}'");
                }
                else if (c == '\r' || c == '\n')
                {
                    // 改行は無視
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
            // Bootloaderモードへ切り替え（必要な場合）
            if (switchToBootloader)
            {
                Log("=== Bootloaderモードへ切り替え ===");
                UpdateStatus("fwupdateコマンド送信中...");
                
                // fwupdateコマンドを送信（Firmwareがリブートしてbootloaderになる）
                _serialService.SendCommand("fwupdate");
                
                // Firmwareが確認を求める場合に対応
                await Task.Delay(500, cancellationToken);
                _serialService.SendCommand("yes");
                
                // Bootloaderの起動を待つ（Flash消去含む）
                Log("Bootloaderの起動とFlash消去を待機中...");
                UpdateStatus("Bootloader起動待機中（Flash消去中）...");
                await Task.Delay(5000, cancellationToken);  // Flash消去に時間がかかる
            }

            // "READY" を待つ
            Log("Bootloaderからの 'READY' を待機中...");
            UpdateStatus("Bootloader準備完了待機中...");
            
            var readyTimeout = DateTime.Now.AddSeconds(30);
            while (DateTime.Now < readyTimeout)
            {
                cancellationToken.ThrowIfCancellationRequested();
                
                var received = string.Join("", receivedData.Select(b => (char)b));
                if (received.Contains("READY"))
                {
                    Log("RX: READY 受信");
                    break;
                }
                if (received.Contains("ERASE_ERR"))
                {
                    throw new Exception("Bootloader: Flash消去エラー");
                }
                
                await Task.Delay(100, cancellationToken);
            }
            
            receivedData.Clear();

            // Size を送信 (4byte, little-endian)
            var totalBytes = firmwareData.Length;
            var sizeBytes = BitConverter.GetBytes((uint)totalBytes);
            
            Log($"TX: Size = {totalBytes} bytes (0x{totalBytes:X8})");
            UpdateStatus($"サイズ送信中: {totalBytes:N0} bytes");
            _serialService.SendRaw(sizeBytes);

            // ACK を待つ
            ackReceived = new TaskCompletionSource<bool>();
            if (!await WaitForAckAsync(ackReceived, AckTimeoutMs, cancellationToken))
            {
                throw new TimeoutException("サイズACK待ちタイムアウト");
            }

            // ファームウェアデータを送信
            UpdateStatus($"転送中: 0/{totalBytes:N0} bytes");
            var sentBytes = 0;
            var bytesSinceLastAck = 0;
            var lastProgress = -1;

            while (sentBytes < totalBytes)
            {
                cancellationToken.ThrowIfCancellationRequested();
                
                if (_transferCancelled)
                {
                    throw new OperationCanceledException("転送がキャンセルされました");
                }

                // チャンクサイズを計算
                var remaining = totalBytes - sentBytes;
                var chunkSize = Math.Min(ChunkSize, remaining);

                // データを送信
                var chunk = new byte[chunkSize];
                Array.Copy(firmwareData, sentBytes, chunk, 0, chunkSize);
                _serialService.SendRaw(chunk);

                sentBytes += chunkSize;
                bytesSinceLastAck += chunkSize;

                // 128バイトごとにACKを待つ
                if (bytesSinceLastAck >= AckIntervalBytes)
                {
                    ackReceived = new TaskCompletionSource<bool>();
                    if (!await WaitForAckAsync(ackReceived, AckTimeoutMs, cancellationToken))
                    {
                        throw new TimeoutException($"ACK待ちタイムアウト (sent: {sentBytes})");
                    }
                    bytesSinceLastAck = 0;
                }

                // 進捗更新
                var progress = (int)((sentBytes * 100L) / totalBytes);
                if (progress != lastProgress)
                {
                    lastProgress = progress;
                    ProgressChanged?.Invoke(this, progress);
                    
                    if (progress % 10 == 0)
                    {
                        var speed = sentBytes / 1024.0;  // 簡易速度表示
                        Log($"TX: {sentBytes:N0}/{totalBytes:N0} bytes ({progress}%)");
                    }
                }

                UpdateStatus($"転送中: {sentBytes:N0}/{totalBytes:N0} bytes ({progress}%)");
            }

            // 最終ACKを待つ（残りデータがある場合）
            if (bytesSinceLastAck > 0)
            {
                ackReceived = new TaskCompletionSource<bool>();
                await WaitForAckAsync(ackReceived, AckTimeoutMs, cancellationToken);
            }

            // 完了メッセージを待つ
            UpdateStatus("転送完了 - 検証中...");
            await Task.Delay(2000, cancellationToken);

            var finalReceived = string.Join("", receivedData.Select(b => (char)b));
            if (finalReceived.Contains("OK"))
            {
                Log($"RX: {finalReceived.Trim()}");
                UpdateStatus("更新完了！デバイスが再起動します");
            }
            else if (finalReceived.Contains("WRITE_ERR"))
            {
                throw new Exception("Bootloader: Flash書き込みエラー");
            }

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

    private async Task<bool> WaitForAckAsync(TaskCompletionSource<bool> ackReceived, int timeoutMs, CancellationToken cancellationToken)
    {
        using var cts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        cts.CancelAfter(timeoutMs);

        try
        {
            var delayTask = Task.Delay(timeoutMs, cts.Token);
            var completedTask = await Task.WhenAny(ackReceived.Task, delayTask);
            
            if (completedTask == ackReceived.Task)
            {
                return await ackReceived.Task;
            }
            return false;  // タイムアウト
        }
        catch (OperationCanceledException)
        {
            return false;
        }
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
