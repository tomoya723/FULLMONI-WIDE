using System.Collections.ObjectModel;
using System.IO;
using System.Windows.Media.Imaging;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using FullmoniTerminal.Models;
using FullmoniTerminal.Services;

namespace FullmoniTerminal.ViewModels;

/// <summary>
/// ファームウェアカタログ用ViewModel
/// </summary>
public partial class FirmwareCatalogViewModel : ObservableObject, IDisposable
{
    private readonly OnlineUpdateService _onlineService;
    private readonly SerialPortService _serialService;
    private readonly FirmwareUpdateService _updateService;
    private CancellationTokenSource? _cts;
    private bool _disposed;

    /// <summary>
    /// 現在のリリースマニフェスト
    /// </summary>
    [ObservableProperty]
    private ReleaseManifest? _currentManifest;

    /// <summary>
    /// ファームウェアバリアント一覧
    /// </summary>
    public ObservableCollection<FirmwareVariantViewModel> Variants { get; } = new();

    /// <summary>
    /// 選択中のバリアント
    /// </summary>
    [ObservableProperty]
    private FirmwareVariantViewModel? _selectedVariant;

    partial void OnSelectedVariantChanged(FirmwareVariantViewModel? value)
    {
        // 選択状態を更新
        foreach (var v in Variants)
        {
            v.IsSelected = v == value;
        }
    }

    /// <summary>
    /// 読み込み中フラグ
    /// </summary>
    [ObservableProperty]
    private bool _isLoading;

    /// <summary>
    /// ダウンロード中フラグ
    /// </summary>
    [ObservableProperty]
    private bool _isDownloading;

    /// <summary>
    /// 進捗 (0-100)
    /// </summary>
    [ObservableProperty]
    private int _progress;

    /// <summary>
    /// ステータスメッセージ
    /// </summary>
    [ObservableProperty]
    private string _statusMessage = string.Empty;

    /// <summary>
    /// エラーメッセージ
    /// </summary>
    [ObservableProperty]
    private string? _errorMessage;

    /// <summary>
    /// リリースバージョン
    /// </summary>
    [ObservableProperty]
    private string _releaseVersion = string.Empty;

    /// <summary>
    /// リリース日
    /// </summary>
    [ObservableProperty]
    private string _releaseDate = string.Empty;

    /// <summary>
    /// 利用可能なリリース一覧
    /// </summary>
    public ObservableCollection<string> AvailableReleases { get; } = new();

    /// <summary>
    /// 選択中のリリースタグ
    /// </summary>
    [ObservableProperty]
    private string? _selectedRelease;

    /// <summary>
    /// SelectedRelease変更時の再帰呼び出し防止フラグ
    /// </summary>
    private bool _isUpdatingSelectedRelease;

    /// <summary>
    /// ダウンロード完了イベント（ファイルパスを返す）
    /// </summary>
    public event EventHandler<string>? DownloadCompleted;

    /// <summary>
    /// ダウンロード＆フラッシュ要求イベント（ファイルパスを返し、フラッシュ開始を要求）
    /// </summary>
    public event EventHandler<string>? FlashRequested;

    public FirmwareCatalogViewModel(SerialPortService serialService)
    {
        _serialService = serialService;
        _onlineService = new OnlineUpdateService();
        _updateService = new FirmwareUpdateService(serialService);

        // イベントハンドラ登録
        _onlineService.StatusChanged += (_, msg) => StatusMessage = msg;
        _onlineService.DownloadProgressChanged += (_, p) => Progress = p;
    }

    /// <summary>
    /// 最新リリースを読み込み
    /// </summary>
    [RelayCommand]
    public async Task LoadLatestReleaseAsync()
    {
        await LoadReleaseAsync(null);
    }

    /// <summary>
    /// 指定リリースを読み込み
    /// </summary>
    public async Task LoadReleaseAsync(string? tag)
    {
        System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] LoadReleaseAsync called: tag={tag}, IsLoading={IsLoading}");
        
        if (IsDownloading) return;
        
        // 既に読み込み中の場合はキャンセルして新しいリクエストを優先
        if (IsLoading)
        {
            _cts?.Cancel();
        }

