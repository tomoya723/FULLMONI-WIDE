using System.Collections.ObjectModel;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Media;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FullmoniTerminal.Services;

namespace FullmoniTerminal.ViewModels;

/// <summary>
/// メインウィンドウのViewModel
/// </summary>
public partial class MainViewModel : ObservableObject, IDisposable
{
    private readonly SerialPortService _serialService;
    private readonly StringBuilder _responseBuffer;
    private bool _disposed;
    private bool _isLoadingParameters;
    private TaskCompletionSource<string>? _responseWaiter;

    /// <summary>
    /// シリアルサービスへの公開アクセス（子ウィンドウ用）
    /// </summary>
    public SerialPortService SerialService => _serialService;

    /// <summary>
    /// 起動画面書き込みViewModel
    /// </summary>
    public StartupImageViewModel StartupImageViewModel { get; }

    /// <summary>
    /// アプリバージョン
    /// </summary>
    public string AppVersion => Assembly.GetExecutingAssembly()
        .GetName().Version?.ToString(3) ?? "1.0.0";

    /// <summary>
    /// コマンド送信メソッド（子ウィンドウ用）
    /// </summary>
    public void SendCommandDirect(string command)
    {
        _serialService.SendCommand(command);
    }

    public MainViewModel()
    {
        _serialService = new SerialPortService();
        _responseBuffer = new StringBuilder();

        // 起動画面書き込みViewModelを初期化
        StartupImageViewModel = new StartupImageViewModel(_serialService);

        // イベント登録
        _serialService.DataReceived += SerialService_DataReceived;
        _serialService.ConnectionChanged += SerialService_ConnectionChanged;
        _serialService.ErrorOccurred += SerialService_ErrorOccurred;

        // 初期値
        BaudRates = [9600, 19200, 38400, 57600, 115200, 230400, 460800];
        SelectedBaudRate = 115200;

        // COMポート一覧を取得
        RefreshPorts();
    }

    #region Properties

    [ObservableProperty]
    private ObservableCollection<string> _availablePorts = [];

    [ObservableProperty]
    private string? _selectedPort;

    [ObservableProperty]
    private int[] _baudRates = [];

    [ObservableProperty]
    private int _selectedBaudRate;

    [ObservableProperty]
    private bool _isConnected;

    [ObservableProperty]
    private bool _isBootloaderMode;

    partial void OnIsBootloaderModeChanged(bool value)
    {
        OnPropertyChanged(nameof(IsFirmwareMode));
        OnPropertyChanged(nameof(IsOperationEnabled));
        OnPropertyChanged(nameof(CanStartFirmwareUpdate));
    }

    /// <summary>
    /// Firmwareモードかどうか（Bootloaderモードでない）
    /// </summary>
    public bool IsFirmwareMode => IsConnected && !IsBootloaderMode;

    partial void OnIsConnectedChanged(bool value)
    {
        OnPropertyChanged(nameof(IsNotConnected));
        OnPropertyChanged(nameof(ConnectionButtonText));
        OnPropertyChanged(nameof(CanStartFirmwareUpdate));
        OnPropertyChanged(nameof(IsOperationEnabled));
        OnPropertyChanged(nameof(IsFirmwareMode));
    }

    [ObservableProperty]
    private string _statusText = "未接続";

    [ObservableProperty]
    private string _activityStatus = "準備完了";

    [ObservableProperty]
    private bool _isDarkTheme;

    partial void OnIsDarkThemeChanged(bool value)
    {
        ThemeService.Instance.IsDarkTheme = value;
    }

    [ObservableProperty]
    private int _txCount;

    [ObservableProperty]
    private int _rxCount;

    // タイヤ設定
    [ObservableProperty]
    private string _tyreWidth = "";

    [ObservableProperty]
    private string _tyreAspect = "";

    [ObservableProperty]
    private string _wheelDia = "";

    // ギア比設定
    [ObservableProperty]
    private string _gear1 = "";

    [ObservableProperty]
    private string _gear2 = "";

    [ObservableProperty]
    private string _gear3 = "";

    [ObservableProperty]
    private string _gear4 = "";

    [ObservableProperty]
    private string _gear5 = "";

    [ObservableProperty]
    private string _gear6 = "";

    [ObservableProperty]
    private string _finalGear = "";

    // ギア比プリセット
    public string[] GearPresets { get; } = ["-- 選択 --", "NA/NB5速", "NB6速"];

    [ObservableProperty]
    private string? _selectedGearPreset = "-- 選択 --";

