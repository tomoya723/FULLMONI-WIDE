using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using FullmoniTerminal.Services;

namespace FullmoniTerminal.Views;

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

    public int Index { get => _index; set { _index = value; OnPropertyChanged(nameof(Index)); } }
    public int Channel { get => _channel; set { _channel = value; OnPropertyChanged(nameof(Channel)); } }
    public int StartByte { get => _startByte; set { _startByte = value; OnPropertyChanged(nameof(StartByte)); } }
    public int ByteCount { get => _byteCount; set { _byteCount = value; OnPropertyChanged(nameof(ByteCount)); } }
    public string DataType { get => _dataType; set { _dataType = value; OnPropertyChanged(nameof(DataType)); } }
    public string Endian { get => _endian; set { _endian = value; OnPropertyChanged(nameof(Endian)); } }
    public int TargetVar { get => _targetVar; set { _targetVar = value; OnPropertyChanged(nameof(TargetVar)); } }
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
/// パラメータ編集ウィンドウ
/// </summary>
public partial class ParameterEditorWindow : Window
{
    private readonly SerialPortService _serialService;
    private readonly Action<string> _sendCommand;
    private ObservableCollection<CanFieldItem> _canFields = new();

    public ParameterEditorWindow(SerialPortService serialService, Action<string> sendCommand)
    {
        InitializeComponent();
        _serialService = serialService;
        _sendCommand = sendCommand;

        // テキストボックスの変更イベントを登録
        TyreWidthBox.TextChanged += TyreSize_TextChanged;
        TyreAspectBox.TextChanged += TyreSize_TextChanged;
        TyreRimBox.TextChanged += TyreSize_TextChanged;

        // CANフィールドの初期化
        InitializeCanFields();
    }

