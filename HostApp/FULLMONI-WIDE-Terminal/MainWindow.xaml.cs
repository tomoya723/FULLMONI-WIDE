using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;
using System.Windows.Threading;
using Wpf.Ui.Controls;
using FullmoniTerminal.ViewModels;
using WpfTextBox = System.Windows.Controls.TextBox;
using SystemMsgBox = System.Windows.MessageBox;
using SystemMsgButton = System.Windows.MessageBoxButton;
using SystemMsgImage = System.Windows.MessageBoxImage;
using SystemMsgResult = System.Windows.MessageBoxResult;

namespace FullmoniTerminal
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : FluentWindow
    {
        private readonly Dictionary<string, StackPanel> _pages;
        private readonly Dictionary<string, Wpf.Ui.Controls.Button> _navButtons;
        private string _currentPage = "home";
        private ObservableCollection<CanFieldItem> _canFields = new();
        private DispatcherTimer? _heroImageTimer;
        private bool _showingImage1 = true;

        public MainWindow()
        {
            InitializeComponent();

            // ヒーロー画像切り替えタイマーを開始
            StartHeroImageAnimation();

            // ページ辞書を初期化
            _pages = new Dictionary<string, StackPanel>
            {
                { "home", HomePage },
                { "status", StatusPage },
                { "settings", SettingsPage },
                { "can", CanPage },
                { "startup", StartupPage },
                { "firmware", FirmwarePage },
                { "about", AboutPage }
            };

            // ナビゲーションボタン辞書を初期化
            _navButtons = new Dictionary<string, Wpf.Ui.Controls.Button>
            {
                { "home", NavHome },
                { "status", NavStatus },
                { "settings", NavSettings },
                { "can", NavCan },
                { "startup", NavStartup },
                { "firmware", NavFirmware },
                { "about", NavAbout }
            };

            // 初期ページを表示（ホームページをデフォルトで表示）
            ShowPage("home");

            // CANフィールドを初期化
            InitializeCanFields();

            // StartupImageViewのDataContextを設定
            SetupStartupImageView();

            // ViewModelのイベントを購読（接続後にCAN設定を自動読み込み）
            System.Diagnostics.Debug.WriteLine($"DataContext type: {DataContext?.GetType().Name ?? "null"}");
            if (DataContext is MainViewModel vm)
            {
                System.Diagnostics.Debug.WriteLine("Subscribing to ViewModel events");
                vm.ParametersLoaded += ViewModel_ParametersLoaded;
                vm.PropertyChanged += ViewModel_PropertyChanged;
            }
            else
            {
                System.Diagnostics.Debug.WriteLine("DataContext is NOT MainViewModel at constructor");
            }

            // DataContextが後から設定される場合に備える
            DataContextChanged += (s, e) =>
            {
                System.Diagnostics.Debug.WriteLine($"DataContextChanged: {e.NewValue?.GetType().Name ?? "null"}");

                if (e.OldValue is MainViewModel oldVm)
                {
                    oldVm.ParametersLoaded -= ViewModel_ParametersLoaded;
                    oldVm.PropertyChanged -= ViewModel_PropertyChanged;
                }
                if (e.NewValue is MainViewModel newVm)
                {
                    newVm.ParametersLoaded += ViewModel_ParametersLoaded;
                    newVm.PropertyChanged += ViewModel_PropertyChanged;
                    // StartupImageViewのDataContextも更新
                    SetupStartupImageView();
                }
            };
        }

        /// <summary>
        /// StartupImageViewのDataContextを設定
        /// </summary>
        private void SetupStartupImageView()
        {
            if (DataContext is MainViewModel vm)
            {
                StartupImageViewControl.DataContext = new StartupImageViewModel(vm.SerialService);
            }
        }

        /// <summary>
        /// ヒーロー画像の切り替えアニメーションを開始
        /// </summary>
        private void StartHeroImageAnimation()
        {
            _heroImageTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(5)
            };
            _heroImageTimer.Tick += HeroImageTimer_Tick;
            _heroImageTimer.Start();
        }

        /// <summary>
        /// ヒーロー画像を切り替え
        /// </summary>
        private void HeroImageTimer_Tick(object? sender, EventArgs e)
        {
            var duration = new Duration(TimeSpan.FromSeconds(1.5));

            if (_showingImage1)
            {
                // FM3 → FM2 に切り替え
                var fadeOut = new DoubleAnimation(0.35, 0, duration);
                var fadeIn = new DoubleAnimation(0, 0.35, duration);
                HeroImage1.BeginAnimation(OpacityProperty, fadeOut);
                HeroImage2.BeginAnimation(OpacityProperty, fadeIn);
            }
            else
            {
                // FM2 → FM3 に切り替え
                var fadeOut = new DoubleAnimation(0.35, 0, duration);
                var fadeIn = new DoubleAnimation(0, 0.35, duration);
                HeroImage2.BeginAnimation(OpacityProperty, fadeOut);
                HeroImage1.BeginAnimation(OpacityProperty, fadeIn);
            }

            _showingImage1 = !_showingImage1;
        }

        /// <summary>
        /// ViewModelのプロパティ変更時
        /// </summary>
        private void ViewModel_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(MainViewModel.IsConnected))
            {
                Dispatcher.Invoke(UpdateHeroConnectButton);
            }
        }

        /// <summary>
        /// ヒーロー接続ボタンの表示を更新
        /// </summary>
        private void UpdateHeroConnectButton()
        {
            if (DataContext is MainViewModel vm)
            {
                System.Diagnostics.Debug.WriteLine($"UpdateHeroConnectButton: IsConnected = {vm.IsConnected}");
                if (vm.IsConnected)
                {
                    HeroConnectButton.Content = "Disconnect";
                    HeroConnectButton.Icon = new Wpf.Ui.Controls.SymbolIcon(Wpf.Ui.Controls.SymbolRegular.PlugDisconnected24);
                }
                else
                {
                    HeroConnectButton.Content = "Connect to Device";
                    HeroConnectButton.Icon = new Wpf.Ui.Controls.SymbolIcon(Wpf.Ui.Controls.SymbolRegular.PlugConnected24);
                }
            }
        }

        /// <summary>
        /// パラメータ読み込み完了時にCAN設定も読み込む
        /// </summary>
        private async void ViewModel_ParametersLoaded(object? sender, EventArgs e)
        {
            // CAN設定を自動で読み込む
            await LoadCanSettingsAsync();
        }

        /// <summary>
        /// CAN設定を非同期で読み込む
        /// </summary>
        private async Task LoadCanSettingsAsync()
        {
            if (DataContext is not MainViewModel vm || !vm.IsConnected)
                return;

            try
            {
                CanStatusText.Text = "Loading CAN settings...";

                // パラメータモードに入る（前の処理でexitしているので再度入る）
                vm.ClearResponseBuffer();
                await vm.SendCommandAndGetResponseAsync("", 800);

                // can_list コマンドを送信して応答を取得
                vm.ClearResponseBuffer();
                var response = await vm.SendCommandAndGetResponseAsync("can_list", 5000);

                // パラメータモードを終了
                await vm.SendCommandAndGetResponseAsync("exit", 500);

                if (string.IsNullOrEmpty(response))
                {
                    CanStatusText.Text = "CAN Config: No response";
                    return;
                }

                // パース処理
                ParseCanResponse(response);

                CanStatusText.Text = "CAN settings loaded";
            }
            catch (Exception ex)
            {
                CanStatusText.Text = $"CAN load error: {ex.Message}";
            }
        }

        /// <summary>
        /// ナビゲーションボタンのクリックハンドラ
        /// </summary>
        private void NavButton_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Wpf.Ui.Controls.Button button && button.Tag is string tag)
            {
                ShowPage(tag);
            }
        }

        /// <summary>
        /// ナビゲーションカードのクリックハンドラ
        /// </summary>
        private void NavCard_Click(object sender, RoutedEventArgs e)
        {
            if (sender is Wpf.Ui.Controls.CardAction card && card.Tag is string tag)
            {
                ShowPage(tag);
            }
            else if (sender is Wpf.Ui.Controls.Button button && button.Tag is string buttonTag)
            {
                ShowPage(buttonTag);
            }
        }

        /// <summary>
        /// ヒーロー接続ボタンのクリックハンドラ
        /// </summary>
        private void HeroConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (DataContext is MainViewModel vm)
            {
                vm.ConnectCommand.Execute(null);
            }
        }

        /// <summary>
        /// 指定されたページを表示
        /// </summary>
        private void ShowPage(string pageName)
        {
            // すべてのページを非表示
            foreach (var page in _pages.Values)
            {
                page.Visibility = Visibility.Collapsed;
            }

            // 指定ページを表示
            if (_pages.TryGetValue(pageName, out var targetPage))
            {
                targetPage.Visibility = Visibility.Visible;
            }

            // ボタンの外観を更新
            foreach (var kvp in _navButtons)
            {
                kvp.Value.Appearance = kvp.Key == pageName
                    ? ControlAppearance.Primary
                    : ControlAppearance.Secondary;
            }

            _currentPage = pageName;
        }

        /// <summary>
        /// ウィンドウ閉じる時のイベント
        /// </summary>
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (DataContext is MainViewModel vm)
            {
                if (!vm.CanCloseWindow)
                {
                    e.Cancel = true;
                    SystemMsgBox.Show("処理中は終了できません", "FULLMONI-WIDE Terminal",
                        SystemMsgButton.OK, SystemMsgImage.Warning);
                    return;
                }

                // シリアル接続を切断
                vm.ConnectCommand.Execute(null);
            }
        }

        /// <summary>
        /// ファームウェアログのテキスト変更時にスクロール
        /// </summary>
        private void FirmwareLogTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (sender is WpfTextBox textBox)
            {
                textBox.ScrollToEnd();
            }
        }

        #region CAN設定関連

        /// <summary>
        /// CANフィールドを初期化
        /// </summary>
        private void InitializeCanFields()
        {
            _canFields.Clear();

            for (int i = 0; i < 16; i++)
            {
                _canFields.Add(new CanFieldItem
                {
                    Index = i + 1,
                    Enabled = false,
                    Channel = 0,
                    StartByte = 0,
                    ByteCount = 2,
                    DataType = "U",
                    Endian = "B",
                    TargetVarName = "USER1",
                    Offset = 0,
                    Multiplier = 1.0,
                    Divisor = 1.0,
                    Decimals = 0,
                    Name = "",
                    Unit = "",
                    WarnLowEnabled = false,
                    WarnLow = "---",
                    WarnHighEnabled = false,
                    WarnHigh = "---"
                });
            }

            CanFieldsGrid.ItemsSource = _canFields;
            CanFieldsGrid.Items.Refresh();
        }

        /// <summary>
        /// CAN設定読み込みボタン
        /// </summary>
        private async void CanReadButton_Click(object sender, RoutedEventArgs e)
        {
            if (DataContext is not MainViewModel vm || !vm.IsConnected)
            {
                SystemMsgBox.Show("Please connect to device", "CAN Config",
                    SystemMsgButton.OK, SystemMsgImage.Warning);
                return;
            }

            try
            {
                CanReadButton.IsEnabled = false;
                CanWriteButton.IsEnabled = false;

                // パラメータモードに入る（最初のコマンドでウェルカムメッセージが返る）
                await vm.SendCommandAndGetResponseAsync("", 500);
                await Task.Delay(100);

                // CAN設定を読み込む
                await LoadCanSettingsAsync();
            }
            finally
            {
                CanReadButton.IsEnabled = true;
                CanWriteButton.IsEnabled = true;
            }
        }

        /// <summary>
        /// CAN応答をパースしてUIに反映
        /// </summary>
        private void ParseCanResponse(string response)
        {
            // Warning設定を解析
            var warnSoundRegex = new Regex(@"Warning:\s*(ON|OFF),\s*Sound:\s*(ON|OFF)", RegexOptions.IgnoreCase);
            var warnSoundMatch = warnSoundRegex.Match(response);
            if (warnSoundMatch.Success)
            {
                WarningEnabledBox.IsChecked = warnSoundMatch.Groups[1].Value.Equals("ON", StringComparison.OrdinalIgnoreCase);
                SoundEnabledBox.IsChecked = warnSoundMatch.Groups[2].Value.Equals("ON", StringComparison.OrdinalIgnoreCase);
            }

            // チャンネル設定を解析
            var channelBoxes = new (WpfTextBox idBox, CheckBox enableBox)[] {
                (CanCh1IdBox, CanCh1EnabledBox),
                (CanCh2IdBox, CanCh2EnabledBox),
                (CanCh3IdBox, CanCh3EnabledBox),
                (CanCh4IdBox, CanCh4EnabledBox),
                (CanCh5IdBox, CanCh5EnabledBox),
                (CanCh6IdBox, CanCh6EnabledBox)
            };

            // ファームウェア形式: "CH1: ID=0x3E8, ON"
            var chRegex = new Regex(@"CH(\d):\s*ID=0x([0-9A-Fa-f]+),\s*(ON|OFF)", RegexOptions.IgnoreCase);
            foreach (Match match in chRegex.Matches(response))
            {
                var chNum = int.Parse(match.Groups[1].Value);
                var canId = match.Groups[2].Value;
                var enabled = match.Groups[3].Value.Equals("ON", StringComparison.OrdinalIgnoreCase);

                if (chNum >= 1 && chNum <= 6)
                {
                    channelBoxes[chNum - 1].idBox.Text = $"0x{canId}";
                    channelBoxes[chNum - 1].enableBox.IsChecked = enabled;
                }
            }

            // フィールド定義を解析
            // ファームウェア形式: " 0  1   0   2   U    B   REV      0  1000  1000  0  Engine  rpm  N    ---  N    ---"
            // No CH Byte Len Type End Var    Off  Mul   Div  Dsh Name    Unit WLo  Lo WHi    Hi
            var fieldRegex = new Regex(@"^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(U|S)\s+(B|L)\s+(\S+)\s+([-\d]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S+)\s+(Y|N)\s+(\S+)\s+(Y|N)\s+(\S+)", RegexOptions.Multiline | RegexOptions.IgnoreCase);
            foreach (Match match in fieldRegex.Matches(response))
            {
                var idx = int.Parse(match.Groups[1].Value);
                if (idx >= 0 && idx < _canFields.Count)
                {
                    var field = _canFields[idx];
                    field.Channel = int.Parse(match.Groups[2].Value);
                    field.StartByte = int.Parse(match.Groups[3].Value);
                    field.ByteCount = int.Parse(match.Groups[4].Value);
                    field.DataType = match.Groups[5].Value.ToUpper();
                    field.Endian = match.Groups[6].Value.ToUpper();
                    field.TargetVarName = match.Groups[7].Value;
                    field.Offset = int.Parse(match.Groups[8].Value);
                    field.Multiplier = double.Parse(match.Groups[9].Value);
                    field.Divisor = double.Parse(match.Groups[10].Value);
                    field.Decimals = int.Parse(match.Groups[11].Value);
                    // Name: "-" は空文字に変換
                    var name = match.Groups[12].Value;
                    field.Name = (name == "-") ? "" : name;
                    // Unit: "-" は空文字に変換
                    var unit = match.Groups[13].Value;
                    field.Unit = (unit == "-") ? "" : unit;
                    // Warning Low
                    field.WarnLowEnabled = match.Groups[14].Value.Equals("Y", StringComparison.OrdinalIgnoreCase);
                    field.WarnLow = match.Groups[15].Value;
                    // Warning High
                    field.WarnHighEnabled = match.Groups[16].Value.Equals("Y", StringComparison.OrdinalIgnoreCase);
                    field.WarnHigh = match.Groups[17].Value;
                    field.Enabled = field.Channel > 0;
                }
            }

            // INotifyPropertyChangedで自動更新されるため、Items.Refresh()は不要
            // Items.Refresh()はTemplateColumnのバインディングと競合する可能性がある
        }

        /// <summary>
        /// CAN設定書き込みボタン
        /// </summary>
        private async void CanWriteButton_Click(object sender, RoutedEventArgs e)
        {
            if (DataContext is not MainViewModel vm || !vm.IsConnected)
            {
                SystemMsgBox.Show("Please connect to device", "CAN Config",
                    SystemMsgButton.OK, SystemMsgImage.Warning);
                return;
            }

            try
            {
                CanStatusText.Text = "Saving...";
                CanReadButton.IsEnabled = false;
                CanWriteButton.IsEnabled = false;

                var commands = new List<string>();

                // マスターワーニング設定
                commands.Add($"can_warning {(WarningEnabledBox.IsChecked == true ? 1 : 0)}");
                commands.Add($"can_sound {(SoundEnabledBox.IsChecked == true ? 1 : 0)}");

                // チャンネル設定
                var channelBoxes = new[] {
                    (CanCh1IdBox, CanCh1EnabledBox),
                    (CanCh2IdBox, CanCh2EnabledBox),
                    (CanCh3IdBox, CanCh3EnabledBox),
                    (CanCh4IdBox, CanCh4EnabledBox),
                    (CanCh5IdBox, CanCh5EnabledBox),
                    (CanCh6IdBox, CanCh6EnabledBox)
                };

                for (int i = 0; i < channelBoxes.Length; i++)
                {
                    var (idBox, enabledBox) = channelBoxes[i];
                    if (!string.IsNullOrEmpty(idBox.Text))
                    {
                        var canId = ParseCanId(idBox.Text);
                        var enabled = enabledBox.IsChecked == true ? 1 : 0;
                        commands.Add($"can_ch {i + 1} {canId} {enabled}");
                    }
                }

                // フィールド設定
                foreach (var field in _canFields)
                {
                    var dataType = field.DataType == "S" ? 1 : 0;
                    var endian = field.Endian == "L" ? 1 : 0;
                    var channel = field.Enabled ? field.Channel : 0;
                    var targetVar = GetTargetVarIndex(field.TargetVarName);
                    var name = string.IsNullOrEmpty(field.Name) ? "-" : field.Name;
                    var unit = string.IsNullOrEmpty(field.Unit) ? "-" : field.Unit;
                    var warnLoEnabled = field.WarnLowEnabled ? 1 : 0;
                    var warnLo = string.IsNullOrEmpty(field.WarnLow) || field.WarnLow == "---" ? "0" : field.WarnLow;
                    var warnHiEnabled = field.WarnHighEnabled ? 1 : 0;
                    var warnHi = string.IsNullOrEmpty(field.WarnHigh) || field.WarnHigh == "---" ? "0" : field.WarnHigh;

                    commands.Add($"can_field {field.Index} {channel} {field.StartByte} {field.ByteCount} {dataType} {endian} {targetVar} {field.Offset} {field.Multiplier} {field.Divisor} {name} {unit} {field.Decimals} {warnLoEnabled} {warnLo} {warnHiEnabled} {warnHi}");
                }

                // コマンドを順次送信
                foreach (var cmd in commands)
                {
                    await vm.SendCommandAndGetResponseAsync(cmd);
                    await System.Threading.Tasks.Task.Delay(150);
                }

                // 設定を保存
                await vm.SendCommandAndGetResponseAsync("can_save");

                // パラメータモードを終了
                await vm.SendCommandAndGetResponseAsync("exit", 200);

                CanStatusText.Text = "Save complete (Saved to EEPROM)";
            }
            catch (FormatException)
            {
                CanStatusText.Text = "Error: CAN ID must be hexadecimal (e.g. 0x7E0)";
            }
            catch (Exception ex)
            {
                CanStatusText.Text = $"エラー: {ex.Message}";
            }
            finally
            {
                CanReadButton.IsEnabled = true;
                CanWriteButton.IsEnabled = true;
            }
        }

        /// <summary>
        /// CAN IDをパース
        /// </summary>
        private int ParseCanId(string text)
        {
            text = text.Trim();
            if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                return Convert.ToInt32(text.Substring(2), 16);
            }
            return Convert.ToInt32(text, 16);
        }

        /// <summary>
        /// ターゲット変数名からインデックスを取得
        /// </summary>
        private int GetTargetVarIndex(string varName)
        {
            return varName?.ToUpperInvariant() switch
            {
                "REV" => 0,      // CAN_TARGET_REV
                "AF" => 1,       // CAN_TARGET_AF
                "NUM1" => 2,     // CAN_TARGET_NUM1 (水温)
                "NUM2" => 3,     // CAN_TARGET_NUM2 (吸気温)
                "NUM3" => 4,     // CAN_TARGET_NUM3 (油温)
                "NUM4" => 5,     // CAN_TARGET_NUM4 (MAP)
                "NUM5" => 6,     // CAN_TARGET_NUM5 (油圧)
                "NUM6" => 7,     // CAN_TARGET_NUM6 (バッテリー)
                "SPEED" => 8,    // CAN_TARGET_SPEED
                _ => 255         // CAN_TARGET_NONE
            };
        }

        /// <summary>
        /// CAN設定デフォルトボタン
        /// </summary>
        private async void CanDefaultButton_Click(object sender, RoutedEventArgs e)
        {
            if (DataContext is not MainViewModel vm || !vm.IsConnected)
            {
                SystemMsgBox.Show("Please connect to device", "CAN Config",
                    SystemMsgButton.OK, SystemMsgImage.Warning);
                return;
            }

            var result = SystemMsgBox.Show(
                "Reset CAN settings to MoTeC defaults?\n(Device settings will be updated)",
                "CAN Config",
                SystemMsgButton.YesNo,
                SystemMsgImage.Question);

            if (result != SystemMsgResult.Yes)
                return;

            try
            {
                CanReadButton.IsEnabled = false;
                CanWriteButton.IsEnabled = false;
                CanDefaultButton.IsEnabled = false;
                CanStatusText.Text = "Applying default settings...";

                // パラメータモードに入る
                vm.ClearResponseBuffer();
                await vm.SendCommandAndGetResponseAsync("", 800);

                // can_preset motec コマンドを送信してデフォルト設定を適用
                vm.ClearResponseBuffer();
                await vm.SendCommandAndGetResponseAsync("can_preset motec", 1000);

                // 設定を保存
                await vm.SendCommandAndGetResponseAsync("can_save", 500);

                // 設定を再読み込み
                vm.ClearResponseBuffer();
                var response = await vm.SendCommandAndGetResponseAsync("can_list", 5000);

                // パラメータモードを終了
                await vm.SendCommandAndGetResponseAsync("exit", 500);

                if (!string.IsNullOrEmpty(response))
                {
                    ParseCanResponse(response);
                }

                CanStatusText.Text = "MoTeC default settings applied";
            }
            catch (Exception ex)
            {
                CanStatusText.Text = $"Error: {ex.Message}";
            }
            finally
            {
                CanReadButton.IsEnabled = true;
                CanWriteButton.IsEnabled = true;
                CanDefaultButton.IsEnabled = true;
            }
        }

        #endregion

        #region Firmware Catalog Events

        /// <summary>
        /// ファームウェアバリアントがクリックされたときの処理
        /// </summary>
        private void FirmwareVariant_Click(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (sender is FrameworkElement element && element.DataContext is FirmwareVariantViewModel variant)
            {
                if (DataContext is MainViewModel vm)
                {
                    vm.FirmwareCatalog.SelectedVariant = variant;
                }
            }
        }

        /// <summary>
        /// リリースComboBoxの選択変更イベント
        /// </summary>
        private async void ReleaseComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // ユーザーによる選択変更時のみ処理（RemovedItemsがある = 既に何か選択されていた状態からの変更）
            if (e.AddedItems.Count > 0 && e.RemovedItems.Count > 0 && e.AddedItems[0] is string selectedTag)
            {
                System.Diagnostics.Debug.WriteLine($"[MainWindow] User selected release: {selectedTag} (from {e.RemovedItems[0]})");
                if (DataContext is MainViewModel vm)
                {
                    // 直接マニフェストをロード（ViewModelのSelectedRelease変更による呼び出しをバイパス）
                    await vm.FirmwareCatalog.LoadReleaseAsync(selectedTag);
                }
            }
        }

        #endregion
    }

    /// <summary>
    /// CANフィールドのデータモデル
    /// </summary>
    public class CanFieldItem : INotifyPropertyChanged
    {
        private int _index;
        private bool _enabled;
        private int _channel;
        private int _startByte;
        private int _byteCount = 2;
        private string _dataType = "U";
        private string _endian = "B";
        private string _targetVarName = "USER1";
        private int _offset;
        private double _multiplier = 1.0;
        private double _divisor = 1.0;
        private int _decimals;
        private string _name = "";
        private string _unit = "";
        private bool _warnLowEnabled;
        private string _warnLow = "---";
        private bool _warnHighEnabled;
        private string _warnHigh = "---";

        public int Index { get => _index; set { _index = value; OnPropertyChanged(nameof(Index)); } }
        public bool Enabled { get => _enabled; set { _enabled = value; OnPropertyChanged(nameof(Enabled)); } }
        public int Channel { get => _channel; set { _channel = value; OnPropertyChanged(nameof(Channel)); } }
        public int StartByte { get => _startByte; set { _startByte = value; OnPropertyChanged(nameof(StartByte)); } }
        public int ByteCount { get => _byteCount; set { _byteCount = value; OnPropertyChanged(nameof(ByteCount)); } }
        public string DataType { get => _dataType; set { _dataType = value; OnPropertyChanged(nameof(DataType)); } }
        public string Endian { get => _endian; set { _endian = value; OnPropertyChanged(nameof(Endian)); } }
        public string TargetVarName { get => _targetVarName; set { _targetVarName = value; OnPropertyChanged(nameof(TargetVarName)); } }
        public int Offset { get => _offset; set { _offset = value; OnPropertyChanged(nameof(Offset)); } }
        public double Multiplier { get => _multiplier; set { _multiplier = value; OnPropertyChanged(nameof(Multiplier)); } }
        public double Divisor { get => _divisor; set { _divisor = value; OnPropertyChanged(nameof(Divisor)); } }
        public int Decimals { get => _decimals; set { _decimals = value; OnPropertyChanged(nameof(Decimals)); } }
        public string Name { get => _name; set { _name = value; OnPropertyChanged(nameof(Name)); } }
        public string Unit { get => _unit; set { _unit = value; OnPropertyChanged(nameof(Unit)); } }
        public bool WarnLowEnabled { get => _warnLowEnabled; set { _warnLowEnabled = value; OnPropertyChanged(nameof(WarnLowEnabled)); } }
        public string WarnLow { get => _warnLow; set { _warnLow = value; OnPropertyChanged(nameof(WarnLow)); } }
        public bool WarnHighEnabled { get => _warnHighEnabled; set { _warnHighEnabled = value; OnPropertyChanged(nameof(WarnHighEnabled)); } }
        public string WarnHigh { get => _warnHigh; set { _warnHigh = value; OnPropertyChanged(nameof(WarnHigh)); } }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