    partial void OnSelectedGearPresetChanged(string? value)
    {
        if (string.IsNullOrEmpty(value) || value == "-- 選択 --") return;

        if (value == "NA/NB5速")
        {
            Gear1 = "3.163";
            Gear2 = "1.888";
            Gear3 = "1.333";
            Gear4 = "1.000";
            Gear5 = "0.814";
            Gear6 = "0";
        }
        else if (value == "NB6速")
        {
            Gear1 = "3.760";
            Gear2 = "2.269";
            Gear3 = "1.645";
            Gear4 = "1.257";
            Gear5 = "1.000";
            Gear6 = "0.843";
        }
    }

    // 警告設定
    [ObservableProperty]
    private string _waterTempLow = "";

    [ObservableProperty]
    private string _waterTempHigh = "";

    [ObservableProperty]
    private string _fuelWarn = "";

    // シフトインジケータ設定
    [ObservableProperty]
    private string _shiftRpm1 = "";

    [ObservableProperty]
    private string _shiftRpm2 = "";

    [ObservableProperty]
    private string _shiftRpm3 = "";

    [ObservableProperty]
    private string _shiftRpm4 = "";

    [ObservableProperty]
    private string _shiftRpm5 = "";

    // ODO/TRIP (表示のみ)
    [ObservableProperty]
    private string _odoValue = "";

    [ObservableProperty]
    private string _tripValue = ""; // ファームウェアからのTRIP値（既に差分計算済み）

    // RTC (表示のみ)
    [ObservableProperty]
    private string _rtcValue = "";

    // ファームウェアバージョン
    [ObservableProperty]
    private string _firmwareVersion = "";

    // ファームウェア更新関連
    [ObservableProperty]
    private string _firmwareFilePath = "";

    partial void OnFirmwareFilePathChanged(string value)
    {
        OnPropertyChanged(nameof(CanStartFirmwareUpdate));
    }

    [ObservableProperty]
    private int _firmwareProgress;

    [ObservableProperty]
    private string _firmwareStatus = "待機中";

    [ObservableProperty]
    private string _firmwareLog = "";

    [ObservableProperty]
    private bool _isFirmwareUpdating;

    partial void OnIsFirmwareUpdatingChanged(bool value)
    {
        OnPropertyChanged(nameof(CanStartFirmwareUpdate));
        OnPropertyChanged(nameof(IsOperationEnabled));
        OnPropertyChanged(nameof(CanCloseWindow));
    }

    public bool CanStartFirmwareUpdate => IsConnected && !string.IsNullOrEmpty(FirmwareFilePath) && !IsFirmwareUpdating;

    public bool IsNotConnected => !IsConnected;

    /// <summary>
    /// 操作が可能かどうか（接続中かFirmwareモードかつファームウェア更新中でない）
    /// </summary>
    public bool IsOperationEnabled => IsFirmwareMode && !IsFirmwareUpdating;

    /// <summary>
    /// ウィンドウを閉じられるかどうか
    /// </summary>
    public bool CanCloseWindow => !IsFirmwareUpdating;

    public string ConnectionButtonText => IsConnected ? "切断" : "接続";

    #endregion

    #region Commands

    [RelayCommand]
    private void RefreshPorts()
    {
        var ports = SerialPortService.GetAvailablePorts();
        AvailablePorts.Clear();
        foreach (var port in ports)
        {
            AvailablePorts.Add(port);
        }

        if (AvailablePorts.Count > 0 && SelectedPort == null)
        {
            SelectedPort = AvailablePorts[0];
        }

        ActivityStatus = $"{ports.Length} 個のCOMポートを検出";
    }

    [RelayCommand]
    private async Task Connect()
    {
        if (IsConnected)
        {
            _serialService.Disconnect();
        }
        else
        {
            if (string.IsNullOrEmpty(SelectedPort))
            {
                ActivityStatus = "⚠️ COMポートを選択してください";
                return;
            }

            ActivityStatus = $"接続中... {SelectedPort}";

            if (_serialService.Connect(SelectedPort, SelectedBaudRate))
            {
                ActivityStatus = "✅ 接続完了！モードを確認中...";
                // 接続後、BootloaderかFirmwareかを判定
                await DetectConnectionMode();
            }
        }
    }