    private void InitializeCanFields()
    {
        _canFields = new ObservableCollection<CanFieldItem>();

        // Issue #50: MoTeC M100デフォルト設定 (警告閾値付き)
        // WarnLow/WarnHigh: -32768 = 無効 (CAN_WARN_DISABLED)
        var defaults = new[]
        {
            //    Ch  Byte Size Type End Var   Off  Mul    Div     En   Name    Unit  WarnEn  WarnLo WarnHi
            new { Ch = 1, Byte = 0, Size = 2, Type = "U", End = "B", Var = 10, Off = 0, Mul = 1000, Div = 1000, En = true,
                  Name = "RPM", Unit = "rpm", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 1, Byte = 4, Size = 2, Type = "U", End = "B", Var = 4, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "MAP", Unit = "kPa", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 1, Byte = 6, Size = 2, Type = "S", End = "B", Var = 2, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "IAT", Unit = "C", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 2, Byte = 0, Size = 2, Type = "S", End = "B", Var = 1, Off = 0, Mul = 1000, Div = 10000, En = true,
                  Name = "WATER", Unit = "C", WarnEn = true, WarnLo = 60, WarnHi = 100 },  // 水温警告
            new { Ch = 2, Byte = 2, Size = 2, Type = "U", End = "B", Var = 11, Off = 0, Mul = 147, Div = 1000, En = true,
                  Name = "AFR", Unit = "", WarnEn = true, WarnLo = 110, WarnHi = 180 },    // A/F警告 (x10)
            new { Ch = 2, Byte = 4, Size = 2, Type = "U", End = "B", Var = 5, Off = 0, Mul = 1000, Div = 10000, En = false,
                  Name = "PRESS", Unit = "kPa", WarnEn = true, WarnLo = 100, WarnHi = -32768 },  // 油圧警告 (下限のみ)
            new { Ch = 3, Byte = 0, Size = 2, Type = "S", End = "B", Var = 3, Off = 0, Mul = 1000, Div = 10000, En = false,
                  Name = "OIL", Unit = "C", WarnEn = true, WarnLo = -32768, WarnHi = 130 },  // 油温警告 (上限のみ)
            new { Ch = 4, Byte = 0, Size = 2, Type = "U", End = "B", Var = 12, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "SPEED", Unit = "km", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 4, Byte = 2, Size = 1, Type = "U", End = "B", Var = 13, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "GEAR", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            // 追加フィールド (予備)
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
            new { Ch = 0, Byte = 0, Size = 2, Type = "U", End = "B", Var = 0, Off = 0, Mul = 1000, Div = 1000, En = false,
                  Name = "", Unit = "", WarnEn = false, WarnLo = -32768, WarnHi = -32768 },
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

        CanFieldsGrid.ItemsSource = _canFields;
    }

    private void TyreSize_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
    {
        UpdateTyrePreview();
    }

    private void UpdateTyrePreview()
    {
        var width = TyreWidthBox.Text;
        var aspect = TyreAspectBox.Text;
        var rim = TyreRimBox.Text;

        if (!string.IsNullOrEmpty(width) && !string.IsNullOrEmpty(aspect) && !string.IsNullOrEmpty(rim))
        {
            TyreSizePreview.Text = $"プレビュー: {width}/{aspect} R{rim}";
        }
    }

    private async void ReadButton_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        // listコマンドで現在のパラメータを取得
        _sendCommand("list");

        // 応答を待つ（少し遅延）
        await Task.Delay(500);

        MessageBox.Show("ターミナルの出力からパラメータを確認してください。\n" +
                       "（自動パース機能は今後実装予定）", "情報", MessageBoxButton.OK, MessageBoxImage.Information);
    }

    private void WriteButton_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        var commands = new List<string>();

        // タイヤサイズ
        if (!string.IsNullOrEmpty(TyreWidthBox.Text))
            commands.Add($"set tyre_width {TyreWidthBox.Text}");
        if (!string.IsNullOrEmpty(TyreAspectBox.Text))
            commands.Add($"set tyre_aspect {TyreAspectBox.Text}");
        if (!string.IsNullOrEmpty(TyreRimBox.Text))
            commands.Add($"set tyre_rim {TyreRimBox.Text}");

        // ギア比
        if (!string.IsNullOrEmpty(Gear1Box.Text))
            commands.Add($"set gear1 {Gear1Box.Text}");
        if (!string.IsNullOrEmpty(Gear2Box.Text))
            commands.Add($"set gear2 {Gear2Box.Text}");
        if (!string.IsNullOrEmpty(Gear3Box.Text))
            commands.Add($"set gear3 {Gear3Box.Text}");
        if (!string.IsNullOrEmpty(Gear4Box.Text))
            commands.Add($"set gear4 {Gear4Box.Text}");
        if (!string.IsNullOrEmpty(Gear5Box.Text))
            commands.Add($"set gear5 {Gear5Box.Text}");
        if (!string.IsNullOrEmpty(Gear6Box.Text))
            commands.Add($"set gear6 {Gear6Box.Text}");
        if (!string.IsNullOrEmpty(FinalGearBox.Text))
            commands.Add($"set final {FinalGearBox.Text}");

        // 警告設定
        if (!string.IsNullOrEmpty(WaterTempLowBox.Text))
            commands.Add($"set water_low {WaterTempLowBox.Text}");
        if (!string.IsNullOrEmpty(WaterTempHighBox.Text))
            commands.Add($"set water_high {WaterTempHighBox.Text}");
        if (!string.IsNullOrEmpty(FuelWarnBox.Text))
            commands.Add($"set fuel_warn {FuelWarnBox.Text}");

        if (commands.Count == 0)
        {
            MessageBox.Show("設定する値を入力してください。", "情報", MessageBoxButton.OK, MessageBoxImage.Information);
            return;
        }

        // コマンドを順番に送信
        foreach (var cmd in commands)
        {
            _sendCommand(cmd);
            Thread.Sleep(100); // 少し待機
        }

        MessageBox.Show($"{commands.Count}個のパラメータを送信しました。\n" +
                       "EEPROMに保存するには「保存」ボタンを押してください。",
                       "完了", MessageBoxButton.OK, MessageBoxImage.Information);
    }

