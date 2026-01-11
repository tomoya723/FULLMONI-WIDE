using System.IO;

namespace FullmoniTerminal.Services;

/// <summary>
/// 起動画面書き込みサービス
/// RX72NのフラッシュメモリへUSB CDC経由でBMP画像を書き込む
/// </summary>
public class StartupImageService
{
    private readonly SerialPortService _serialService;
    
    // 起動画面の仕様
    private const int ExpectedWidth = 765;
    private const int ExpectedHeight = 256;
    private const string BmpMagic = "BM";
    
    // フラッシュ書き込み設定
    private const int ChunkSize = 256;  // FirmwareUpdateServiceと同様の256バイトチャンク
    private const int AckTimeoutMs = 5000;
    private const byte AckChar = (byte)'.';
    
    /// <summary>
    /// 進捗変更イベント (0-100%)
    /// </summary>
    public event EventHandler<int>? ProgressChanged;
    
    /// <summary>
    /// ステータスメッセージイベント
    /// </summary>
    public event EventHandler<string>? StatusChanged;
    
    /// <summary>
    /// 詳細ログイベント
    /// </summary>
    public event EventHandler<string>? LogMessage;
    
    /// <summary>
    /// 書き込み完了イベント (true=成功, false=失敗)
    /// </summary>
    public event EventHandler<bool>? WriteCompleted;
    
    public StartupImageService(SerialPortService serialService)
    {
        _serialService = serialService;
    }
    
    /// <summary>
    /// BMPファイルを読み込んで検証
    /// </summary>
    /// <param name="filePath">BMPファイルパス</param>
    /// <returns>BMPデータ（ヘッダ含む）</returns>
    /// <exception cref="FileNotFoundException">ファイルが見つからない</exception>
    /// <exception cref="InvalidDataException">BMP形式またはサイズが不正</exception>
    public static byte[] LoadAndValidateBmp(string filePath)
    {
        if (!File.Exists(filePath))
        {
            throw new FileNotFoundException("ファイルが見つかりません", filePath);
        }
        
        var data = File.ReadAllBytes(filePath);
        
        if (data.Length < 54)
        {
            throw new InvalidDataException("ファイルサイズが小さすぎます（BMPヘッダが不完全）");
        }
        
        // BMPマジックナンバーチェック（"BM" = 0x42 0x4D）
        if (data[0] != 0x42 || data[1] != 0x4D)
        {
            throw new InvalidDataException("BMP形式ではありません（マジックナンバー不一致）");
        }
        
        // 画像サイズをBMPヘッダから読み取る（little-endian）
        // オフセット18: 幅（4バイト）
        // オフセット22: 高さ（4バイト）
        int width = BitConverter.ToInt32(data, 18);
        int height = Math.Abs(BitConverter.ToInt32(data, 22)); // 高さは負の場合あり（トップダウン）
        
        if (width != ExpectedWidth || height != ExpectedHeight)
        {
            throw new InvalidDataException(
                $"画像サイズが不正です。期待値: {ExpectedWidth}×{ExpectedHeight}px、実際: {width}×{height}px");
        }
        
        return data;
    }
    
