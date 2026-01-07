using System.Text;
using System.Text.RegularExpressions;
using System.Windows;
using FullmoniTerminal.Services;

namespace FullmoniTerminal.Views;

/// <summary>
/// パラメータ編集ウィンドウ
/// </summary>
public partial class ParameterEditorWindow : Window
{
    private readonly SerialPortService _serialService;
    private readonly Action<string> _sendCommand;

    public ParameterEditorWindow(SerialPortService serialService, Action<string> sendCommand)
    {
        InitializeComponent();
        _serialService = serialService;
        _sendCommand = sendCommand;

        // テキストボックスの変更イベントを登録
        TyreWidthBox.TextChanged += TyreSize_TextChanged;
        TyreAspectBox.TextChanged += TyreSize_TextChanged;
        TyreRimBox.TextChanged += TyreSize_TextChanged;
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
}