    private void SaveButton_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        var result = MessageBox.Show("パラメータをEEPROMに保存しますか？", "確認",
                                     MessageBoxButton.YesNo, MessageBoxImage.Question);
        if (result == MessageBoxResult.Yes)
        {
            _sendCommand("save");
            MessageBox.Show("保存コマンドを送信しました。", "情報", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }

    private void CloseButton_Click(object sender, RoutedEventArgs e)
    {
        Close();
    }

    #region CAN設定

    private async void CanReadButton_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        // Issue #65: can_list コマンドで現在の設定を取得
        _sendCommand("can_list");

        await Task.Delay(500);

        MessageBox.Show("ターミナルの出力からCAN設定を確認してください。\n" +
                       "（自動パース機能は今後実装予定）", "情報", MessageBoxButton.OK, MessageBoxImage.Information);
    }

    private void CanWriteButton_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        var commands = new List<string>();

        // CANチャンネル設定 (Issue #65: 新コマンド名 can_ch)
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
                commands.Add($"can_ch {i} {canId} {enabled}");
            }
        }

        // CANフィールド設定 (Issue #50: 警告設定対応)
        foreach (var field in _canFields)
        {
            if (field.Enabled)
            {
                var dataType = field.DataType == "S" ? 1 : 0;
                var endian = field.Endian == "L" ? 1 : 0;
                var warnEn = field.WarnEnabled ? 1 : 0;

                // 名前と単位はスペースを含まない (空文字の場合は "-" を送信)
                var name = string.IsNullOrWhiteSpace(field.Name) ? "-" : field.Name.Replace(" ", "_");
                var unit = string.IsNullOrWhiteSpace(field.Unit) ? "-" : field.Unit.Replace(" ", "_");

                // can_field <n> <ch> <byte> <len> <type> <end> <var> <off> <mul> <div> <name> <unit> <warn_en> <warn_lo> <warn_hi>
                commands.Add($"can_field {field.Index} {field.Channel} {field.StartByte} {field.ByteCount} {dataType} {endian} {field.TargetVar} {field.Offset} {field.Multiplier} {field.Divisor} {name} {unit} {warnEn} {field.WarnLow} {field.WarnHigh}");
            }
        }

        if (commands.Count == 0)
        {
            MessageBox.Show("設定するCAN項目がありません。", "情報", MessageBoxButton.OK, MessageBoxImage.Information);
            return;
        }

        // コマンドを順番に送信
        foreach (var cmd in commands)
        {
            _sendCommand(cmd);
            Thread.Sleep(150); // CAN設定は少し長めに待機
        }

        MessageBox.Show($"{commands.Count}個のCAN設定を送信しました。\n" +
                       "EEPROMに保存するには「CAN保存(EEPROM)」ボタンを押してください。",
                       "完了", MessageBoxButton.OK, MessageBoxImage.Information);
    }

    private void CanSaveButton_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        var result = MessageBox.Show("CAN設定をEEPROMに保存しますか？", "確認",
                                     MessageBoxButton.YesNo, MessageBoxImage.Question);
        if (result == MessageBoxResult.Yes)
        {
            _sendCommand("can_save");
            MessageBox.Show("CAN保存コマンド(can_save)を送信しました。", "情報", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }

    private int ParseCanId(string text)
    {
        text = text.Trim();

        // 0x prefix
        if (text.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        {
            if (int.TryParse(text.Substring(2), System.Globalization.NumberStyles.HexNumber, null, out int hexResult))
            {
                return hexResult;
            }
        }

        // Decimal
        if (int.TryParse(text, out int decResult))
        {
            return decResult;
        }

        return 0;
    }

    #endregion
}