    /// <summary>
    /// 接続先がBootloaderかFirmwareかを検出
    /// </summary>
    private async Task DetectConnectionMode()
    {
        _responseBuffer.Clear();
        IsBootloaderMode = false;

        // Bootloaderにキー送信してメニューを表示させる
        _serialService.SendCommand("\r");
        TxCount++;

        // 少し待ってデータを受信
        await Task.Delay(800);

        var received = _responseBuffer.ToString();

        // デバッグ: 受信内容をログ出力
        System.Diagnostics.Debug.WriteLine($"[DetectConnectionMode] Received ({received.Length} chars): {received.Replace("\r", "\\r").Replace("\n", "\\n")}");

        // Bootloaderの特徴的な文字列をチェック
        // 実際のBootloader出力: "=== FULLMONI Bootloader ===" "U=Update B=Boot R=Reset S=Status"
        if (received.Contains("Bootloader") ||
            received.Contains("U=Update") ||
            received.Contains("B=Boot") ||
            received.Contains("S=Status"))
        {
            IsBootloaderMode = true;
            StatusText = $"Bootloaderモード ({SelectedPort})";
            ActivityStatus = "⚠️ Bootloaderモードで接続 - ファームウェア更新タブを使用してください";
            System.Diagnostics.Debug.WriteLine("[DetectConnectionMode] -> Bootloader mode detected!");
            return;
        }

        // Firmwareとして接続 - パラメータ取得
        System.Diagnostics.Debug.WriteLine("[DetectConnectionMode] -> Firmware mode, loading parameters...");
        ActivityStatus = "✅ Firmwareモード - パラメータを取得中...";
        await LoadParametersInternal();
    }

    [RelayCommand]
    private async Task LoadParameters()
    {
        if (!IsConnected) return;
        await LoadParametersInternal();
    }

    /// <summary>
    /// パラメータ読込の内部実装（キー送信→list→exit）
    /// </summary>
    private async Task LoadParametersInternal()
    {
        _isLoadingParameters = true;
        ActivityStatus = "📥 パラメータを読み込み中...";

        try
        {
            _responseBuffer.Clear();

            // キー送信でパラメータモードに入る（任意のキー）
            _serialService.SendCommand("");
            TxCount++;
            await Task.Delay(500);

            // versionコマンドでバージョン取得
            _responseBuffer.Clear();
            _serialService.SendCommand("version");
            TxCount++;
            await Task.Delay(500);

            var versionResponse = _responseBuffer.ToString();
            ParseVersionResponse(versionResponse);

            // listコマンドを送信
            _responseBuffer.Clear();
            _serialService.SendCommand("list");
            TxCount++;
            await Task.Delay(1000); // レスポンス収集のため待機

            var response = _responseBuffer.ToString();
            ParseParameterResponse(response);

            // rtcコマンドでRTC取得
            _responseBuffer.Clear();
            _serialService.SendCommand("rtc");
            TxCount++;
            await Task.Delay(500);

            var rtcResponse = _responseBuffer.ToString();
            ParseRtcResponse(rtcResponse);

            // exitコマンドで通信待ちを終了
            _serialService.SendCommand("exit");
            TxCount++;
            await Task.Delay(100);

            ActivityStatus = "✅ パラメータを読み込みました";
        }
        catch (Exception ex)
        {
            ActivityStatus = $"❌ 読み込みエラー: {ex.Message}";
        }
        finally
        {
            _isLoadingParameters = false;
            _responseWaiter = null;
        }
    }

    [RelayCommand]
    private async Task SaveParameters()
    {
        if (!IsConnected) return;

        // 入力バリデーション
        var validationErrors = ValidateAllParameters();
        if (validationErrors.Count > 0)
        {
            var errorMessage = "入力エラーがあります。以下を確認してください:\n\n" + string.Join("\n", validationErrors);
            System.Windows.MessageBox.Show(errorMessage, "入力エラー", System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Warning);
            return;
        }

        ActivityStatus = "📤 パラメータを送信中...";

        try
        {
            // キー送信でパラメータモードに入る
            _serialService.SendCommand("");
            TxCount++;
            await Task.Delay(500);

            // タイヤ設定
            await SendParameterIfValid("tyre_width", TyreWidth);
            await SendParameterIfValid("tyre_aspect", TyreAspect);
            await SendParameterIfValid("tyre_rim", WheelDia);

            // ギア比設定
            await SendGearRatioIfValid("gear1", Gear1);
            await SendGearRatioIfValid("gear2", Gear2);
            await SendGearRatioIfValid("gear3", Gear3);
            await SendGearRatioIfValid("gear4", Gear4);
            await SendGearRatioIfValid("gear5", Gear5);
            await SendGearRatioIfValid("gear6", Gear6);
            await SendGearRatioIfValid("final", FinalGear);

            // 警告設定
            await SendParameterIfValid("water_low", WaterTempLow);
            await SendParameterIfValid("water_high", WaterTempHigh);
            await SendParameterIfValid("fuel_warn", FuelWarn);

            // シフトインジケータ設定
            await SendParameterIfValid("shift_rpm1", ShiftRpm1);
            await SendParameterIfValid("shift_rpm2", ShiftRpm2);
            await SendParameterIfValid("shift_rpm3", ShiftRpm3);
            await SendParameterIfValid("shift_rpm4", ShiftRpm4);
            await SendParameterIfValid("shift_rpm5", ShiftRpm5);

            // 保存コマンド
            _serialService.SendCommand("save");
            TxCount++;
            await Task.Delay(300);

            // exitコマンドで通信待ちを終了
            _serialService.SendCommand("exit");
            TxCount++;

            ActivityStatus = "✅ パラメータを保存しました";
        }
        catch (Exception ex)
        {
            ActivityStatus = $"❌ 保存エラー: {ex.Message}";
        }
    }