        try
        {
            IsLoading = true;
            ErrorMessage = null;
            
            // 古いデータを即座にクリア
            CurrentManifest = null;
            ReleaseVersion = string.Empty;
            ReleaseDate = string.Empty;
            Variants.Clear();

            _cts?.Dispose();
            _cts = new CancellationTokenSource();

            // リリース一覧を取得（初回のみ）
            if (AvailableReleases.Count == 0)
            {
                var releases = await _onlineService.GetAvailableReleasesAsync(10, _cts.Token);
                AvailableReleases.Clear();
                foreach (var release in releases)
                {
                    AvailableReleases.Add(release);
                }
            }

            // マニフェストを取得
            ReleaseManifest? manifest;
            string targetTag;
            if (string.IsNullOrEmpty(tag))
            {
                // 最新リリースのタグを取得
                targetTag = AvailableReleases.Count > 0 ? AvailableReleases[0] : string.Empty;
                if (string.IsNullOrEmpty(targetTag))
                {
                    ErrorMessage = "No releases available";
                    return;
                }
                manifest = await _onlineService.GetManifestForTagAsync(targetTag, _cts.Token);
                // ドロップダウンで選択状態にする（マニフェスト取得後に設定）
            }
            else
            {
                targetTag = tag;
                System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] Loading manifest for tag: {tag}");
                manifest = await _onlineService.GetManifestForTagAsync(tag, _cts.Token);
            }

