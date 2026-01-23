using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace FullmoniTerminal;

/// <summary>
/// ComboBox選択肢用のDataContext
/// </summary>
public class CanFieldsContext
{
    public ObservableCollection<CanFieldItem> Fields { get; set; } = new();
    public List<string> DataTypeOptions { get; } = new() { "U", "S" };
    public List<string> EndianOptions { get; } = new() { "B", "L" };
    public List<string> VarOptions { get; } = new() { "REV", "AF", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "SPEED" };
}

/// <summary>
/// CANフィールド定義のデータモデル
/// Issue #50: 警告設定（Name, Unit, WarnEnabled, WarnLow, WarnHigh）を追加
/// </summary>
public class CanFieldItem : INotifyPropertyChanged
{
    private int _index;
    private int _channel = 1;
    private int _startByte = 0;
    private int _byteCount = 2;
    private string _dataType = "U";
    private string _endian = "B";
    private int _targetVar = 0;
    private int _offset = 0;
    private int _multiplier = 1000;
    private int _divisor = 1000;
    private bool _enabled = false;
    // Issue #50: 警告設定用フィールド
    private string _name = "";
    private string _unit = "";
    private bool _warnEnabled = false;
    private int _warnLow = -32768;   // CAN_WARN_DISABLED
    private int _warnHigh = -32768;  // CAN_WARN_DISABLED

    // 変数名リスト（TargetVarNameプロパティで使用）
    private static readonly string[] VarNames = { "REV", "AF", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "SPEED" };

    public int Index { get => _index; set { _index = value; OnPropertyChanged(nameof(Index)); } }
    public int Channel { get => _channel; set { _channel = value; OnPropertyChanged(nameof(Channel)); } }
    public int StartByte { get => _startByte; set { _startByte = value; OnPropertyChanged(nameof(StartByte)); } }
    public int ByteCount { get => _byteCount; set { _byteCount = value; OnPropertyChanged(nameof(ByteCount)); } }
    public string DataType { get => _dataType; set { _dataType = value; OnPropertyChanged(nameof(DataType)); } }
    public string Endian { get => _endian; set { _endian = value; OnPropertyChanged(nameof(Endian)); } }
    public int TargetVar { get => _targetVar; set { _targetVar = value; OnPropertyChanged(nameof(TargetVar)); OnPropertyChanged(nameof(TargetVarName)); } }
    
    // ComboBox用: 変数名で表示・選択
    public string TargetVarName
    {
        get => (_targetVar >= 0 && _targetVar < VarNames.Length) ? VarNames[_targetVar] : "REV";
        set
        {
            for (int i = 0; i < VarNames.Length; i++)
            {
                if (VarNames[i] == value)
                {
                    TargetVar = i;
                    return;
                }
            }
            TargetVar = 0; // デフォルト
        }
    }
    
    public int Offset { get => _offset; set { _offset = value; OnPropertyChanged(nameof(Offset)); } }
    public int Multiplier { get => _multiplier; set { _multiplier = value; OnPropertyChanged(nameof(Multiplier)); } }
    public int Divisor { get => _divisor; set { _divisor = value; OnPropertyChanged(nameof(Divisor)); } }
    public bool Enabled { get => _enabled; set { _enabled = value; OnPropertyChanged(nameof(Enabled)); } }
    // Issue #50: 警告設定用プロパティ
    public string Name { get => _name; set { _name = value; OnPropertyChanged(nameof(Name)); } }
    public string Unit { get => _unit; set { _unit = value; OnPropertyChanged(nameof(Unit)); } }
    public bool WarnEnabled { get => _warnEnabled; set { _warnEnabled = value; OnPropertyChanged(nameof(WarnEnabled)); } }
    public int WarnLow { get => _warnLow; set { _warnLow = value; OnPropertyChanged(nameof(WarnLow)); } }
    public int WarnHigh { get => _warnHigh; set { _warnHigh = value; OnPropertyChanged(nameof(WarnHigh)); } }