    [RelayCommand]
    private async Task ResetDefault()
    {
        if (!IsConnected) return;

        // UIスレッドで確認ダイアログを表示
        var result = await Application.Current.Dispatcher.InvokeAsync(() =>
            MessageBox.Show(
                Application.Current.MainWindow,
                "すべてのパラメータをデフォルト値にリセットしますか？",
                "確認",
                MessageBoxButton.YesNo,
                MessageBoxImage.Question));

        if (result != MessageBoxResult.Yes) return;

        ActivityStatus = "🔄 デフォルト値にリセット中...";

        try
        {
            await Task.Run(async () =>
            {
                // キー送信でパラメータモードに入る
                _serialService.SendCommand("");
                await Task.Delay(500);

                // defaultコマンド
                _serialService.SendCommand("default");
                await Task.Delay(300);

                // 変更後の値を取得
                _responseBuffer.Clear();
                _serialService.SendCommand("list");
                await Task.Delay(1000);

                // exitコマンドで通信待ちを終了
                _serialService.SendCommand("exit");
            });

            TxCount += 4;
            var response = _responseBuffer.ToString();
            ParseParameterResponse(response);

            ActivityStatus = "✅ デフォルト値にリセットしました";
        }
        catch (Exception ex)
        {
            ActivityStatus = $"❌ リセットエラー: {ex.Message}";
        }
    }

    [RelayCommand]
    private async Task ResetTrip()
    {
        if (!IsConnected) return;

        // UIスレッドで確認ダイアログを表示
        var result = await Application.Current.Dispatcher.InvokeAsync(() =>
            MessageBox.Show(
                Application.Current.MainWindow,
                "TRIPをリセットしますか？",
                "確認",
                MessageBoxButton.YesNo,
                MessageBoxImage.Question));

        if (result != MessageBoxResult.Yes) return;

        ActivityStatus = "🔄 TRIPリセット中...";

        try
        {
            await Task.Run(async () =>
            {
                // キー送信でパラメータモードに入る
                _serialService.SendCommand("");
                await Task.Delay(500);

                // trip_resetコマンド
                _serialService.SendCommand("trip_reset");
                await Task.Delay(300);

                // saveコマンドでフラッシュに保存
                _serialService.SendCommand("save");
                await Task.Delay(500);

                // 変更後の値を取得
                _responseBuffer.Clear();
                _serialService.SendCommand("list");
                await Task.Delay(1000);

                // exitコマンドで通信待ちを終了
                _serialService.SendCommand("exit");
            });

            TxCount += 5;
            var response = _responseBuffer.ToString();
            ParseParameterResponse(response);

            ActivityStatus = "✅ TRIPをリセットしました";
        }
        catch (Exception ex)
        {
            ActivityStatus = $"❌ TRIPリセットエラー: {ex.Message}";
        }
    }

    [RelayCommand]
    private async Task SyncRtc()
    {
        if (!IsConnected) return;

        ActivityStatus = "🕐 RTC同期中...";

        try
        {
            // キー送信でパラメータモードに入る
            _serialService.SendCommand("");
            TxCount++;
            await Task.Delay(500);

            // Windows時刻を取得してrtcコマンド発行
            var now = DateTime.Now;
            var rtcCommand = $"rtc {now:yy MM dd HH mm ss}";
            _serialService.SendCommand(rtcCommand);
            TxCount++;
            await Task.Delay(300);

            // RTC値を再取得
            _responseBuffer.Clear();
            _serialService.SendCommand("rtc");
            TxCount++;
            await Task.Delay(500);

            var rtcResponse = _responseBuffer.ToString();
            ParseRtcResponse(rtcResponse);

            // exitコマンドで通信待ちを終了
            _serialService.SendCommand("exit");
            TxCount++;

            ActivityStatus = $"✅ RTCを同期しました ({now:yy/MM/dd HH:mm:ss})";
        }
        catch (Exception ex)
        {
            ActivityStatus = $"❌ RTC同期エラー: {ex.Message}";
        }
    }

    [RelayCommand]
    private void BrowseFirmware()
    {
        var dialog = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "BINファイル (*.bin)|*.bin|すべてのファイル (*.*)|*.*",
            Title = "ファームウェアファイルを選択"
        };

