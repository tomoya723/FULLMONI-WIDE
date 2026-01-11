using System.IO;

namespace FullmoniTerminal.Services;

/// <summary>
/// ファームウェアアップデートサービス
/// Size prefix + Streaming プロトコルでバイナリデータをBootloaderに転送
///
/// プロトコル:
/// 1. Bootloaderにリブート後、バナー待ち
/// 2. 'U' コマンド送信 → Flash消去
/// 3. "Erase OK" 受信
/// 4. Size (4byte LE) 送信 → ACK '.'
/// 5. データ送信 (ストリーミング、中間ACKなし)
/// 6. "Done!" 受信 → 自動リセット
/// </summary>
public class FirmwareUpdateService
{
    private readonly SerialPortService _serialService;

    // ACK文字
    private const byte ACK_CHAR = (byte)'.';

    // 送信制御 - ストリーミングモード（中間ACKなし）
    private const int ChunkSize = 16384;       // 16KB チャンクで最大速度
    private const int AckTimeoutMs = 10000;    // サイズACK待ちタイムアウト
    private const int ProgressUpdateInterval = 1; // 進捗更新間隔（%）- 細かく更新

    // 全体進捗のフェーズ重み付け（合計100%）
    private const int PhaseBootloaderSwitch = 5;   // Bootloader切り替え: 0-5%
    private const int PhaseFlashErase = 15;        // Flash消去: 5-20%
    private const int PhaseTransferStart = 20;     // 転送開始: 20%
    private const int PhaseTransferEnd = 98;       // 転送終了: 98%
    private const int PhaseVerify = 100;           // 検証完了: 100%

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
        TaskCompletionSource<bool>? currentAckWaiter = null;