    public event PropertyChangedEventHandler? PropertyChanged;
    protected void OnPropertyChanged(string propertyName)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    private ObservableCollection<CanFieldItem> _canFields = new();
    private CanFieldsContext _canFieldsContext = new();

    public MainWindow()
    {
        InitializeComponent();
        InitializeCanFields();
        
        // ViewModelのParametersLoadedイベントを購読して、CAN設定も読み込む
        Loaded += MainWindow_Loaded;
    }

    private void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
        if (DataContext is ViewModels.MainViewModel vm)
        {
            vm.ParametersLoaded += ViewModel_ParametersLoaded;
        }
    }
    
    private async void ViewModel_ParametersLoaded(object? sender, EventArgs e)
    {
        // パラメータ読込完了後にCAN設定も読み込む
        await LoadCanConfigurationAsync();
    }
    
    /// <summary>
    /// CAN設定をデバイスから読み込む (Issue #50: 接続時自動読込対応)
    /// </summary>
    private async Task LoadCanConfigurationAsync()
    {
        if (DataContext is not ViewModels.MainViewModel vm || !vm.IsConnected)
            return;

        UpdateCanStatus("CAN設定を読み込み中...");

        try
        {
            // パラメータモードに入る
            vm.ClearResponseBuffer();
            vm.SendCommandDirect("");
            await Task.Delay(300);

            // バッファをクリアして安定するまで待つ
            vm.ClearResponseBuffer();
            await Task.Delay(100);

            // can_listを送信
            var response = await vm.SendCommandAndGetResponseAsync("can_list", 5000);

            // 応答を解析してUIに反映
            int chCount = ParseCanConfigResponse(response);

            // パラメータモードを抜ける
            vm.ClearResponseBuffer();
            vm.SendCommandDirect("exit");
            await Task.Delay(200);

            if (string.IsNullOrWhiteSpace(response))
            {
                UpdateCanStatus("CAN設定: 応答なし");
                return;
            }

            UpdateCanStatus($"CAN設定: {chCount}件読込完了");
        }
        catch (Exception ex)
        {
            UpdateCanStatus($"CAN設定読込エラー: {ex.Message}");
        }
    }

    // 変数名 → 数値インデックス変換テーブル
    private static readonly Dictionary<string, int> VarNameToIndex = new(StringComparer.OrdinalIgnoreCase)
    {
        { "REV", 0 },
        { "AF", 1 },
        { "NUM1", 2 },
        { "NUM2", 3 },
        { "NUM3", 4 },
        { "NUM4", 5 },
        { "NUM5", 6 },
        { "NUM6", 7 },
        { "SPEED", 8 },
        { "---", 255 }
    };

    // 数値インデックス → 変数名変換テーブル
    private static readonly string[] VarIndexToName = { "REV", "AF", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "SPEED" };

    private void InitializeCanFields()
    {
        _canFields = new ObservableCollection<CanFieldItem>();
        _canFieldsContext = new CanFieldsContext { Fields = _canFields };

        // Issue #50: MoTeC M100デフォルト設定 (Firmware側CAN_PRESET_MOTECと一致)
        // Var: 0=REV, 1=AF, 2=NUM1(水温), 3=NUM2(吸気温), 4=NUM3(油温), 5=NUM4(MAP), 6=NUM5(油圧), 7=NUM6(電圧), 8=SPEED
        // WarnLow/WarnHigh: -32768 = 無効 (CAN_WARN_DISABLED)
        var defaults = new[]
        {
            // CH1 (0x3E8): RPM(0-1), MAP(4-5), IAT(6-7)
            new { Ch = 1, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = true,
                  Name = "REV", Unit = "rpm", WarnEn = true, WarnLo = 200, WarnHi = 9000 },
            new { Ch = 1, Byte = 4, Size = 2, Type = "U", End = "B", Var = 5, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "MAP", Unit = "kPa", WarnEn = true, WarnLo = 0, WarnHi = 150 },
            new { Ch = 1, Byte = 6, Size = 2, Type = "S", End = "B", Var = 3, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "IAT", Unit = "deg", WarnEn = true, WarnLo = -40, WarnHi = 80 },
            // CH2 (0x3E9): ECT(0-1), AFR(2-3)
            new { Ch = 2, Byte = 0, Size = 2, Type = "S", End = "B", Var = 2, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "WATER", Unit = "deg", WarnEn = true, WarnLo = 60, WarnHi = 110 },
            new { Ch = 2, Byte = 2, Size = 2, Type = "U", End = "B", Var = 1, Off = 0, Mul = 147, Div = 1000, En = true,
                  Name = "A/F", Unit = "afr", WarnEn = true, WarnLo = 100, WarnHi = 180 },
            // CH3 (0x3EA): OilTemp(6-7)
            new { Ch = 3, Byte = 6, Size = 2, Type = "S", End = "B", Var = 4, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "OIL-T", Unit = "deg", WarnEn = true, WarnLo = 60, WarnHi = 130 },
            // CH4 (0x3EB): OilPressure(0-1), BattV(6-7)
            new { Ch = 4, Byte = 0, Size = 2, Type = "U", End = "B", Var = 6, Off = 0, Mul = 1, Div = 1000, En = true,
                  Name = "OIL_P", Unit = "kPa", WarnEn = true, WarnLo = 1500, WarnHi = 9000 },
            new { Ch = 4, Byte = 6, Size = 2, Type = "U", End = "B", Var = 7, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "BATT", Unit = "V", WarnEn = false, WarnLo = 9, WarnHi = 160 },
        };

        for (int i = 0; i < defaults.Length; i++)
        {
            var d = defaults[i];
            _canFields.Add(new CanFieldItem
            {
                Index = i,
                Channel = d.Ch,
                StartByte = d.Byte,
                ByteCount = d.Size,
                DataType = d.Type,
                Endian = d.End,
                TargetVar = d.Var,
                Offset = d.Off,
                Multiplier = d.Mul,
                Divisor = d.Div,
                Enabled = d.En,
                Name = d.Name,
                Unit = d.Unit,
                WarnEnabled = d.WarnEn,
                WarnLow = d.WarnLo,
                WarnHigh = d.WarnHi
            });
        }

        CanFieldsGrid.DataContext = _canFieldsContext;
        CanFieldsGrid.ItemsSource = _canFields;
        
        // セル編集終了時のバリデーションハンドラを登録
        CanFieldsGrid.CellEditEnding += CanFieldsGrid_CellEditEnding;
    }

    /// <summary>
    /// DataGridのセル編集終了時に数値バリデーションを行う
    /// </summary>
    private void CanFieldsGrid_CellEditEnding(object? sender, DataGridCellEditEndingEventArgs e)
    {
        // キャンセル時は何もしない
        if (e.EditAction == DataGridEditAction.Cancel)
            return;

        // 編集要素がTextBoxの場合のみ検証
        if (e.EditingElement is not TextBox textBox)
            return;

        // 列名を取得
        var columnHeader = e.Column.Header?.ToString();
        if (string.IsNullOrEmpty(columnHeader))
            return;

        // 数値列のみバリデーション
        var numericColumns = new[] { "CH", "Byte", "Size", "Off", "Mul", "Div" };
        if (!numericColumns.Contains(columnHeader))
            return;

        var inputText = textBox.Text.Trim();

        // 空文字列チェック
        if (string.IsNullOrEmpty(inputText))
        {
            MessageBox.Show(
                this,
                $"{columnHeader}フィールドには数値を入力してください。",
                "入力エラー",
                MessageBoxButton.OK,
                MessageBoxImage.Warning);
            e.Cancel = true;
            return;
        }

        // 整数パースチェック
        if (!int.TryParse(inputText, out int value))
        {
            MessageBox.Show(
                this,
                $"{columnHeader}フィールドには整数を入力してください。\n入力値: {inputText}",
                "入力エラー",
                MessageBoxButton.OK,
                MessageBoxImage.Warning);
            e.Cancel = true;
            return;
        }

        // 列ごとの範囲チェック
        string? rangeError = columnHeader switch
        {
            "CH" when value < 1 || value > 6 => "CH は 1～6 の範囲で入力してください。",
            "Byte" when value < 0 || value > 7 => "Byte は 0～7 の範囲で入力してください。",
            "Size" when value != 1 && value != 2 && value != 4 => "Size は 1, 2, 4 のいずれかを入力してください。",
            "Mul" when value <= 0 => "Mul は 1 以上の値を入力してください。",
            "Div" when value <= 0 => "Div は 1 以上の値を入力してください。",
            _ => null
        };

        if (rangeError != null)
        {
            MessageBox.Show(
                this,
                rangeError,
                "入力エラー",
                MessageBoxButton.OK,
                MessageBoxImage.Warning);
            e.Cancel = true;
        }
    }

    private void Window_Closing(object sender, CancelEventArgs e)
    {
        if (DataContext is ViewModels.MainViewModel vm)
        {
            // ファームウェア更新中は閉じられないようにする
            if (vm.IsFirmwareUpdating)
            {
                e.Cancel = true;
                MessageBox.Show(
                    this,
                    "ファームウェア更新中はアプリケーションを終了できません。\n更新が完了するまでお待ちください。",
                    "警告",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);
                return;
            }
            vm.Dispose();
        }
    }

    private void FirmwareLogTextBox_TextChanged(object sender, TextChangedEventArgs e)
    {
        if (sender is TextBox textBox)
        {
            textBox.ScrollToEnd();
        }
    }

    private void OpenGitHubLink_Click(object sender, RoutedEventArgs e)
    {
        OpenUrl("https://github.com/tomoya723/FULLMONI-WIDE");
    }

    private void OpenDocsLink_Click(object sender, RoutedEventArgs e)
    {
        OpenUrl("https://github.com/tomoya723/FULLMONI-WIDE/blob/main/Firmware/docs/PARAM_CONSOLE.md");
    }

    private void OpenIssuesLink_Click(object sender, RoutedEventArgs e)
    {
        OpenUrl("https://github.com/tomoya723/FULLMONI-WIDE/issues");
    }

    private void AboutScrollViewer_Loaded(object sender, RoutedEventArgs e)
    {
        if (sender is ScrollViewer scrollViewer)
        {
            scrollViewer.ScrollToTop();
        }
    }

    private static void OpenUrl(string url)
    {
        try
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = url,
                UseShellExecute = true
            });
        }
        catch
        {
            // ブラウザで開けなかった場合は無視
        }
    }

    #region CAN設定

    private void UpdateCanStatus(string message)
    {
        CanStatusText.Text = $"[{DateTime.Now:HH:mm:ss}] {message}";
    }

    /// <summary>
    /// パラメータモードに入ってからコマンドを送信
    /// </summary>
    private async Task SendCanCommandAsync(ViewModels.MainViewModel vm, string command)
    {
        // パラメータモードに入る（空コマンドでウェイク）
        vm.SendCommandDirect("");
        await Task.Delay(300);

        // コマンド送信
        vm.SendCommandDirect(command);
    }

    private async void CanReadButton_Click(object sender, RoutedEventArgs e)
    {
        if (DataContext is ViewModels.MainViewModel vm && vm.IsConnected)
        {
            vm.IsBusy = true;
            UpdateCanStatus("CAN設定を読み込み中...");

            try
            {
                // パラメータモードに入る
                vm.ClearResponseBuffer();
                vm.SendCommandDirect("");
                await Task.Delay(300);

                // バッファをクリアして安定するまで待つ
                vm.ClearResponseBuffer();
                await Task.Delay(100);

                // can_listを送信（プロンプトが来るまで待機）
                var response = await vm.SendCommandAndGetResponseAsync("can_list", 5000);

                // デバッグ: 応答をファイルに保存
                try
                {
                    System.IO.File.WriteAllText("can_list_response.txt", response ?? "(null)");
                }
                catch { }

                // 応答を解析してUIに反映
                int chCount = ParseCanConfigResponse(response);

                // パラメータモードを抜ける
                vm.ClearResponseBuffer();
                vm.SendCommandDirect("exit");
                await Task.Delay(200);

                if (string.IsNullOrWhiteSpace(response))
                {
                    UpdateCanStatus("エラー: 応答がありません");
                    return;
                }

                // フィールドが読み込めなかった場合は警告
                if (chCount <= 6)
                {
                    UpdateCanStatus($"警告: フィールドが読み込めませんでした（{chCount}件はチャンネルのみ）");
                }
                else
                {
                    UpdateCanStatus($"読み込み完了: {chCount}件のデータを反映");
                }
            }
            finally
            {
                vm.IsBusy = false;
            }
        }
        else
        {
            UpdateCanStatus("エラー: デバイスに接続してください");
        }
    }

    /// <summary>
    /// can_listの応答を解析してUIに反映
    /// Issue #50: Name, Unit, WarnEnabled, WarnLow, WarnHigh対応
    /// </summary>
    private int ParseCanConfigResponse(string response)
    {
        int matchCount = 0;

        // チャンネル設定を解析: CH1: ID=0x3E8, ON
        var chRegex = new Regex(@"CH(\d+):\s*ID=0x([0-9A-Fa-f]+),\s*(ON|OFF)", RegexOptions.IgnoreCase);
        var chMatches = chRegex.Matches(response);

        var channelBoxes = new (TextBox idBox, CheckBox enableBox)[] {
            (CanCh1IdBox, CanCh1EnabledBox),
            (CanCh2IdBox, CanCh2EnabledBox),
            (CanCh3IdBox, CanCh3EnabledBox),
            (CanCh4IdBox, CanCh4EnabledBox),
            (CanCh5IdBox, CanCh5EnabledBox),
            (CanCh6IdBox, CanCh6EnabledBox)
        };

        foreach (Match match in chMatches)
        {
            int chNum = int.Parse(match.Groups[1].Value);
            string canId = match.Groups[2].Value;
            bool enabled = match.Groups[3].Value.Equals("ON", StringComparison.OrdinalIgnoreCase);

            if (chNum >= 1 && chNum <= 6)
            {
                channelBoxes[chNum - 1].idBox.Text = $"0x{canId}";
                channelBoxes[chNum - 1].enableBox.IsChecked = enabled;
                matchCount++;
            }
        }

        // フィールド設定を解析 (Issue #50: 警告設定対応)
        // ファームウェア出力: "%2d %2d   %d   %d   %c    %c   %-5s %4d %5d %5d %-7s %-4s %c  %6s %6s"
        // 例: " 3  2   0   2   S    B   NUM1     0  1000 10000 WATER   C    Y      60    100"
        // グループ: Index, CH, Byte, Len, Type, End, Var, Off, Mul, Div, Name, Unit, WE, WarnLo, WarnHi
        // 空のName/Unitはファームウェアが "-" を出力する
        var fieldRegex = new Regex(@"^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+([US])\s+([BL])\s+(\S+)\s+(-?\d+)\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S+)\s+([YN])\s+(-?\d+|---)\s+(-?\d+|---)", RegexOptions.IgnoreCase | RegexOptions.Multiline);
        var fieldMatches = fieldRegex.Matches(response);

        // フィールドがマッチした場合のみ更新（読込み失敗時は既存設定を保持）
        if (fieldMatches.Count > 0)
        {
            // 一旦全フィールドを無効化
            foreach (var field in _canFields)
            {
                field.Enabled = false;
            }
        }

        foreach (Match match in fieldMatches)
        {
            int index = int.Parse(match.Groups[1].Value);
            if (index >= 0 && index < _canFields.Count)
            {
                var field = _canFields[index];
                field.Channel = int.Parse(match.Groups[2].Value);
                field.StartByte = int.Parse(match.Groups[3].Value);
                field.ByteCount = int.Parse(match.Groups[4].Value);
                field.DataType = match.Groups[5].Value.ToUpper();
                field.Endian = match.Groups[6].Value.ToUpper();
                // Groups[7] = Var名 → 数値に変換
                string varName = match.Groups[7].Value;
                field.TargetVar = VarNameToIndex.TryGetValue(varName, out int varIdx) ? varIdx : 255;
                // Groups[8] = Offset
                field.Offset = int.Parse(match.Groups[8].Value);
                field.Multiplier = int.Parse(match.Groups[9].Value);
                field.Divisor = int.Parse(match.Groups[10].Value);
                
                // Issue #50: 警告設定 ("-" は空として扱う)
                string nameVal = match.Groups[11].Value.Trim();
                string unitVal = match.Groups[12].Value.Trim();
                field.Name = (nameVal == "-") ? "" : nameVal;
                field.Unit = (unitVal == "-") ? "" : unitVal;
                field.WarnEnabled = match.Groups[13].Value.Equals("Y", StringComparison.OrdinalIgnoreCase);
                // WarnLo/WarnHi: "---" は -32768 (CAN_WARN_DISABLED)
                string warnLoStr = match.Groups[14].Value;
                string warnHiStr = match.Groups[15].Value;
                field.WarnLow = warnLoStr == "---" ? -32768 : int.Parse(warnLoStr);
                field.WarnHigh = warnHiStr == "---" ? -32768 : int.Parse(warnHiStr);
                
                // channel > 0 なら有効
                field.Enabled = field.Channel > 0;
                matchCount++;
            }
        }

        return matchCount;
    }

    /// <summary>
    /// CAN設定のバリデーション
    /// </summary>
    private bool ValidateCanSettings(out string errorMessage)
    {
        errorMessage = "";
        var errors = new List<string>();

        // チャンネル設定のバリデーション
        var channelBoxes = new[] {
            (CanCh1IdBox, "CH1"),
            (CanCh2IdBox, "CH2"),
            (CanCh3IdBox, "CH3"),
            (CanCh4IdBox, "CH4"),
            (CanCh5IdBox, "CH5"),
            (CanCh6IdBox, "CH6")
        };

        foreach (var (idBox, name) in channelBoxes)
        {
            if (!string.IsNullOrEmpty(idBox.Text))
            {
                var canId = ParseCanId(idBox.Text);
                if (canId < 0 || canId > 0x7FF)
                {
                    errors.Add($"{name}: CAN IDは0x000〜0x7FFの範囲で指定してください");
                }
            }
        }

        // フィールド設定のバリデーション
        foreach (var field in _canFields)
        {
            if (!field.Enabled) continue; // 無効なフィールドはスキップ

            var idx = field.Index;

            // チャンネル範囲チェック
            if (field.Channel < 1 || field.Channel > 6)
            {
                errors.Add($"フィールド{idx}: CHは1〜6の範囲で指定してください");
            }

            // Byte位置チェック
            if (field.StartByte < 0 || field.StartByte > 7)
            {
                errors.Add($"フィールド{idx}: Byteは0〜7の範囲で指定してください");
            }

            // Sizeチェック
            if (field.ByteCount != 1 && field.ByteCount != 2 && field.ByteCount != 4)
            {
                errors.Add($"フィールド{idx}: Sizeは1, 2, 4のいずれかを指定してください");
            }

            // Byte + Size がデータ範囲を超えていないかチェック
            if (field.StartByte + field.ByteCount > 8)
            {
                errors.Add($"フィールド{idx}: Byte({field.StartByte}) + Size({field.ByteCount}) が8を超えています");
            }

            // Mul/Divチェック
            if (field.Multiplier <= 0)
            {
                errors.Add($"フィールド{idx}: Mulは1以上を指定してください");
            }
            if (field.Divisor <= 0)
            {
                errors.Add($"フィールド{idx}: Divは1以上を指定してください");
            }
        }

        if (errors.Count > 0)
        {
            errorMessage = string.Join("\n", errors);
            return false;
        }

        return true;
    }

    private async void CanWriteButton_Click(object sender, RoutedEventArgs e)
    {
        if (DataContext is not ViewModels.MainViewModel vm || !vm.IsConnected)
        {
            UpdateCanStatus("エラー: デバイスに接続してください");
            return;
        }

        // バリデーション
        if (!ValidateCanSettings(out var errorMessage))
        {
            MessageBox.Show($"入力値に問題があります:\n\n{errorMessage}", "入力エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        vm.IsBusy = true;

        try
        {
            var commands = new List<string>();

            // CANチャンネル設定
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
                    // can_ch <n> <id> <en> (n=1-6)
                    commands.Add($"can_ch {i + 1} {canId} {enabled}");
                }
            }

            // CANフィールド設定（ENオフの場合はchannel=0で無効化）
            foreach (var field in _canFields)
            {
                var dataType = field.DataType == "S" ? 1 : 0;
                var endian = field.Endian == "L" ? 1 : 0;
                // ENオフの場合はchannel=0で無効化
                var channel = field.Enabled ? field.Channel : 0;
                var warnEn = field.WarnEnabled ? 1 : 0;
                // 名前と単位（空の場合は "-" を送信）
                var name = string.IsNullOrWhiteSpace(field.Name) ? "-" : field.Name.Replace(" ", "_");
                var unit = string.IsNullOrWhiteSpace(field.Unit) ? "-" : field.Unit.Replace(" ", "_");
                // can_field <n> <ch> <byte> <len> <type> <end> <var> <off> <mul> <div> <name> <unit> <warn_en> <warn_lo> <warn_hi>
                commands.Add($"can_field {field.Index} {channel} {field.StartByte} {field.ByteCount} {dataType} {endian} {field.TargetVar} {field.Offset} {field.Multiplier} {field.Divisor} {name} {unit} {warnEn} {field.WarnLow} {field.WarnHigh}");
            }

            if (commands.Count == 0)
            {
                UpdateCanStatus("設定するCAN項目がありません（ENを有効にしてください）");
                return;
            }

            // パラメータモードに入ってからコマンドを順番に送信
            UpdateCanStatus($"送信中: {commands.Count}個のコマンド...");

            // パラメータモードに入る
            vm.SendCommandDirect("");
            await Task.Delay(300);

            foreach (var cmd in commands)
            {
                vm.SendCommandDirect(cmd);
                await Task.Delay(150);
            }

            // EEPROMに保存（E2PROM書き込みは時間がかかるため長めに待機）
            vm.SendCommandDirect("can_save");
            await Task.Delay(1000);

            // パラメータモードを抜ける
            vm.SendCommandDirect("exit");
            await Task.Delay(300);

            UpdateCanStatus($"完了: {commands.Count}個のCAN設定を送信・保存");
        }
        finally
        {
            vm.IsBusy = false;
        }
    }

    private void CanDefaultButton_Click(object sender, RoutedEventArgs e)
    {
        var result = MessageBox.Show("CAN設定を出荷時設定に戻しますか？", "確認", MessageBoxButton.YesNo, MessageBoxImage.Question);
        if (result == MessageBoxResult.Yes)
        {
            // チャンネル設定をデフォルトに
            CanCh1IdBox.Text = "0x3E8"; CanCh1EnabledBox.IsChecked = true;
            CanCh2IdBox.Text = "0x3E9"; CanCh2EnabledBox.IsChecked = true;
            CanCh3IdBox.Text = "0x3EA"; CanCh3EnabledBox.IsChecked = true;
            CanCh4IdBox.Text = "0x3EB"; CanCh4EnabledBox.IsChecked = true;
            CanCh5IdBox.Text = "0x3EC"; CanCh5EnabledBox.IsChecked = true;
            CanCh6IdBox.Text = "0x3ED"; CanCh6EnabledBox.IsChecked = false;

            // フィールド設定を再初期化 (Issue #50: Firmware側CAN_PRESET_MOTECと完全一致)
            _canFields.Clear();
            // Var: 0=REV, 1=AF, 2=NUM1(水温), 3=NUM2(吸気温), 4=NUM3(油温), 5=NUM4(MAP), 6=NUM5(油圧), 7=NUM6(電圧), 8=SPEED
            var defaults = new[]
            {
                // CH1 (0x3E8): RPM(0-1), MAP(4-5), IAT(6-7)
                new { Ch = 1, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = true,
                      Name = "REV", Unit = "rpm", WarnEn = true, WarnLo = 200, WarnHi = 9000 },
                new { Ch = 1, Byte = 4, Size = 2, Type = "U", End = "B", Var = 5, Off = 0, Mul = 1000, Div = 10000, En = true,
                      Name = "MAP", Unit = "kPa", WarnEn = true, WarnLo = 0, WarnHi = 150 },
                new { Ch = 1, Byte = 6, Size = 2, Type = "S", End = "B", Var = 3, Off = 0, Mul = 1000, Div = 10000, En = true,
                      Name = "IAT", Unit = "deg", WarnEn = true, WarnLo = -40, WarnHi = 80 },
                // CH2 (0x3E9): ECT(0-1), AFR(2-3)
                new { Ch = 2, Byte = 0, Size = 2, Type = "S", End = "B", Var = 2, Off = 0, Mul = 1000, Div = 10000, En = true,
                      Name = "WATER", Unit = "deg", WarnEn = true, WarnLo = 60, WarnHi = 110 },
                new { Ch = 2, Byte = 2, Size = 2, Type = "U", End = "B", Var = 1, Off = 0, Mul = 147, Div = 1000, En = true,
                      Name = "A/F", Unit = "afr", WarnEn = true, WarnLo = 100, WarnHi = 180 },
                // CH3 (0x3EA): OilTemp(6-7)
                new { Ch = 3, Byte = 6, Size = 2, Type = "S", End = "B", Var = 4, Off = 0, Mul = 1000, Div = 10000, En = true,
                      Name = "OIL-T", Unit = "deg", WarnEn = true, WarnLo = 60, WarnHi = 130 },
                // CH4 (0x3EB): OilPressure(0-1), BattV(6-7)
                new { Ch = 4, Byte = 0, Size = 2, Type = "U", End = "B", Var = 6, Off = 0, Mul = 1, Div = 1000, En = true,
                      Name = "OIL_P", Unit = "kPa", WarnEn = true, WarnLo = 1500, WarnHi = 9000 },
                new { Ch = 4, Byte = 6, Size = 2, Type = "U", End = "B", Var = 7, Off = 0, Mul = 1000, Div = 10000, En = true,
                      Name = "BATT", Unit = "V", WarnEn = false, WarnLo = 9, WarnHi = 160 },
            };

            for (int i = 0; i < defaults.Length; i++)
            {
                var d = defaults[i];
                _canFields.Add(new CanFieldItem
                {
                    Index = i,
                    Channel = d.Ch,
                    StartByte = d.Byte,
                    ByteCount = d.Size,
                    DataType = d.Type,
                    Endian = d.End,
                    TargetVar = d.Var,
                    Offset = d.Off,
                    Multiplier = d.Mul,
                    Divisor = d.Div,
                    Enabled = d.En,
                    Name = d.Name,
                    Unit = d.Unit,
                    WarnEnabled = d.WarnEn,
                    WarnLow = d.WarnLo,
                    WarnHigh = d.WarnHi
                });
            }

            UpdateCanStatus("デフォルト値に戻しました（書込ボタンで適用）");
        }
    }

    private static int ParseCanId(string text)
    {
        text = text.Trim();
        if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        {
            if (int.TryParse(text.Substring(2), System.Globalization.NumberStyles.HexNumber, null, out int hexResult))
                return hexResult;
        }
        if (int.TryParse(text, out int decResult))
            return decResult;
        return 0;
    }

    #endregion
}