        if (dialog.ShowDialog() == true)
        {
            FirmwareFilePath = dialog.FileName;
            FirmwareStatus = "ファイルを選択しました";
            OnPropertyChanged(nameof(CanStartFirmwareUpdate));
        }
    }

    [RelayCommand]
    private void ClearFirmwareLog()
    {
        FirmwareLog = "";
    }

    [RelayCommand]
    private void CopyFirmwareLog()
    {
        if (!string.IsNullOrEmpty(FirmwareLog))
        {
            System.Windows.Clipboard.SetText(FirmwareLog);
            ActivityStatus = "ログをクリップボードにコピーしました";
        }
    }

    private void AppendFirmwareLog(string message)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            var timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
            FirmwareLog += $"[{timestamp}] {message}\n";
        });
    }

    [RelayCommand]
    private async Task StartFirmwareUpdate()
    {
        if (!IsConnected || string.IsNullOrEmpty(FirmwareFilePath)) return;

        // Bootloaderモードか確認し、メッセージを変更
        var message = IsBootloaderMode
            ? "ファームウェアの更新を開始しますか？\n\n" +
              "【Bootloaderモード】\n" +
              "• 更新中は絶対に電源を切らないでください\n" +
              "• 更新完了後、デバイスは自動的に再起動します"
            : "ファームウェアの更新を開始しますか？\n\n" +
              "【重要】\n" +
              "• 更新中は絶対に電源を切らないでください\n" +
              "• デバイスがBootloaderモードに切り替わります\n" +
              "• 更新完了後、デバイスは自動的に再起動します";

        var result = await Application.Current.Dispatcher.InvokeAsync(() =>
            MessageBox.Show(
                Application.Current.MainWindow,
                message,
                "ファームウェア更新",
                MessageBoxButton.YesNo,
                MessageBoxImage.Warning));

        if (result != MessageBoxResult.Yes) return;

        IsFirmwareUpdating = true;
        FirmwareProgress = 0;
        FirmwareStatus = "ファイルを読み込み中...";
        FirmwareLog = "";
        ActivityStatus = "🔄 ファームウェア更新中...";

        try
        {
            // BINファイルを読み込む
            AppendFirmwareLog($"ファイル読み込み: {FirmwareFilePath}");
            var firmwareData = await Task.Run(() =>
                Services.FirmwareUpdateService.LoadBinFile(FirmwareFilePath));

            AppendFirmwareLog($"ファイルサイズ: {firmwareData.Length:N0} bytes");
            FirmwareStatus = $"ファイル読み込み完了: {firmwareData.Length:N0} bytes";

            // ファームウェア更新サービスを作成
            var updateService = new Services.FirmwareUpdateService(_serialService);

            // イベントハンドラを設定
            updateService.ProgressChanged += (s, progress) =>
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    FirmwareProgress = progress;
                });
            };

            updateService.StatusChanged += (s, status) =>
            {
                Application.Current.Dispatcher.Invoke(() =>
                {
                    FirmwareStatus = status;
                });
            };

            updateService.LogMessage += (s, message) =>
            {
                AppendFirmwareLog(message);
            };

            updateService.UpdateCompleted += async (s, success) =>
            {
                await Application.Current.Dispatcher.InvokeAsync(async () =>
                {
                    IsFirmwareUpdating = false;
                    if (success)
                    {
                        ActivityStatus = "✅ ファームウェア更新完了 - 再接続中...";

                        // デバイス再起動を待つ
                        await Task.Delay(3000);

                        // 現在のポートを切断
                        var currentPort = SelectedPort;
                        if (_serialService.IsConnected)
                        {
                            _serialService.Disconnect();
                        }

                        // 少し待ってから再接続
                        await Task.Delay(1000);

                        // COMポートリストを更新
                        RefreshPorts();

                        // 同じポートに再接続を試みる
                        if (!string.IsNullOrEmpty(currentPort) && AvailablePorts.Contains(currentPort))
                        {
                            SelectedPort = currentPort;
                            await Task.Delay(500);
                            if (_serialService.Connect(currentPort))
                            {
                                // 再接続後、モードを再検出（Firmwareに戻っているはず）
                                await DetectConnectionMode();
                                ActivityStatus = "✅ ファームウェア更新完了 - 再接続しました";
                            }
                            else
                            {
                                ActivityStatus = "✅ ファームウェア更新完了 - 手動で再接続してください";
                            }
                        }
                        else
                        {
                            ActivityStatus = "✅ ファームウェア更新完了 - 手動で再接続してください";
                        }
                    }
                    else
                    {
                        ActivityStatus = "❌ ファームウェア更新に失敗しました";
                    }
                });
            };

            // ファームウェア転送を開始
            // Bootloaderモードの場合は切り替えをスキップ
            if (IsBootloaderMode)
            {
                FirmwareStatus = "ファームウェア転送を開始中...";
            }
            else
            {
                FirmwareStatus = "ブートローダーモードに切り替え中...";
            }
            await updateService.SendFirmwareAsync(firmwareData, switchToBootloader: !IsBootloaderMode);
        }
        catch (Exception ex)
        {
            IsFirmwareUpdating = false;
            FirmwareStatus = $"エラー: {ex.Message}";
            ActivityStatus = $"❌ ファームウェア更新エラー: {ex.Message}";

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"ファームウェア更新中にエラーが発生しました。\n\n{ex.Message}",
                    "エラー",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error));
        }
    }

    #endregion

    #region Private Methods

    private async Task SendParameterIfValid(string paramName, string value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return;
        }

        _serialService.SendCommand($"set {paramName} {value}");
        TxCount++;
        await Task.Delay(100); // 連続送信の間隔
    }

    private async Task SendGearRatioIfValid(string paramName, string value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return;
        }

        // ギア比は小数で入力されるが、ファームウェアは1000倍の整数で保存
        if (double.TryParse(value, out var ratio))
        {
            var intValue = (int)(ratio * 1000);
            _serialService.SendCommand($"set {paramName} {intValue}");
            TxCount++;
            await Task.Delay(100);
        }
    }

    private void ParseParameterResponse(string response)
    {
        // ファームウェアのlistコマンド出力形式に合わせてパース
        // 例: "Tyre: 195/65 R15"
        //     "Gear1: 3.545  Gear2: 2.033  Gear3: 1.355"
        //     "Gear4: 1.028  Gear5: 0.831  Gear6: 0.636"
        //     "Final: 4.058"
        //     "Water Temp Warning: 0 - 100 C"
        //     "Fuel Warning: 15 %"

        // タイヤ情報: "Tyre: 195/65 R15"
        var tyreMatch = Regex.Match(response, @"Tyre:\s*(\d+)/(\d+)\s*R(\d+)");
        if (tyreMatch.Success)
        {
            TyreWidth = tyreMatch.Groups[1].Value;
            TyreAspect = tyreMatch.Groups[2].Value;
            WheelDia = tyreMatch.Groups[3].Value;
        }

        // ギア比: "Gear1: 3.545" など
        var gear1Match = Regex.Match(response, @"Gear1:\s*([\d.]+)");
        if (gear1Match.Success) Gear1 = gear1Match.Groups[1].Value;

        var gear2Match = Regex.Match(response, @"Gear2:\s*([\d.]+)");
        if (gear2Match.Success) Gear2 = gear2Match.Groups[1].Value;

        var gear3Match = Regex.Match(response, @"Gear3:\s*([\d.]+)");
        if (gear3Match.Success) Gear3 = gear3Match.Groups[1].Value;

        var gear4Match = Regex.Match(response, @"Gear4:\s*([\d.]+)");
        if (gear4Match.Success) Gear4 = gear4Match.Groups[1].Value;

        var gear5Match = Regex.Match(response, @"Gear5:\s*([\d.]+)");
        if (gear5Match.Success) Gear5 = gear5Match.Groups[1].Value;

        var gear6Match = Regex.Match(response, @"Gear6:\s*([\d.]+)");
        if (gear6Match.Success) Gear6 = gear6Match.Groups[1].Value;

        // ファイナルギア: "Final: 4.058"
        var finalMatch = Regex.Match(response, @"Final:\s*([\d.]+)");
        if (finalMatch.Success) FinalGear = finalMatch.Groups[1].Value;

        // 水温警告: "Water Temp Warning: 60 - 100 C"
        var waterMatch = Regex.Match(response, @"Water Temp Warning:\s*(\d+)\s*-\s*(\d+)");
        if (waterMatch.Success)
        {
            WaterTempLow = waterMatch.Groups[1].Value;
            WaterTempHigh = waterMatch.Groups[2].Value;
        }

        // 燃料警告: "Fuel Warning: 10 %"
        var fuelMatch = Regex.Match(response, @"Fuel Warning:\s*(\d+)");
        if (fuelMatch.Success) FuelWarn = fuelMatch.Groups[1].Value;

        // シフトインジケータ: "Shift RPM: 5500 / 6000 / 6500 / 7000 / 7500"
        var shiftMatch = Regex.Match(response, @"Shift RPM:\s*(\d+)\s*/\s*(\d+)\s*/\s*(\d+)\s*/\s*(\d+)\s*/\s*(\d+)");
        if (shiftMatch.Success)
        {
            ShiftRpm1 = shiftMatch.Groups[1].Value;
            ShiftRpm2 = shiftMatch.Groups[2].Value;
            ShiftRpm3 = shiftMatch.Groups[3].Value;
            ShiftRpm4 = shiftMatch.Groups[4].Value;
            ShiftRpm5 = shiftMatch.Groups[5].Value;
        }

        // ODO/TRIP: "ODO: 106894 km  TRIP: 250.5 km"
        var odoMatch = Regex.Match(response, @"ODO:\s*([\d.]+)\s*km");
        if (odoMatch.Success)
        {
            OdoValue = odoMatch.Groups[1].Value;
        }

        var tripMatch = Regex.Match(response, @"TRIP:\s*([\d.]+)\s*km");
        if (tripMatch.Success)
        {
            TripValue = tripMatch.Groups[1].Value;
        }
    }

    private void ParseVersionResponse(string response)
    {
        // VERSION出力形式: "VERSION 1.0.0"
        var versionMatch = Regex.Match(response, @"VERSION\s+(\d+\.\d+\.\d+)");
        if (versionMatch.Success)
        {
            FirmwareVersion = versionMatch.Groups[1].Value;
        }
    }

    private void ParseRtcResponse(string response)
    {
        // RTC出力形式: "RTC: 26/01/07 12:34:56" のような形式を想定
        var rtcMatch = Regex.Match(response, @"RTC:\s*(.+)");
        if (rtcMatch.Success)
        {
            RtcValue = rtcMatch.Groups[1].Value.Trim();
        }
        else
        {
            // 別の形式: "20YY/MM/DD HH:MM:SS"
            var dateMatch = Regex.Match(response, @"(\d{2,4}[/-]\d{2}[/-]\d{2}\s+\d{2}:\d{2}:\d{2})");
            if (dateMatch.Success)
            {
                RtcValue = dateMatch.Groups[1].Value;
            }
        }
    }

    private void SerialService_DataReceived(object? sender, string e)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            RxCount++;
            _responseBuffer.Append(e);
            _responseWaiter?.TrySetResult(e);
        });
    }

    private void SerialService_ConnectionChanged(object? sender, bool connected)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            IsConnected = connected;
            OnPropertyChanged(nameof(IsNotConnected));
            OnPropertyChanged(nameof(ConnectionButtonText));

            if (connected)
            {
                StatusText = $"接続中 ({SelectedPort})";
                TxCount = 0;
                RxCount = 0;
            }
            else
            {
                StatusText = "未接続";
                // Bootloaderモードをリセット
                IsBootloaderMode = false;
                // ファームウェア更新中は「切断されました」を表示しない（Bootloader切替時に一時的に切断されるため）
                if (!IsFirmwareUpdating)
                {
                    ActivityStatus = "切断されました";
                }
            }
        });
    }

    private void SerialService_ErrorOccurred(object? sender, string e)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            ActivityStatus = $"❌ エラー: {e}";
        });
    }

    #endregion

    #region Validation

    private List<string> ValidateAllParameters()
    {
        var errors = new List<string>();

        // === タイヤ設定バリデーション ===
        // 正の整数で最大500まで
        if (!ValidatePositiveInt(TyreWidth, 1, 500, out _))
            errors.Add("【タイヤ幅】正の整数で1～500の範囲で入力してください");
        if (!ValidatePositiveInt(TyreAspect, 1, 500, out _))
            errors.Add("【扁平率】正の整数で1～500の範囲で入力してください");
        if (!ValidatePositiveInt(WheelDia, 1, 500, out _))
            errors.Add("【リム径】正の整数で1～500の範囲で入力してください");

        // === 警告設定バリデーション ===
        // 温度: -40～200
        if (!ValidateInt(WaterTempLow, -40, 200, out int waterLow))
            errors.Add("【水温低温】-40～200の整数で入力してください");
        if (!ValidateInt(WaterTempHigh, -40, 200, out int waterHigh))
            errors.Add("【水温高温】-40～200の整数で入力してください");
        if (waterLow >= waterHigh && !string.IsNullOrEmpty(WaterTempLow) && !string.IsNullOrEmpty(WaterTempHigh))
            errors.Add("【水温警告】水温低温 < 水温高温 の関係にしてください");

        // 燃料: 0～1000
        if (!ValidatePositiveInt(FuelWarn, 0, 1000, out _))
            errors.Add("【燃料警告】0～1000の整数で入力してください");

        // === ギア比バリデーション ===
        // 正の小数で、1速>2速>3速>4速>5速>6速の関係
        bool gear1Valid = ValidatePositiveDecimal(Gear1, out double g1);
        bool gear2Valid = ValidatePositiveDecimal(Gear2, out double g2);
        bool gear3Valid = ValidatePositiveDecimal(Gear3, out double g3);
        bool gear4Valid = ValidatePositiveDecimal(Gear4, out double g4);
        bool gear5Valid = ValidatePositiveDecimal(Gear5, out double g5);
        bool gear6Valid = ValidatePositiveDecimal(Gear6, out double g6);

        if (!gear1Valid) errors.Add("【1速】正の小数で入力してください");
        if (!gear2Valid) errors.Add("【2速】正の小数で入力してください");
        if (!gear3Valid) errors.Add("【3速】正の小数で入力してください");
        if (!gear4Valid) errors.Add("【4速】正の小数で入力してください");
        if (!gear5Valid) errors.Add("【5速】正の小数で入力してください");
        // 6速は0許可（5速車の場合）
        if (!string.IsNullOrEmpty(Gear6) && !gear6Valid && Gear6 != "0")
            errors.Add("【6速】正の小数で入力してください（5速車は0）");

        if (!ValidatePositiveDecimal(FinalGear, out _))
            errors.Add("【ファイナル】正の小数で入力してください");

        // ギア比の大小関係チェック（入力がある場合のみ）
        if (gear1Valid && gear2Valid && g1 <= g2)
            errors.Add("【ギア比】1速 > 2速 の関係にしてください");
        if (gear2Valid && gear3Valid && g2 <= g3)
            errors.Add("【ギア比】2速 > 3速 の関係にしてください");
        if (gear3Valid && gear4Valid && g3 <= g4)
            errors.Add("【ギア比】3速 > 4速 の関係にしてください");
        if (gear4Valid && gear5Valid && g4 <= g5)
            errors.Add("【ギア比】4速 > 5速 の関係にしてください");
        if (gear5Valid && gear6Valid && g6 > 0 && g5 <= g6)
            errors.Add("【ギア比】5速 > 6速 の関係にしてください");

        // === シフトインジケータバリデーション ===
        // 正の整数で最大15000まで、昇順関係
        bool rpm1Valid = ValidatePositiveInt(ShiftRpm1, 1, 15000, out int rpm1);
        bool rpm2Valid = ValidatePositiveInt(ShiftRpm2, 1, 15000, out int rpm2);
        bool rpm3Valid = ValidatePositiveInt(ShiftRpm3, 1, 15000, out int rpm3);
        bool rpm4Valid = ValidatePositiveInt(ShiftRpm4, 1, 15000, out int rpm4);
        bool rpm5Valid = ValidatePositiveInt(ShiftRpm5, 1, 15000, out int rpm5);

        if (!rpm1Valid) errors.Add("【青2灯】正の整数で1～15000の範囲で入力してください");
        if (!rpm2Valid) errors.Add("【青4灯】正の整数で1～15000の範囲で入力してください");
        if (!rpm3Valid) errors.Add("【緑6灯】正の整数で1～15000の範囲で入力してください");
        if (!rpm4Valid) errors.Add("【赤8灯】正の整数で1～15000の範囲で入力してください");
        if (!rpm5Valid) errors.Add("【白点滅】正の整数で1～15000の範囲で入力してください");

        // シフトRPMの大小関係チェック
        if (rpm1Valid && rpm2Valid && rpm1 >= rpm2)
            errors.Add("【シフトインジケータ】青2灯 < 青4灯 の関係にしてください");
        if (rpm2Valid && rpm3Valid && rpm2 >= rpm3)
            errors.Add("【シフトインジケータ】青4灯 < 緑6灯 の関係にしてください");
        if (rpm3Valid && rpm4Valid && rpm3 >= rpm4)
            errors.Add("【シフトインジケータ】緑6灯 < 赤8灯 の関係にしてください");
        if (rpm4Valid && rpm5Valid && rpm4 >= rpm5)
            errors.Add("【シフトインジケータ】赤8灯 < 白点滅 の関係にしてください");

        return errors;
    }

    private bool ValidatePositiveInt(string value, int min, int max, out int result)
    {
        result = 0;
        if (string.IsNullOrEmpty(value)) return false;
        if (!int.TryParse(value, out result)) return false;
        return result >= min && result <= max;
    }

    private bool ValidateInt(string value, int min, int max, out int result)
    {
        result = 0;
        if (string.IsNullOrEmpty(value)) return false;
        if (!int.TryParse(value, out result)) return false;
        return result >= min && result <= max;
    }

    private bool ValidatePositiveDecimal(string value, out double result)
    {
        result = 0;
        if (string.IsNullOrEmpty(value)) return false;
        if (!double.TryParse(value, out result)) return false;
        return result > 0;
    }

    private bool ValidateDecimalRange(string value, double min, double max, out double result)
    {
        result = 0;
        if (string.IsNullOrEmpty(value)) return false;
        if (!double.TryParse(value, out result)) return false;
        return result >= min && result <= max;
    }

    #endregion

    #region IDisposable

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
                _serialService.DataReceived -= SerialService_DataReceived;
                _serialService.ConnectionChanged -= SerialService_ConnectionChanged;
                _serialService.ErrorOccurred -= SerialService_ErrorOccurred;
                _serialService.Dispose();
                StartupImageViewModel.Dispose();
            }
            _disposed = true;
        }
    }

    ~MainViewModel()
    {
        Dispose(false);
    }

    #endregion
}
