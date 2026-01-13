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
    private const int MaxImageSize = 391696;  // acmtc配列の実サイズ (firmware側と一致させる)
    private const string BmpMagic = "BM";

    // フラッシュ書き込み設定
    // 注: チャンクサイズは256バイトに設定（FirmwareUpdateServiceの16KBより小さい）
    // 理由: フラッシュ書き込みは各チャンクごとにACK待ちが必要で、
    // RX72Nのフラッシュプログラミング単位（128または256バイト）に合わせるため
    private const int ChunkSize = 256;
    private const int SizeAckTimeoutMs = 30000;  // サイズACK + 消去完了待ち (最大17ブロック × 約1秒)
    private const int ChunkAckTimeoutMs = 5000;  // チャンクACK待ち
    private const byte AckChar = 0x06;  // ASCII ACK制御文字（ログの'.'と衝突防止）

    // 終端マーカー (ファームウェアと一致させる)
    private static readonly byte[] EndMarker = { 0xED, 0x0F, 0xAA, 0x55 };

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
    /// BMPファイルを読み込んで検証し、AppWizard形式に変換
    /// </summary>
    /// <param name="filePath">BMPファイルパス</param>
    /// <returns>AppWizard形式データ（フラッシュ書き込み用）</returns>
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
        // 配列境界チェック
        if (data.Length < 26)
        {
            throw new InvalidDataException("BMPヘッダが不完全です（サイズ情報が読み取れません）");
        }

        int width = BitConverter.ToInt32(data, 18);
        int height = Math.Abs(BitConverter.ToInt32(data, 22)); // 高さは負の場合あり（トップダウン）

        if (width != ExpectedWidth || height != ExpectedHeight)
        {
            throw new InvalidDataException(
                $"画像サイズが不正です。期待値: {ExpectedWidth}×{ExpectedHeight}px、実際: {width}×{height}px");
        }

        // ビット深度チェック
        int bitsPerPixel = BitConverter.ToInt16(data, 28);
        if (bitsPerPixel != 24 && bitsPerPixel != 32)
        {
            throw new InvalidDataException(
                $"24bit または 32bit BMPが必要です。実際: {bitsPerPixel}bit");
        }

        // AppWizard形式（RGB565 RLEストリーム）に変換
        return AppWizardImageConverter.ConvertBmpToAppWizard(data);
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
                    Log($"RX: ACK (0x06)");
                    currentAckWaiter?.TrySetResult(true);
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

            // ACKを待つ (サイズ確認)
            currentAckWaiter = new TaskCompletionSource<bool>();
            if (!await WaitForAckAsync(currentAckWaiter, SizeAckTimeoutMs, cancellationToken))
            {
                throw new TimeoutException("サイズACK待ちタイムアウト");
            }

            // フラッシュ消去完了のACKを待つ
            UpdateStatus("フラッシュ消去中... (約20秒かかります)");
            ProgressChanged?.Invoke(this, 12);
            currentAckWaiter = new TaskCompletionSource<bool>();
            if (!await WaitForAckAsync(currentAckWaiter, SizeAckTimeoutMs, cancellationToken))
            {
                throw new TimeoutException("フラッシュ消去完了待ちタイムアウト");
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
                if (!await WaitForAckAsync(currentAckWaiter, ChunkAckTimeoutMs, cancellationToken))
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
        if (data.Count == 0) return string.Empty;
        return System.Text.Encoding.ASCII.GetString(data.ToArray());
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

    /// <summary>
    /// CRC-16-CCITT計算
    /// </summary>
    private static ushort CalculateCrc16Ccitt(byte[] data, int offset, int length)
    {
        ushort crc = 0xFFFF;
        for (int i = offset; i < offset + length; i++)
        {
            crc ^= (ushort)(data[i] << 8);
            for (int j = 0; j < 8; j++)
            {
                if ((crc & 0x8000) != 0)
                {
                    crc = (ushort)((crc << 1) ^ 0x1021);
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

    /// <summary>
    /// フラッシュから現在の起動画面を読み込む
    /// <summary>
    /// フラッシュから起動画面を読み込む（終端マーカー検出プロトコル）
    /// プロトコル:
    ///   1. FW→Host: サイズ(4バイト)
    ///   2. FW→Host: データ(連続送信)
    ///   3. FW→Host: CRC16(2バイト)
    ///   4. FW→Host: 終端マーカー(4バイト: 0xED 0x0F 0xAA 0x55)
    /// </summary>
    public async Task<byte[]> ReadFromFlashAsync(CancellationToken cancellationToken)
    {
        _serialService.SuspendDataReceivedEvent();

        try
        {
            _serialService.DiscardBuffers();

            Log("Parameter Consoleに接続中...");
            UpdateStatus("Parameter Consoleに接続中...");
            _serialService.SendCommand("");
            await Task.Delay(500, cancellationToken);

            _serialService.DiscardBuffers();

            Log("TX: imgread");
            UpdateStatus("起動画面読み込みモードに切り替え中...");
            ProgressChanged?.Invoke(this, 5);
            _serialService.SendCommand("imgread");

            await Task.Delay(300, cancellationToken);

            // 1. サイズ受信
            UpdateStatus("サイズ情報を受信中...");
            var timeout = 5000;
            uint totalSize = 0;
            int sizeOffset = -1;

            var headerBuffer = new byte[64];
            var headerCount = 0;
            var readBuffer = new byte[4096];

            while (timeout > 0)
            {
                cancellationToken.ThrowIfCancellationRequested();

                int bytesRead = await _serialService.ReadDirectAsync(readBuffer, 0, readBuffer.Length, 50);
                if (bytesRead > 0)
                {
                    int toCopy = Math.Min(bytesRead, headerBuffer.Length - headerCount);
                    Buffer.BlockCopy(readBuffer, 0, headerBuffer, headerCount, toCopy);
                    headerCount += toCopy;
                }

                // サイズパターンを探す
                for (int i = 0; i <= headerCount - 4; i++)
                {
                    uint testSize = (uint)(headerBuffer[i] |
                                           (headerBuffer[i+1] << 8) |
                                           (headerBuffer[i+2] << 16) |
                                           (headerBuffer[i+3] << 24));
                    if (testSize == MaxImageSize)
                    {
                        totalSize = testSize;
                        sizeOffset = i;
                        break;
                    }
                }

                if (sizeOffset >= 0) break;

                await Task.Delay(10, cancellationToken);
                timeout -= 10;
            }

            var hexDump = string.Join(" ", headerBuffer.Take(Math.Min(32, headerCount)).Select(b => b.ToString("X2")));
            Log($"RX header ({headerCount} bytes): {hexDump}");

            if (sizeOffset < 0)
            {
                throw new Exception($"サイズパターンが見つかりません (受信: {headerCount} bytes)");
            }

            Log($"RX: Size = {totalSize} bytes");

            // 2. データ + CRC + 終端マーカー受信
            // 総受信量 = データ + CRC(2) + 終端マーカー(4)
            int totalNeeded = (int)totalSize + 2 + 4;
            var receiveBuffer = new List<byte>(totalNeeded + 1024);  // 余裕を持って確保

            // headerBufferに既にデータが含まれている場合、それをコピー
            int dataStartInHeader = sizeOffset + 4;  // サイズ(4バイト)の後からデータ
            int alreadyReceived = headerCount - dataStartInHeader;
            if (alreadyReceived > 0)
            {
                for (int i = dataStartInHeader; i < headerCount; i++)
                {
                    receiveBuffer.Add(headerBuffer[i]);
                }
                Log($"Already have {alreadyReceived} bytes from header buffer");
            }

            var stopwatch = System.Diagnostics.Stopwatch.StartNew();
            var noDataCount = 0;
            var lastProgressPercent = -1;
            var endMarkerFound = false;

            UpdateStatus($"画像データ受信中: {totalSize:N0} bytes");

            // 終端マーカーを検出するまで受信を継続
            int loopCount = 0;
            while (!endMarkerFound)
            {
                cancellationToken.ThrowIfCancellationRequested();
                loopCount++;

                var loopSw = System.Diagnostics.Stopwatch.StartNew();
                int bytesRead = await _serialService.ReadDirectAsync(readBuffer, 0, readBuffer.Length, 500);
                loopSw.Stop();

                // 10回ごと、または0バイト受信時にログ
                if (loopCount % 10 == 0 || bytesRead == 0)
                {
                    int bytesToRead = _serialService.GetBytesToRead();
                    Log($"[DEBUG] Loop#{loopCount}: ReadDirectAsync returned {bytesRead} bytes in {loopSw.ElapsedMilliseconds}ms, total={receiveBuffer.Count}, BytesToRead={bytesToRead}");
                }

                if (bytesRead > 0)
                {
                    for (int i = 0; i < bytesRead; i++)
                    {
                        receiveBuffer.Add(readBuffer[i]);
                    }
                    noDataCount = 0;

                    // 終端マーカーをチェック（期待データ量を受信した後のみ）
                    // データ(391696) + CRC(2) = 391698 バイト以上受信してから検出
                    int minDataBeforeMarker = (int)totalSize + 2;
                    if (receiveBuffer.Count >= minDataBeforeMarker + 4)
                    {
                        int lastIdx = receiveBuffer.Count - 1;
                        if (receiveBuffer[lastIdx - 3] == EndMarker[0] &&
                            receiveBuffer[lastIdx - 2] == EndMarker[1] &&
                            receiveBuffer[lastIdx - 1] == EndMarker[2] &&
                            receiveBuffer[lastIdx] == EndMarker[3])
                        {
                            Log($"終端マーカー検出! 総受信: {receiveBuffer.Count} bytes");
                            endMarkerFound = true;
                        }
                    }

                    // 進捗表示
                    int dataReceived = Math.Min(receiveBuffer.Count, (int)totalSize);
                    var progress = (int)((dataReceived * 100L) / totalSize);
                    var overallProgress = 10 + (int)((progress * 85) / 100);

                    if (progress / 10 != lastProgressPercent / 10)
                    {
                        Log($"RX: {dataReceived:N0}/{totalSize:N0} bytes ({progress}%)");
                        lastProgressPercent = progress;
                    }
                    ProgressChanged?.Invoke(this, overallProgress);
                    UpdateStatus($"受信中: {dataReceived:N0}/{totalSize:N0} bytes ({progress}%)");
                }
                else
                {
                    noDataCount++;
                    // 既に十分なデータを受信していれば、終端マーカー待ちを延長
                    int maxNoDataCount = receiveBuffer.Count >= totalSize ? 60 : 40;  // 30秒 or 20秒
                    if (noDataCount > maxNoDataCount)
                    {
                        Log($"タイムアウト: 受信={receiveBuffer.Count}, 期待={totalNeeded}");
                        // 終端マーカーなしでも処理を試みる
                        break;
                    }
                }

                // 明らかに過剰な受信をした場合は中断
                if (receiveBuffer.Count > totalNeeded + 1024)
                {
                    Log($"警告: 過剰受信 ({receiveBuffer.Count} bytes), 処理を試みます");
                    break;
                }
            }

            stopwatch.Stop();
            var totalElapsedSec = stopwatch.Elapsed.TotalSeconds;
            var avgSpeedKBps = (receiveBuffer.Count / 1024.0) / totalElapsedSec;

            Log($"受信完了: {receiveBuffer.Count:N0} bytes in {totalElapsedSec:F1}s ({avgSpeedKBps:F1} KB/s)");

            // 受信データの検証
            if (receiveBuffer.Count < totalSize + 2)
            {
                throw new Exception($"受信データ不足: {receiveBuffer.Count}/{totalSize + 2} bytes");
            }

            // 終端マーカーを除去してデータを取り出す
            var imageData = receiveBuffer.ToArray();
            int dataEndIndex = imageData.Length;

            // 終端マーカーがあれば除去
            if (endMarkerFound && imageData.Length >= 4)
            {
                dataEndIndex -= 4;  // 終端マーカー4バイトを除去
            }

            // CRC検証
            UpdateStatus("CRC検証中...");
            if (dataEndIndex < totalSize + 2)
            {
                throw new Exception($"CRCデータが不足: dataEndIndex={dataEndIndex}, required={totalSize + 2}");
            }

            ushort receivedCrc = (ushort)(imageData[totalSize] | (imageData[totalSize + 1] << 8));

            ushort crc = 0xFFFF;
            for (int i = 0; i < totalSize; i++)
            {
                crc ^= (ushort)(imageData[i] << 8);
                for (int j = 0; j < 8; j++)
                {
                    if ((crc & 0x8000) != 0)
                        crc = (ushort)((crc << 1) ^ 0x1021);
                    else
                        crc <<= 1;
                }
            }

            Log($"CRC: received=0x{receivedCrc:X4}, calculated=0x{crc:X4}");

            if (receivedCrc != crc)
            {
                throw new Exception($"CRCエラー: 受信データが破損しています (受信CRC=0x{receivedCrc:X4}, 計算CRC=0x{crc:X4})");
            }

            Log("CRC検証OK");
            ProgressChanged?.Invoke(this, 100);
            UpdateStatus("読み込み完了");

            _serialService.SendCommand("exit");
            await Task.Delay(100, cancellationToken);

            var result = new byte[totalSize];
            Buffer.BlockCopy(imageData, 0, result, 0, (int)totalSize);
            return result;
        }
        finally
        {
            _serialService.ResumeDataReceivedEvent();
        }
    }
}
