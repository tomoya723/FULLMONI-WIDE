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
    private byte[]? _loadedBmpData;  // デバイスから読み込んだBMPデータ
    private byte[]? _loadedAppWizardData;  // デバイスから読み込んだAppWizardデータ（書き戻し用）

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
    private string _statusMessage = "Please select an image";

    [ObservableProperty]
    private bool _isWriting;

    partial void OnIsWritingChanged(bool value)
    {
        OnPropertyChanged(nameof(CanSelectImage));
        OnPropertyChanged(nameof(CanWrite));
        OnPropertyChanged(nameof(CanCancel));
        OnPropertyChanged(nameof(CanRead));
        OnPropertyChanged(nameof(CanSave));
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
    /// 書き込み可能かどうか（ファイル選択または読み込みデータがある場合）
    /// </summary>
    public bool CanWrite => IsConnected && !IsWriting &&
        (!string.IsNullOrEmpty(ImageFilePath) || _loadedAppWizardData != null);

    /// <summary>
    /// キャンセル可能かどうか
    /// </summary>
    public bool CanCancel => IsWriting;

    /// <summary>
    /// 読み込み可能かどうか
    /// </summary>
    public bool CanRead => IsConnected && !IsWriting;

    /// <summary>
    /// BMP保存可能かどうか
    /// </summary>
    public bool CanSave => _loadedBmpData != null && !IsWriting;

    #endregion

    #region Commands

    [RelayCommand]
    private void SelectImage()
    {
        var dialog = new Microsoft.Win32.OpenFileDialog
        {
            Filter = "BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*",
            Title = "Select Boot Screen BMP Image"
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
                StatusMessage = $"Image loaded: {imageData.Length:N0} bytes (765×256)";
                AppendLog($"Selected: {dialog.FileName}");
                AppendLog($"Size: {imageData.Length:N0} bytes");
            }
            catch (Exception ex)
            {
                StatusMessage = $"Error: {ex.Message}";
                AppendLog($"Error: {ex.Message}");

                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"Failed to load image.\n\n{ex.Message}",
                    "Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
            }
        }
    }

    [RelayCommand]
    private async Task WriteImage()
    {
        if (!IsConnected || IsWriting)
        {
            return;
        }

        // ファイルパスもロードデータもない場合は終了
        if (string.IsNullOrEmpty(ImageFilePath) && _loadedAppWizardData == null)
        {
            return;
        }

        // Confirmation dialog
        var result = await Application.Current.Dispatcher.InvokeAsync(() =>
            MessageBox.Show(
                Application.Current.MainWindow,
                "Write boot screen to RX72N flash memory.\n\n" +
                "[IMPORTANT]\n" +
                "• Do NOT turn off power during writing\n" +
                "• Existing boot screen will be overwritten\n" +
                "• Device will restart after completion\n\n" +
                "Start writing?",
                "Confirm",
                MessageBoxButton.YesNo,
                MessageBoxImage.Warning));

        if (result != MessageBoxResult.Yes)
        {
            return;
        }

        IsWriting = true;
        Progress = 0;
        Log = "";
        StatusMessage = "Preparing...";

        _cancellationTokenSource = new CancellationTokenSource();

        try
        {
            // Prepare image data (use loaded data if available, otherwise load from file)
            byte[] imageData;
            if (_loadedAppWizardData != null)
            {
                imageData = _loadedAppWizardData;
                AppendLog("Using loaded data");
            }
            else
            {
                imageData = await Task.Run(() =>
                    StartupImageService.LoadAndValidateBmp(ImageFilePath));
                AppendLog($"Loaded from file: {ImageFilePath}");
            }

            // フラッシュに書き込み
            await _imageService.WriteToFlashAsync(imageData, _cancellationTokenSource.Token);

            // Success - device auto-restarts, try to reconnect
            Progress = 100;
            StatusMessage = "Write complete. Reconnecting after device restart...";
            AppendLog("Waiting for device restart...");

            // COMポート情報を保存
            var portName = _serialService.CurrentPortName;
            var baudRate = _serialService.CurrentBaudRate;

            // 再接続を試みる（デバイスが再起動するまで待つ）
            if (!string.IsNullOrEmpty(portName))
            {
                var reconnected = await _serialService.ReconnectAsync(portName, baudRate, 20, 500);
                if (reconnected)
                {
                    AppendLog("Reconnected to device");
                    StatusMessage = "Boot screen write completed";
                }
                else
                {
                    AppendLog("Reconnection failed. Please connect manually.");
                    StatusMessage = "Write complete (reconnection failed)";
                }
            }

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    "Boot screen write completed.\n\n" +
                    "Device will restart automatically and display the new boot screen.",
                    "Complete",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information));
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "Write cancelled";
            AppendLog("Write cancelled");
        }
        catch (Exception ex)
        {
            StatusMessage = $"Error: {ex.Message}";
            AppendLog($"Error: {ex.Message}");

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"Error during boot screen write.\n\n{ex.Message}",
                    "Error",
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
        StatusMessage = "Cancelling...";
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
            StatusMessage = "Log copied to clipboard";
        }
    }

    [RelayCommand]
    private void SaveImage()
    {
        if (_loadedBmpData == null)
        {
            StatusMessage = "No image to save";
            return;
        }

        var dialog = new Microsoft.Win32.SaveFileDialog
        {
            Filter = "BMP Files (*.bmp)|*.bmp",
            Title = "Save Boot Screen as BMP",
            FileName = $"startup_image_{DateTime.Now:yyyyMMdd_HHmmss}.bmp"
        };

        if (dialog.ShowDialog() == true)
        {
            try
            {
                System.IO.File.WriteAllBytes(dialog.FileName, _loadedBmpData);
                StatusMessage = $"Image saved: {dialog.FileName}";
                AppendLog($"Saved BMP: {dialog.FileName}");
            }
            catch (Exception ex)
            {
                StatusMessage = $"Save error: {ex.Message}";
                AppendLog($"Save error: {ex.Message}");

                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"Failed to save image.\n\n{ex.Message}",
                    "Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);
            }
        }
    }

    [RelayCommand]
    private async Task ReadFromDevice()
    {
        if (!IsConnected || IsWriting)
        {
            return;
        }

        IsWriting = true;  // Use same flag during reading
        Progress = 0;
        // Don't clear log (keep history)
        AppendLog("\n========================================");
        StatusMessage = "Reading boot screen from device...";

        _cancellationTokenSource = new CancellationTokenSource();

        try
        {
            AppendLog("Starting boot screen read");

            // フラッシュから読み込み
            var imageData = await _imageService.ReadFromFlashAsync(_cancellationTokenSource.Token);

            AppendLog($"Received data: {imageData.Length:N0} bytes");

            // AppWizard形式からBMPに変換してプレビュー表示
            var bmpData = AppWizardImageConverter.ConvertAppWizardToBmp(imageData);

            // データを保持（保存・書き戻し用）
            _loadedBmpData = bmpData;
            _loadedAppWizardData = imageData;
            OnPropertyChanged(nameof(CanSave));
            OnPropertyChanged(nameof(CanWrite));

            // BMPをプレビュー表示
            using var stream = new System.IO.MemoryStream(bmpData);
            var bitmap = new BitmapImage();
            bitmap.BeginInit();
            bitmap.StreamSource = stream;
            bitmap.CacheOption = BitmapCacheOption.OnLoad;
            bitmap.EndInit();
            bitmap.Freeze();

            PreviewImage = bitmap;
            ImageFilePath = "(Read from device)";
            StatusMessage = "Boot screen read complete - Can be saved as BMP";
            AppendLog("Read complete - Can save as BMP");
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "Read cancelled";
            AppendLog("Read cancelled");
        }
        catch (Exception ex)
        {
            StatusMessage = $"Error: {ex.Message}";
            AppendLog($"Error: {ex.Message}");

            await Application.Current.Dispatcher.InvokeAsync(() =>
                MessageBox.Show(
                    Application.Current.MainWindow,
                    $"Error reading boot screen.\n\n{ex.Message}",
                    "Error",
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
                StatusMessage = "Connection lost";
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