        // 受信イベントハンドラ
        void DataReceivedHandler(object? sender, string data)
        {
            foreach (var c in data)
            {
                receivedData.Add((byte)c);

                if (c == (char)ACK_CHAR)
                {
                    Log($"RX: ACK ('.')");
                    currentAckWaiter?.TrySetResult(true);
                }
                else if (c >= 0x20 && c <= 0x7E)
                {
                    // Log printable characters (batched)
                }
                else if (c == '\r' || c == '\n')
                {
                    // 改行時にバッファを出力
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
            // Bootloaderモードへ切り替え（必要な場合）
            if (switchToBootloader)
            {
                Log("=== Bootloaderモードへ切り替え ===");
                ProgressChanged?.Invoke(this, 0);

                // Step 1: Parameter Consoleに入る（空コマンドを送信）
                // FirmwareはSTANDBYモードで任意のデータ受信でParameter Consoleに遷移
                // 注意: この最初のデータはParam Console起動トリガーとして消費される
                Log("Parameter Console起動中...");
                UpdateStatus("Parameter Consoleに接続中...");
                _serialService.SendCommand("");  // 改行のみ送信

                // Parameter Consoleのバナーを待つ
                var consoleTimeout = DateTime.Now.AddSeconds(5);
                while (DateTime.Now < consoleTimeout)
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    var received = GetReceivedString(receivedData);
                    if (received.Contains("Parameter Console") || received.Contains("> "))
                    {
                        Log("Parameter Console起動確認");
                        ProgressChanged?.Invoke(this, 1);
                        break;
                    }
                    await Task.Delay(100, cancellationToken);
                }
                receivedData.Clear();
                await Task.Delay(200, cancellationToken);

                // Step 2: fwupdateコマンドを送信
                Log("TX: fwupdate");
                UpdateStatus("fwupdateコマンド送信中...");
                ProgressChanged?.Invoke(this, 2);
                _serialService.SendCommand("fwupdate");

                // Step 3: 確認プロンプトを待つ
                Log("確認プロンプト待機中...");
                var confirmTimeout = DateTime.Now.AddSeconds(5);
                while (DateTime.Now < confirmTimeout)
                {
                    cancellationToken.ThrowIfCancellationRequested();
                    var received = GetReceivedString(receivedData);
                    if (received.Contains("yes") || received.Contains("confirm"))
                    {
                        Log("確認プロンプト受信");
                        break;
                    }
                    await Task.Delay(100, cancellationToken);
                }
                receivedData.Clear();
                await Task.Delay(100, cancellationToken);

                // Step 4: "yes"を送信して確認
                Log("TX: yes");
                UpdateStatus("確認中...");
                ProgressChanged?.Invoke(this, 3);
                _serialService.SendCommand("yes");

                // リブート前の準備
                var portName = _serialService.CurrentPortName;
                var baudRate = _serialService.CurrentBaudRate;

                if (string.IsNullOrEmpty(portName))
                {
                    throw new InvalidOperationException("ポート名が取得できません");
                }

                // USBリセットを待つ（デバイスが切断される）
                Log("デバイスリブート中...");
                UpdateStatus("デバイスリブート中...");
                ProgressChanged?.Invoke(this, 4);
                await Task.Delay(1000, cancellationToken);

                // COMポートの再接続を試みる
                Log($"COMポート({portName})の再接続を試行中...");
                UpdateStatus($"{portName}に再接続中...");

                // イベントハンドラを一時解除
                _serialService.DataReceived -= DataReceivedHandler;

                // 再接続（最大10秒待機）
                var reconnected = await _serialService.ReconnectAsync(portName!, baudRate, maxRetries: 20, retryDelayMs: 500);

                if (!reconnected)
                {
                    throw new TimeoutException($"COMポート({portName})の再接続に失敗しました");
                }

                // イベントハンドラを再登録
                _serialService.DataReceived += DataReceivedHandler;
                Log("COMポート再接続完了");

                // 少し待ってからバナーを探す
                await Task.Delay(1000, cancellationToken);

                // Bootloaderの起動を待つ - バナーは起動時に1回だけ送信されるので
                // 再接続タイミングで見逃す可能性がある。改行を送ってプロンプトを得る
                Log("Bootloaderの起動を待機中...");
                UpdateStatus("Bootloader起動待機中...");
            }

            // Bootloaderバナーまたはプロンプトを待つ
            Log("Bootloaderバナーを待機中...");
            UpdateStatus("Bootloader接続待機中...");
            receivedData.Clear();  // バッファクリア

            // 改行を送信してBootloaderのプロンプトを表示させる
            await Task.Delay(500, cancellationToken);
            _serialService.SendCommand("");  // 改行のみ送信してBootloaderのレスポンスを確認

            var bannerTimeout = DateTime.Now.AddSeconds(10);
            bool bootloaderFound = false;
            while (DateTime.Now < bannerTimeout)
            {
                cancellationToken.ThrowIfCancellationRequested();

                var received = GetReceivedString(receivedData);
                // デバッグ: 受信データをログ出力
                if (receivedData.Count > 0 && DateTime.Now.Second % 2 == 0)
                {
                    Log($"DEBUG: Buffer contains {receivedData.Count} bytes");
                }

                // Bootloaderのバナーは "=== FULLMONI-WIDE Bootloader ==="
                // またはコマンドプロンプト ">" やメニュー表示を探す
                if (received.Contains("Bootloader") ||
                    received.Contains("U)pdate") ||
                    received.Contains("Commands:") ||
                    received.Contains("> "))
                {
                    Log("Bootloaderバナー/プロンプト受信");
                    ProgressChanged?.Invoke(this, PhaseBootloaderSwitch);
                    bootloaderFound = true;
                    break;
                }

                await Task.Delay(100, cancellationToken);
            }

            if (!bootloaderFound)
            {
                // 最後に受信したデータをログに出力
                var debugBuffer = GetReceivedString(receivedData);
                Log($"DEBUG: Final buffer: [{debugBuffer}]");
                throw new TimeoutException("Bootloaderバナー待ちタイムアウト - Bootloaderが起動していません");
            }

            receivedData.Clear();

            // 'U' コマンドを送信してFlash消去開始
            Log("TX: U (Update command)");
            UpdateStatus("Flash消去中...");
            ProgressChanged?.Invoke(this, PhaseBootloaderSwitch + 1); // 6%
            _serialService.SendCommand("U");

            // "Erase OK" を待つ
            var eraseTimeout = DateTime.Now.AddSeconds(60);  // Flash消去は時間がかかる
            var eraseStartTime = DateTime.Now;
            while (DateTime.Now < eraseTimeout)
            {
                cancellationToken.ThrowIfCancellationRequested();

                // Flash消去中の進捗を推定（約刀6秒かかる）
                var eraseElapsed = (DateTime.Now - eraseStartTime).TotalSeconds;
                var eraseProgress = Math.Min((int)(eraseElapsed / 6.0 * (PhaseFlashErase - PhaseBootloaderSwitch - 1)), PhaseFlashErase - PhaseBootloaderSwitch - 2);
                ProgressChanged?.Invoke(this, PhaseBootloaderSwitch + 1 + eraseProgress);

                var received = GetReceivedString(receivedData);
                if (received.Contains("Erase OK"))
                {
                    Log("RX: Erase OK");
                    ProgressChanged?.Invoke(this, PhaseFlashErase); // 15%
                    break;
                }
                if (received.Contains("Fail"))
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
            ProgressChanged?.Invoke(this, PhaseFlashErase + 2); // 17%
            _serialService.SendRaw(sizeBytes);

            // ACK を待つ
            currentAckWaiter = new TaskCompletionSource<bool>();
            if (!await WaitForAckAsync(currentAckWaiter, AckTimeoutMs, cancellationToken))
            {
                throw new TimeoutException("サイズACK待ちタイムアウト");
            }

            // ファームウェアデータをストリーミング送信（中間ACKなし）
            UpdateStatus($"転送中: 0/{totalBytes:N0} bytes");
            ProgressChanged?.Invoke(this, PhaseTransferStart); // 20%
            var sentBytes = 0;
            var lastProgressLog = -1;
            var stopwatch = System.Diagnostics.Stopwatch.StartNew();

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

                // 全体進捗を計算（転送は20%-98%の範囲）
                var transferProgress = (int)((sentBytes * 100L) / totalBytes);
                var overallProgress = PhaseTransferStart + (int)((transferProgress * (PhaseTransferEnd - PhaseTransferStart)) / 100);

                if (transferProgress >= lastProgressLog + ProgressUpdateInterval || transferProgress == 100)
                {
                    lastProgressLog = transferProgress;
                    ProgressChanged?.Invoke(this, overallProgress);

                    // ステータス更新
                    var elapsedSec = stopwatch.ElapsedMilliseconds / 1000.0;
                    var speedKBps = elapsedSec > 0 ? (sentBytes / 1024.0) / elapsedSec : 0;
                    UpdateStatus($"転送中: {sentBytes:N0}/{totalBytes:N0} bytes ({transferProgress}%) - {speedKBps:F1} KB/s");
                    Log($"TX: {sentBytes:N0}/{totalBytes:N0} bytes ({transferProgress}%) - {speedKBps:F1} KB/s");

                    // UIスレッドに制御を返す（5%ごとのみ）
                    await Task.Yield();
                }
            }

            stopwatch.Stop();
            var totalElapsedSec = stopwatch.ElapsedMilliseconds / 1000.0;
            var avgSpeedKBps = totalElapsedSec > 0 ? (totalBytes / 1024.0) / totalElapsedSec : 0;
            Log($"転送完了: {totalBytes:N0} bytes in {totalElapsedSec:F1}s ({avgSpeedKBps:F1} KB/s)");

            // 完了メッセージを待つ
            UpdateStatus("転送完了 - 検証中...");
            ProgressChanged?.Invoke(this, PhaseTransferEnd); // 98%
            await Task.Delay(2000, cancellationToken);

            var finalReceived = GetReceivedString(receivedData);
            if (finalReceived.Contains("Done"))
            {
                Log($"RX: Done! 転送成功");
                ProgressChanged?.Invoke(this, PhaseVerify); // 100%
                UpdateStatus("更新完了！デバイスが再起動します");
            }
            else if (finalReceived.Contains("Write ERR"))
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
