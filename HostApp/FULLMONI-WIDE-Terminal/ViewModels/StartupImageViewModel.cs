using System.Windows;
using System.Windows.Media.Imaging;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FullmoniTerminal.Services;

namespace FullmoniTerminal.ViewModels;

/// <summary>
/// 起動画面書き込みのViewModel
/// </summary>
public partial class StartupImageViewModel : ObservableObject, IDisposable
{
    private readonly SerialPortService _serialService;
    private readonly StartupImageService _imageService;
    private CancellationTokenSource? _cancellationTokenSource;
    private bool _disposed;

    public StartupImageViewModel(SerialPortService serialService)
    {
        _serialService = serialService;
        _imageService = new StartupImageService(_serialService);

        // イベント登録
        _imageService.ProgressChanged += ImageService_ProgressChanged;
        _imageService.StatusChanged += ImageService_StatusChanged;
        _imageService.LogMessage += ImageService_LogMessage;
        _imageService.WriteCompleted += ImageService_WriteCompleted;

        _serialService.ConnectionChanged += SerialService_ConnectionChanged;
    }

    #region Properties

    [ObservableProperty]
    private string _imageFilePath = "";

    partial void OnImageFilePathChanged(string value)
    {
        OnPropertyChanged(nameof(CanWrite));
    }

    [ObservableProperty]
    private BitmapImage? _previewImage;

    [ObservableProperty]
    private int _progress;

    [ObservableProperty]
    private string _statusMessage = "画像を選択してください";

    [ObservableProperty]
    private bool _isWriting;

    partial void OnIsWritingChanged(bool value)
    {
        OnPropertyChanged(nameof(CanSelectImage));
        OnPropertyChanged(nameof(CanWrite));
        OnPropertyChanged(nameof(CanCancel));
        OnPropertyChanged(nameof(CanRead));
    }

    [ObservableProperty]
    private string _log = "";

    [ObservableProperty]
    private bool _isConnected;

    /// <summary>
    /// 画像選択が可能かどうか
    /// </summary>
    public bool CanSelectImage => !IsWriting;

    /// <summary>
    /// 書き込み可能かどうか
    /// </summary>
    public bool CanWrite => IsConnected && !string.IsNullOrEmpty(ImageFilePath) && !IsWriting;

    /// <summary>
    /// キャンセル可能かどうか
    /// </summary>
    public bool CanCancel => IsWriting;

    /// <summary>
    /// 読み込み可能かどうか
    /// </summary>
    public bool CanRead => IsConnected && !IsWriting;

    #endregion

    #region Commands

    [RelayCommand]
    private void SelectImage()
    {
        var dialog = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "BMPファイル (*.bmp)|*.bmp|すべてのファイル (*.*)|*.*",
            Title = "起動画面BMPファイルを選択"
        };

