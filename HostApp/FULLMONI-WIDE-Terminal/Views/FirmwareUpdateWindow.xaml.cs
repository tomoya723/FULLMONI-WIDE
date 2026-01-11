using System.IO;
using System.Windows;
using FullmoniTerminal.Services;
using Microsoft.Win32;

namespace FullmoniTerminal.Views;

/// <summary>
/// ファームウェアアップデートウィンドウ
/// </summary>
public partial class FirmwareUpdateWindow : Window
{
    private readonly SerialPortService _serialService;
    private readonly Action<string> _sendCommand;
    private readonly FirmwareUpdateService _updateService;
    private string? _selectedFilePath;
    private byte[]? _firmwareData;
    private bool _isUpdating;
    private CancellationTokenSource? _cts;

    public FirmwareUpdateWindow(SerialPortService serialService, Action<string> sendCommand)
    {
        InitializeComponent();
        _serialService = serialService;
        _sendCommand = sendCommand;
        _updateService = new FirmwareUpdateService(serialService);

        // イベント登録
        _updateService.ProgressChanged += UpdateService_ProgressChanged;
        _updateService.StatusChanged += UpdateService_StatusChanged;
        _updateService.UpdateCompleted += UpdateService_UpdateCompleted;
    }

    private void BrowseButton_Click(object sender, RoutedEventArgs e)
    {
        var dialog = new OpenFileDialog
        {
            Filter = "Binary files (*.bin)|*.bin|All files (*.*)|*.*",
            Title = "ファームウェアファイルを選択"
        };

        if (dialog.ShowDialog() == true)
        {
            _selectedFilePath = dialog.FileName;
            FilePathBox.Text = _selectedFilePath;

            try
            {
                var fileInfo = new FileInfo(_selectedFilePath);
                FileSizeText.Text = $"{fileInfo.Length:N0} bytes ({fileInfo.Length / 1024.0:F1} KB)";

                // BINファイルを読み込む
                _firmwareData = FirmwareUpdateService.LoadBinFile(_selectedFilePath);
                AppendLog($"バイナリファイルを読み込みました: {_firmwareData.Length:N0} bytes");
            }
            catch (Exception ex)
            {
                AppendLog($"エラー: ファイル読み込み失敗 - {ex.Message}");
                _firmwareData = null;
            }
        }
    }

    private async void Step1Button_Click(object sender, RoutedEventArgs e)
    {
        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        var result = MessageBox.Show(
            "ファームウェアを更新するため、デバイスをブートローダーモードに移行します。\n\n" +
            "続行しますか？",
            "確認",
            MessageBoxButton.YesNo,
            MessageBoxImage.Warning);

        if (result != MessageBoxResult.Yes)
        {
            return;
        }

        AppendLog("=== Step 1: ブートローダーへ移行 ===");
        AppendLog("fwupdateコマンドを送信中...");

        // fwupdateコマンドを送信
        _sendCommand("fwupdate");

        // ユーザーに確認入力を求める
        await Task.Delay(1000);
        AppendLog("ターミナルで 'yes' と入力して確認してください。");

        // 確認待ち（ユーザーがターミナルで'yes'を入力する）
        await Task.Delay(2000);
        _sendCommand("yes");

        AppendLog("デバイスがリブートしてブートローダーが起動するまで待機...");
        AppendLog("（約5秒後に接続が切断される場合があります）");

        // ブートローダー起動を待つ
        await Task.Delay(5000);

        Step1Button.IsEnabled = false;
        Step2Button.IsEnabled = true;

        AppendLog("Step 2 に進んでください。");
        AppendLog("※ 接続が切れた場合は再接続してください。");
    }

    private async void Step2Button_Click(object sender, RoutedEventArgs e)
    {
        if (_firmwareData == null || _firmwareData.Length == 0)
        {
            MessageBox.Show("ファームウェアファイルを選択してください。", "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        if (!_serialService.IsConnected)
        {
            MessageBox.Show("シリアルポートに接続してください。\n" +
                          "ブートローダーモードでデバイスが起動している必要があります。",
                          "エラー", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }

        var result = MessageBox.Show(
            $"ファームウェア転送を開始します。\n\n" +
            $"ファイル: {Path.GetFileName(_selectedFilePath)}\n" +
            $"サイズ: {_firmwareData.Length:N0} bytes\n\n" +
            "転送中は絶対に電源を切らないでください。\n" +
            "続行しますか？",
            "最終確認",
            MessageBoxButton.YesNo,
            MessageBoxImage.Warning);

        if (result != MessageBoxResult.Yes)
        {
            return;
        }

        _isUpdating = true;
        Step1Button.IsEnabled = false;
        Step2Button.IsEnabled = false;

        _cts = new CancellationTokenSource();

        AppendLog("=== Step 2: ファームウェア転送 ===");

        try
        {
            // このウィンドウではすでにブートローダーモードに切り替え済みの想定
            await _updateService.SendFirmwareAsync(_firmwareData, switchToBootloader: false, _cts.Token);
        }
        catch (OperationCanceledException)
        {
            AppendLog("転送がキャンセルされました。");
        }
        catch (Exception ex)
        {
            AppendLog($"エラー: {ex.Message}");
            MessageBox.Show($"ファームウェア転送中にエラーが発生しました:\n{ex.Message}",
                          "エラー", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        finally
        {
            _isUpdating = false;
        }
    }

    private void UpdateService_ProgressChanged(object? sender, int progress)
    {
        Dispatcher.Invoke(() =>
        {
            ProgressBar.Value = progress;
            ProgressText.Text = $"{progress}%";
        });
    }

    private void UpdateService_StatusChanged(object? sender, string message)
    {
        Dispatcher.Invoke(() => AppendLog(message));
    }

    private void UpdateService_UpdateCompleted(object? sender, bool success)
    {
        Dispatcher.Invoke(() =>
        {
            if (success)
            {
                AppendLog("=== アップデート完了 ===");
                MessageBox.Show("ファームウェアの更新が完了しました。\n" +
                              "デバイスは自動的に再起動します。",
                              "完了", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            else
            {
                AppendLog("=== アップデート失敗 ===");
                MessageBox.Show("ファームウェアの更新に失敗しました。\n" +
                              "デバイスの電源を入れ直して再試行してください。",
                              "エラー", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            Step1Button.IsEnabled = true;
            Step2Button.IsEnabled = false;
        });
    }

    private void AppendLog(string message)
    {
        StatusText.Text = message;
    }

    private void CloseButton_Click(object sender, RoutedEventArgs e)
    {
        if (_isUpdating)
        {
            var result = MessageBox.Show("転送中です。キャンセルしますか？", "確認",
                                        MessageBoxButton.YesNo, MessageBoxImage.Warning);
            if (result == MessageBoxResult.Yes)
            {
                _cts?.Cancel();
            }
            else
            {
                return;
            }
        }

        Close();
    }

    private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
    {
        if (_isUpdating)
        {
            var result = MessageBox.Show("転送中です。キャンセルしますか？", "確認",
                                        MessageBoxButton.YesNo, MessageBoxImage.Warning);
            if (result != MessageBoxResult.Yes)
            {
                e.Cancel = true;
                return;
            }
            _cts?.Cancel();
        }

        _updateService.ProgressChanged -= UpdateService_ProgressChanged;
        _updateService.StatusChanged -= UpdateService_StatusChanged;
        _updateService.UpdateCompleted -= UpdateService_UpdateCompleted;
    }
}