    /// <summary>
    /// フラッシュメモリへBMP画像を書き込む
    /// </summary>
    /// <param name="imageData">BMPデータ</param>
    /// <param name="cancellationToken">キャンセルトークン</param>
    public async Task WriteToFlashAsync(byte[] imageData, CancellationToken cancellationToken = default)
    {
        if (!_serialService.IsConnected)
        {
            throw new InvalidOperationException("シリアルポートが接続されていません");
        }
        
        var receivedData = new List<byte>();
        TaskCompletionSource<bool>? currentAckWaiter = null;
        
        // 受信イベントハンドラ
        void DataReceivedHandler(object? sender, string data)
        {
            foreach (var c in data)
            {
                receivedData.Add((byte)c);
                
                if (c == (char)AckChar)
                {
                    Log($"RX: ACK ('.')");
                    currentAckWaiter?.TrySetResult(true);
                }
                else if (c >= 0x20 && c <= 0x7E)
                {
                    // 印字可能文字をログ
                }
                else if (c == '\r' || c == '\n')
                {
                    var line = GetReceivedLine(receivedData);
                    if (!string.IsNullOrWhiteSpace(line))
                    {
                        Log($"RX: {line}");
                    }
                }
            }
        }
        
        _serialService.DataReceived += DataReceivedHandler;
        
        try
        {
            Log("=== 起動画面書き込み開始 ===");
            UpdateStatus("準備中...");
            ProgressChanged?.Invoke(this, 0);
            
            // Parameter Consoleに入る
            Log("Parameter Consoleに接続中...");
            UpdateStatus("Parameter Consoleに接続中...");
            _serialService.SendCommand("");
            await Task.Delay(500, cancellationToken);
            
            // imgwriteコマンドを送信（仮のコマンド名、実際のファームウェアに合わせて変更必要）
            Log("TX: imgwrite");
            UpdateStatus("起動画面書き込みモードに切り替え中...");
            ProgressChanged?.Invoke(this, 5);
            _serialService.SendCommand("imgwrite");
            
            // ACKまたはプロンプトを待つ
            await Task.Delay(1000, cancellationToken);
            receivedData.Clear();
            
            // サイズ送信（4バイト、little-endian）
            var totalBytes = imageData.Length;
            var sizeBytes = BitConverter.GetBytes((uint)totalBytes);
            
            Log($"TX: Size = {totalBytes} bytes (0x{totalBytes:X8})");
            UpdateStatus($"サイズ送信中: {totalBytes:N0} bytes");
            ProgressChanged?.Invoke(this, 10);
            _serialService.SendRaw(sizeBytes);
            
            // ACKを待つ
            currentAckWaiter = new TaskCompletionSource<bool>();
            if (!await WaitForAckAsync(currentAckWaiter, AckTimeoutMs, cancellationToken))
            {
                throw new TimeoutException("サイズACK待ちタイムアウト");
            }
            
            // データをチャンク単位で送信
            UpdateStatus($"転送中: 0/{totalBytes:N0} bytes");
            ProgressChanged?.Invoke(this, 15);
            var sentBytes = 0;
            var lastProgressPercent = -1;
            var stopwatch = System.Diagnostics.Stopwatch.StartNew();
            
            while (sentBytes < totalBytes)
            {
                cancellationToken.ThrowIfCancellationRequested();
                
                // チャンクサイズ計算
                var remaining = totalBytes - sentBytes;
                var chunkSize = Math.Min(ChunkSize, remaining);
                
                // チャンク送信
                var chunk = new byte[chunkSize];
                Array.Copy(imageData, sentBytes, chunk, 0, chunkSize);
                _serialService.SendRaw(chunk);
                
                // チャンクごとにACK待ち
                currentAckWaiter = new TaskCompletionSource<bool>();
                if (!await WaitForAckAsync(currentAckWaiter, AckTimeoutMs, cancellationToken))
                {
                    throw new TimeoutException($"チャンクACK待ちタイムアウト (offset: {sentBytes})");
                }
                
                sentBytes += chunkSize;
                
                // 進捗更新（15%-95%の範囲）
                var transferProgress = (int)((sentBytes * 100L) / totalBytes);
                var overallProgress = 15 + (int)((transferProgress * 80) / 100);
                
                if (transferProgress != lastProgressPercent)
                {
                    lastProgressPercent = transferProgress;
                    ProgressChanged?.Invoke(this, overallProgress);
                    
                    var elapsedSec = stopwatch.ElapsedMilliseconds / 1000.0;
                    var speedKBps = elapsedSec > 0 ? (sentBytes / 1024.0) / elapsedSec : 0;
                    UpdateStatus($"転送中: {sentBytes:N0}/{totalBytes:N0} bytes ({transferProgress}%) - {speedKBps:F1} KB/s");
                    
                    if (transferProgress % 10 == 0)
                    {
                        Log($"TX: {sentBytes:N0}/{totalBytes:N0} bytes ({transferProgress}%)");
                    }
                }
            }
            
            stopwatch.Stop();
            var totalElapsedSec = stopwatch.ElapsedMilliseconds / 1000.0;
            var avgSpeedKBps = totalElapsedSec > 0 ? (totalBytes / 1024.0) / totalElapsedSec : 0;
            Log($"転送完了: {totalBytes:N0} bytes in {totalElapsedSec:F1}s ({avgSpeedKBps:F1} KB/s)");
            
            // 完了メッセージを待つ
            UpdateStatus("書き込み完了を確認中...");
            ProgressChanged?.Invoke(this, 95);
            await Task.Delay(2000, cancellationToken);
            
            var finalReceived = GetReceivedString(receivedData);
            if (finalReceived.Contains("Done") || finalReceived.Contains("OK"))
            {
                Log("RX: Done! 書き込み成功");
                ProgressChanged?.Invoke(this, 100);
                UpdateStatus("起動画面の書き込みが完了しました");
                WriteCompleted?.Invoke(this, true);
            }
            else if (finalReceived.Contains("ERR") || finalReceived.Contains("Fail"))
            {
                throw new Exception("書き込みエラーが発生しました");
            }
            else
            {
                Log("警告: 完了メッセージを受信できませんでしたが、転送は完了しました");
                ProgressChanged?.Invoke(this, 100);
                UpdateStatus("起動画面の書き込みが完了しました（確認メッセージなし）");
                WriteCompleted?.Invoke(this, true);
            }
            
            // exitコマンドでParameter Consoleから抜ける
            _serialService.SendCommand("exit");
            await Task.Delay(100, cancellationToken);
        }
        catch (OperationCanceledException)
        {
            UpdateStatus("書き込みがキャンセルされました");
            WriteCompleted?.Invoke(this, false);
            throw;
        }
        catch (Exception ex)
        {
            UpdateStatus($"エラー: {ex.Message}");
            WriteCompleted?.Invoke(this, false);
            throw;
        }
        finally
        {
            _serialService.DataReceived -= DataReceivedHandler;
        }
    }
    
    private static string GetReceivedString(List<byte> data)
    {
        return string.Join("", data.Select(b => (char)b));
    }
    
    private static string GetReceivedLine(List<byte> data)
    {
        var str = GetReceivedString(data);
        var lastNewline = str.LastIndexOf('\n');
        if (lastNewline > 0)
        {
            var start = str.LastIndexOf('\n', lastNewline - 1);
            if (start < 0) start = 0;
            return str.Substring(start, lastNewline - start).Trim();
        }
        return str.Trim();
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
            return false;
        }
        catch (OperationCanceledException)
        {
            return false;
        }
    }
    
    private void UpdateStatus(string message)
    {
        StatusChanged?.Invoke(this, message);
    }
    
    private void Log(string message)
    {
        LogMessage?.Invoke(this, message);
    }
}