            if (manifest == null)
            {
                System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] Manifest is null for tag: {targetTag}");
                ErrorMessage = $"Release {targetTag} does not have firmware manifest";
                return;
            }

            CurrentManifest = manifest;
            ReleaseVersion = manifest.Version;
            ReleaseDate = manifest.ReleaseDate;
            
            // マニフェスト取得成功後にドロップダウンを更新（イベント発火を防ぐ）
            _isUpdatingSelectedRelease = true;
            SelectedRelease = targetTag;
            _isUpdatingSelectedRelease = false;

            // バリアントを追加
            if (manifest.Firmware?.Variants != null)
            {
                foreach (var variant in manifest.Firmware.Variants)
                {
                    var vm = new FirmwareVariantViewModel(variant);
                    Variants.Add(vm);

                    // サムネイルを非同期で読み込み
                    _ = LoadThumbnailAsync(vm, _cts.Token);
                }
            }

            if (Variants.Count > 0)
            {
                SelectedVariant = Variants[0];
            }

            StatusMessage = $"Found {Variants.Count} firmware variant(s)";
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "Operation cancelled";
        }
        catch (Exception ex)
        {
            ErrorMessage = ex.Message;
            StatusMessage = "Error loading release";
        }
        finally
        {
            IsLoading = false;
        }
    }

    /// <summary>
    /// サムネイルを非同期で読み込み
    /// </summary>
    private async Task LoadThumbnailAsync(FirmwareVariantViewModel vm, CancellationToken cancellationToken)
    {
        try
        {
            var imageData = await _onlineService.DownloadThumbnailAsync(vm.ThumbnailUrl, cancellationToken);
            if (imageData != null)
            {
                vm.ThumbnailImage = LoadImageFromBytes(imageData);
            }
        }
        catch
        {
            // サムネイル読み込み失敗は無視
        }
    }

    /// <summary>
    /// バイト配列から画像を読み込み
    /// </summary>
    private static BitmapImage? LoadImageFromBytes(byte[] data)
    {
        try
        {
            var image = new BitmapImage();
            using var ms = new MemoryStream(data);
            image.BeginInit();
            image.CacheOption = BitmapCacheOption.OnLoad;
            image.StreamSource = ms;
            image.EndInit();
            image.Freeze();
            return image;
        }
        catch
        {
            return null;
        }
    }

    /// <summary>
    /// 選択中のバリアントをダウンロード
    /// </summary>
    [RelayCommand]
    public async Task DownloadSelectedAsync()
    {
        if (SelectedVariant == null || IsDownloading) return;

        try
        {
            IsDownloading = true;
            Progress = 0;
            ErrorMessage = null;

            _cts?.Cancel();
            _cts = new CancellationTokenSource();

            // ダウンロード先を決定（一時フォルダ）
            var tempDir = Path.Combine(Path.GetTempPath(), "FULLMONI-WIDE", "Firmware");
            var fileName = SelectedVariant.Variant.File;
            var filePath = Path.Combine(tempDir, fileName);

            System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] Downloading to: {filePath}");
            System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] Source URL: {SelectedVariant.Variant.DownloadUrl}");

            var result = await _onlineService.DownloadFirmwareAsync(
                SelectedVariant.Variant,
                filePath,
                _cts.Token);

            if (result != null)
            {
                StatusMessage = $"✓ Download complete! Ready to flash.";
                System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] Download successful: {result}");
                DownloadCompleted?.Invoke(this, result);
            }
            else
            {
                // ダウンロード失敗の詳細を表示
                ErrorMessage = $"Download failed. Status: {StatusMessage}. Variant: {SelectedVariant.Variant.Name}, URL: {SelectedVariant.Variant.DownloadUrl ?? "(null)"}";
            }
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "Download cancelled";
        }
        catch (Exception ex)
        {
            ErrorMessage = $"Error: {ex.Message}";
            System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] Exception: {ex}");
        }
        finally
        {
            IsDownloading = false;
        }
    }

    /// <summary>
    /// ダウンロードしてすぐに書き込み
    /// </summary>
    [RelayCommand]
    public async Task DownloadAndFlashAsync()
    {
        if (SelectedVariant == null || IsDownloading) return;

        try
        {
            IsDownloading = true;
            Progress = 0;
            ErrorMessage = null;

            _cts?.Cancel();
            _cts = new CancellationTokenSource();

            // ダウンロード先を決定（一時フォルダ）
            var tempDir = Path.Combine(Path.GetTempPath(), "FULLMONI-WIDE", "Firmware");
            var fileName = SelectedVariant.Variant.File;
            var filePath = Path.Combine(tempDir, fileName);

            var result = await _onlineService.DownloadFirmwareAsync(
                SelectedVariant.Variant,
                filePath,
                _cts.Token);

            if (result != null)
            {
                StatusMessage = $"✓ Download complete! Starting flash...";
                // フラッシュ開始を要求
                FlashRequested?.Invoke(this, result);
            }
            else
            {
                ErrorMessage = $"Download failed. Status: {StatusMessage}";
            }
        }
        catch (OperationCanceledException)
        {
            StatusMessage = "Download cancelled";
        }
        catch (Exception ex)
        {
            ErrorMessage = $"Error: {ex.Message}";
        }
        finally
        {
            IsDownloading = false;
        }
    }

    /// <summary>
    /// 操作をキャンセル
    /// </summary>
    [RelayCommand]
    public void Cancel()
    {
        _cts?.Cancel();
    }

    partial void OnSelectedReleaseChanged(string? oldValue, string? newValue)
    {
        // 選択変更時の処理はMainWindow.xaml.csのReleaseComboBox_SelectionChangedで行う
        // ここでは何もしない（重複呼び出し防止）
        System.Diagnostics.Debug.WriteLine($"[FirmwareCatalog] OnSelectedReleaseChanged: old={oldValue}, new={newValue} (no action)");
    }

    public void Dispose()
    {
        if (!_disposed)
        {
            _cts?.Cancel();
            _cts?.Dispose();
            _onlineService.Dispose();
            _disposed = true;
        }
        GC.SuppressFinalize(this);
    }
}

/// <summary>
/// 個別のファームウェアバリアント用ViewModel
/// </summary>
public partial class FirmwareVariantViewModel : ObservableObject
{
    public FirmwareVariant Variant { get; }

    public string Id => Variant.Id;
    public string Name => Variant.Name;
    public string? Description => Variant.Description;
    public string? ThumbnailUrl => Variant.ThumbnailUrl;
    public string SizeDisplay => FormatSize(Variant.Size);

    [ObservableProperty]
    private BitmapImage? _thumbnailImage;

    /// <summary>
    /// 選択状態
    /// </summary>
    [ObservableProperty]
    private bool _isSelected;

    public FirmwareVariantViewModel(FirmwareVariant variant)
    {
        Variant = variant;
    }

    private static string FormatSize(long bytes)
    {
        if (bytes < 1024) return $"{bytes} B";
        if (bytes < 1024 * 1024) return $"{bytes / 1024.0:F1} KB";
        return $"{bytes / 1024.0 / 1024.0:F2} MB";
    }
}