        if (dialog.ShowDialog() == true)
        {
            try
            {
                // BMPファイルを検証
                var imageData = StartupImageService.LoadAndValidateBmp(dialog.FileName);

                // プレビュー画像を読み込み
                var bitmap = new BitmapImage();
                bitmap.BeginInit();
                bitmap.UriSource = new Uri(dialog.FileName);
                bitmap.CacheOption = BitmapCacheOption.OnLoad;
                bitmap.EndInit();

                PreviewImage = bitmap;
                ImageFilePath = dialog.FileName;
                StatusMessage = $"画像を読み込みました: {imageData.Length:N0} bytes (765×256)";
                AppendLog($"画像を選択: {dialog.FileName}");
                AppendLog($"サイズ: {imageData.Length:N0} bytes");
            }
            catch (Exception ex)
            {
                StatusMessage = $"エラー: {ex.Message}";
                AppendLog($"エラー: {ex.Message}");

                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"画像の読み込みに失敗しました。\n\n{ex.Message}",
                    "エラー",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
            }
        }
    }

    [RelayCommand]
    private async Task WriteImage()
    {
        if (!IsConnected || string.IsNullOrEmpty(ImageFilePath) || IsWriting)
        {
            return;
        }

        // 確認ダイアログ
        var result = await Application.Current.Dispatcher.InvokeAsync(() =>
            MessageBox.Show(
                Application.Current.MainWindow,
                "起動画面をRX72Nのフラッシュメモリに書き込みます。\n\n" +
                "【重要】\n" +
                "• 書き込み中は絶対に電源を切らないでください\n" +
                "• 既存の起動画面は上書きされます\n" +
                "• 完了後、デバイスを再起動してください\n\n" +
                "書き込みを開始しますか？",
                "確認",
                MessageBoxButton.YesNo,
                MessageBoxImage.Warning));

        if (result != MessageBoxResult.Yes)
        {
            return;
        }

        IsWriting = true;
        Progress = 0;
        Log = "";
        StatusMessage = "準備中...";

        _cancellationTokenSource = new CancellationTokenSource();

        try
        {
            // BMPデータを読み込み
            var imageData = await Task.Run(() =>
                StartupImageService.LoadAndValidateBmp(ImageFilePath));

            // フラッシュに書き込み
            await _imageService.WriteToFlashAsync(imageData, _cancellationTokenSource.Token);

            // 成功 - デバイスは自動再起動するので再接続を試みる
            Progress = 100;
            StatusMessage = "書き込み完了。デバイス再起動後に再接続中...";
            AppendLog("デバイス再起動待ち...");

            // COMポート情報を保存
            var portName = _serialService.CurrentPortName;
            var baudRate = _serialService.CurrentBaudRate;

            // 再接続を試みる（デバイスが再起動するまで待つ）
            if (!string.IsNullOrEmpty(portName))
            {
                var reconnected = await _serialService.ReconnectAsync(portName, baudRate, 20, 500);
                if (reconnected)
                {
                    AppendLog("デバイスに再接続しました");
                    StatusMessage = "起動画面の書き込みが完了しました";
                }
                else
                {
                    AppendLog("再接続に失敗しました。手動で接続してください。");
                    StatusMessage = "書き込み完了（再接続に失敗）";
                }
            }

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    "起動画面の書き込みが完了しました。\n\n" +
                    "デバイスは自動的に再起動され、新しい起動画面が表示されます。",
                    "完了",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information));
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "書き込みがキャンセルされました";
            AppendLog("書き込みがキャンセルされました");
        }
        catch (Exception ex)
        {
            StatusMessage = $"エラー: {ex.Message}";
            AppendLog($"エラー: {ex.Message}");

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"起動画面の書き込み中にエラーが発生しました。\n\n{ex.Message}",
                    "エラー",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error));
        }
        finally
        {
            IsWriting = false;
            _cancellationTokenSource?.Dispose();
            _cancellationTokenSource = null;
        }
    }

    [RelayCommand]
    private void Cancel()
    {
        _cancellationTokenSource?.Cancel();
        StatusMessage = "キャンセル中...";
    }

    [RelayCommand]
    private void ClearLog()
    {
        Log = "";
    }

    [RelayCommand]
    private void CopyLog()
    {
        if (!string.IsNullOrEmpty(Log))
        {
            Clipboard.SetText(Log);
            StatusMessage = "ログをクリップボードにコピーしました";
        }
    }

    [RelayCommand]
    private async Task ReadFromDevice()
    {
        if (!IsConnected || IsWriting)
        {
            return;
        }

        IsWriting = true;  // 読み込み中も同じフラグを使用
        Progress = 0;
        // ログはクリアしない（履歴を残す）
        AppendLog("\n========================================");
        StatusMessage = "デバイスから起動画面を読み込み中...";

        _cancellationTokenSource = new CancellationTokenSource();

        try
        {
            AppendLog("起動画面読み込み開始");

            // フラッシュから読み込み
            var imageData = await _imageService.ReadFromFlashAsync(_cancellationTokenSource.Token);

            AppendLog($"受信データ: {imageData.Length:N0} bytes");

            // AppWizard形式からBMPに変換してプレビュー表示
            var bmpData = AppWizardImageConverter.ConvertAppWizardToBmp(imageData);

            // BMPをプレビュー表示
            using var stream = new System.IO.MemoryStream(bmpData);
            var bitmap = new BitmapImage();
            bitmap.BeginInit();
            bitmap.StreamSource = stream;
            bitmap.CacheOption = BitmapCacheOption.OnLoad;
            bitmap.EndInit();
            bitmap.Freeze();

            PreviewImage = bitmap;
            ImageFilePath = "(デバイスから読み込み)";
            StatusMessage = "起動画面の読み込みが完了しました";
            AppendLog("読み込み完了");
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "読み込みがキャンセルされました";
            AppendLog("読み込みがキャンセルされました");
        }
        catch (Exception ex)
        {
            StatusMessage = $"エラー: {ex.Message}";
            AppendLog($"エラー: {ex.Message}");

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"起動画面の読み込み中にエラーが発生しました。\n\n{ex.Message}",
                    "エラー",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error));
        }
        finally
        {
            IsWriting = false;
            _cancellationTokenSource?.Dispose();
            _cancellationTokenSource = null;
        }
    }

    #endregion

    #region Event Handlers

    private void ImageService_ProgressChanged(object? sender, int progress)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            Progress = progress;
        });
    }

    private void ImageService_StatusChanged(object? sender, string status)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            StatusMessage = status;
        });
    }

    private void ImageService_LogMessage(object? sender, string message)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            AppendLog(message);
        });
    }

    private void ImageService_WriteCompleted(object? sender, bool success)
    {
        // WriteImageメソッドで処理済み
    }

    private void SerialService_ConnectionChanged(object? sender, bool connected)
    {
        Application.Current.Dispatcher.Invoke(() =>
        {
            IsConnected = connected;
            OnPropertyChanged(nameof(CanWrite));
            OnPropertyChanged(nameof(CanRead));

            if (!connected && IsWriting)
            {
                StatusMessage = "接続が切断されました";
                _cancellationTokenSource?.Cancel();
            }
        });
    }

    #endregion

    #region Private Methods

    private void AppendLog(string message)
    {
        var timestamp = DateTime.Now.ToString("HH:mm:ss.fff");
        Log += $"[{timestamp}] {message}\n";
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
                _imageService.ProgressChanged -= ImageService_ProgressChanged;
                _imageService.StatusChanged -= ImageService_StatusChanged;
                _imageService.LogMessage -= ImageService_LogMessage;
                _imageService.WriteCompleted -= ImageService_WriteCompleted;
                _serialService.ConnectionChanged -= SerialService_ConnectionChanged;

                _cancellationTokenSource?.Dispose();
            }
            _disposed = true;
        }
    }

    ~StartupImageViewModel()
    {
        Dispose(false);
    }

    #endregion
}
