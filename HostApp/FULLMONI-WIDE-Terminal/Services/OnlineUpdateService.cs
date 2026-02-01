using System.IO;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text.Json;
using FullmoniTerminal.Models;

namespace FullmoniTerminal.Services;

/// <summary>
/// GitHub Releases からファームウェアカタログを取得し、ダウンロードするサービス
/// </summary>
public class OnlineUpdateService : IDisposable
{
    private readonly HttpClient _httpClient;
    private bool _disposed;

    // GitHub リポジトリ情報
    private const string RepoOwner = "tomoya723";
    private const string RepoName = "FULLMONI-WIDE";

    // ローカルテストモード
    private static readonly bool UseLocalTest = false;  // GitHub Releasesから取得
    private static readonly string LocalTestPath = @"C:\Users\tomoy\git\FULLMONI-WIDE\test-release";

    /// <summary>
    /// GitHub Releases のベースURL
    /// </summary>
    public static string ReleasesBaseUrl => $"https://github.com/{RepoOwner}/{RepoName}/releases";

    /// <summary>
    /// GitHub API のベースURL
    /// </summary>
    private static string ApiBaseUrl => $"https://api.github.com/repos/{RepoOwner}/{RepoName}";

    /// <summary>
    /// ダウンロード進捗イベント (0-100%)
    /// </summary>
    public event EventHandler<int>? DownloadProgressChanged;

    /// <summary>
    /// ステータスメッセージイベント
    /// </summary>
    public event EventHandler<string>? StatusChanged;

    public OnlineUpdateService()
    {
        _httpClient = new HttpClient();
        _httpClient.DefaultRequestHeaders.Add("User-Agent", $"FULLMONI-WIDE-Terminal/1.0");
        _httpClient.Timeout = TimeSpan.FromMinutes(10); // 大きなファイル用に長めのタイムアウト
    }

    /// <summary>
    /// 最新リリースのマニフェストを取得
    /// </summary>
    public async Task<ReleaseManifest?> GetLatestManifestAsync(CancellationToken cancellationToken = default)
    {
        // ローカルテストモード
        if (UseLocalTest)
        {
            return await GetLocalManifestAsync(cancellationToken);
        }

        try
        {
            StatusChanged?.Invoke(this, "Checking for latest release...");

            // GitHub API でリリース一覧を取得（pre-releaseも含む）
            var apiUrl = $"{ApiBaseUrl}/releases?per_page=1";
            var response = await _httpClient.GetAsync(apiUrl, cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                StatusChanged?.Invoke(this, $"Failed to get release info: {response.StatusCode}");
                return null;
            }

            var json = await response.Content.ReadAsStringAsync(cancellationToken);
            using var doc = JsonDocument.Parse(json);
            var releases = doc.RootElement;

            if (releases.GetArrayLength() == 0)
            {
                StatusChanged?.Invoke(this, "No releases found");
                return null;
            }

            var tagName = releases[0].GetProperty("tag_name").GetString();
            if (string.IsNullOrEmpty(tagName))
            {
                StatusChanged?.Invoke(this, "No release tag found");
                return null;
            }

            // マニフェストをダウンロード
            return await GetManifestForTagAsync(tagName, cancellationToken);
        }
        catch (HttpRequestException ex)
        {
            StatusChanged?.Invoke(this, $"Network error: {ex.Message}");
            return null;
        }
        catch (Exception ex)
        {
            StatusChanged?.Invoke(this, $"Error: {ex.Message}");
            return null;
        }
    }

    /// <summary>
    /// ローカルテスト用: ローカルフォルダからマニフェストを読み込み
    /// </summary>
    private async Task<ReleaseManifest?> GetLocalManifestAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            StatusChanged?.Invoke(this, "[LOCAL TEST] Loading manifest from local folder...");

            var manifestPath = Path.Combine(LocalTestPath, "release-manifest.json");
            if (!File.Exists(manifestPath))
            {
                StatusChanged?.Invoke(this, $"Manifest not found: {manifestPath}");
                return null;
            }

            var json = await File.ReadAllTextAsync(manifestPath, cancellationToken);
            var manifest = JsonSerializer.Deserialize<ReleaseManifest>(json);

            if (manifest?.Firmware?.Variants != null)
            {
                // 各バリアントにローカルファイルパスを設定
                foreach (var variant in manifest.Firmware.Variants)
                {
                    variant.DownloadUrl = Path.Combine(LocalTestPath, variant.File);
                    if (!string.IsNullOrEmpty(variant.Thumbnail))
                    {
                        variant.ThumbnailUrl = Path.Combine(LocalTestPath, variant.Thumbnail);
                    }
                }
            }

            StatusChanged?.Invoke(this, $"[LOCAL TEST] Found {manifest?.Firmware?.Variants?.Count ?? 0} firmware variant(s)");
            return manifest;
        }
        catch (Exception ex)
        {
            StatusChanged?.Invoke(this, $"[LOCAL TEST] Error: {ex.Message}");
            return null;
        }
    }

    /// <summary>
    /// 特定タグのマニフェストを取得
    /// </summary>
    public async Task<ReleaseManifest?> GetManifestForTagAsync(string tag, CancellationToken cancellationToken = default)
    {
        // ローカルテストモードではローカルマニフェストを返す
        if (UseLocalTest)
        {
            return await GetLocalManifestAsync(cancellationToken);
        }

        try
        {
            StatusChanged?.Invoke(this, $"Fetching manifest for {tag}...");

            var manifestUrl = GetDownloadUrl(tag, "release-manifest.json");
            var response = await _httpClient.GetAsync(manifestUrl, cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                StatusChanged?.Invoke(this, $"Manifest not found for {tag}");
                return null;
            }

            var json = await response.Content.ReadAsStringAsync(cancellationToken);
            var manifest = JsonSerializer.Deserialize<ReleaseManifest>(json);

            if (manifest?.Firmware?.Variants != null)
            {
                // 各バリアントにダウンロードURLを設定
                foreach (var variant in manifest.Firmware.Variants)
                {
                    variant.DownloadUrl = GetDownloadUrl(tag, variant.File);
                    if (!string.IsNullOrEmpty(variant.Thumbnail))
                    {
                        variant.ThumbnailUrl = GetDownloadUrl(tag, variant.Thumbnail);
                    }
                }
            }

            StatusChanged?.Invoke(this, $"Found {manifest?.Firmware?.Variants?.Count ?? 0} firmware variant(s)");
            return manifest;
        }
        catch (JsonException ex)
        {
            StatusChanged?.Invoke(this, $"Invalid manifest format: {ex.Message}");
            return null;
        }
        catch (Exception ex)
        {
            StatusChanged?.Invoke(this, $"Error fetching manifest: {ex.Message}");
            return null;
        }
    }

    /// <summary>
    /// 利用可能なリリースタグの一覧を取得
    /// </summary>
    public async Task<List<string>> GetAvailableReleasesAsync(int limit = 10, CancellationToken cancellationToken = default)
    {
        var tags = new List<string>();

        try
        {
            var apiUrl = $"{ApiBaseUrl}/releases?per_page={limit}";
            var response = await _httpClient.GetAsync(apiUrl, cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                return tags;
            }

            var json = await response.Content.ReadAsStringAsync(cancellationToken);
            using var doc = JsonDocument.Parse(json);

            foreach (var release in doc.RootElement.EnumerateArray())
            {
                var tagName = release.GetProperty("tag_name").GetString();
                if (!string.IsNullOrEmpty(tagName))
                {
                    tags.Add(tagName);
                }
            }
        }
        catch
        {
            // エラー時は空リストを返す
        }

        return tags;
    }

    /// <summary>
    /// ファームウェアをダウンロード
    /// </summary>
    /// <param name="variant">ダウンロードするバリアント</param>
    /// <param name="destinationPath">保存先パス</param>
    /// <param name="cancellationToken">キャンセルトークン</param>
    /// <returns>ダウンロードしたファイルのパス、失敗時はnull</returns>
    public async Task<string?> DownloadFirmwareAsync(
        FirmwareVariant variant,
        string destinationPath,
        CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrEmpty(variant.DownloadUrl))
        {
            StatusChanged?.Invoke(this, "Download URL not available");
            return null;
        }

        // ローカルテストモード: ファイルをコピー
        if (UseLocalTest)
        {
            return await CopyLocalFirmwareAsync(variant, destinationPath, cancellationToken);
        }

        try
        {
            StatusChanged?.Invoke(this, $"Downloading {variant.Name}...");
            DownloadProgressChanged?.Invoke(this, 0);

            using var response = await _httpClient.GetAsync(
                variant.DownloadUrl,
                HttpCompletionOption.ResponseHeadersRead,
                cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                StatusChanged?.Invoke(this, $"Download failed: {response.StatusCode}");
                return null;
            }

            var totalBytes = response.Content.Headers.ContentLength ?? variant.Size;
            var buffer = new byte[81920]; // 80KB buffer
            long bytesRead = 0;
            int lastProgress = 0;

            // ディレクトリが存在しない場合は作成
            var directory = Path.GetDirectoryName(destinationPath);
            if (!string.IsNullOrEmpty(directory) && !Directory.Exists(directory))
            {
                Directory.CreateDirectory(directory);
            }

            await using var contentStream = await response.Content.ReadAsStreamAsync(cancellationToken);
            await using var fileStream = new FileStream(destinationPath, FileMode.Create, FileAccess.Write, FileShare.None, 81920, true);

            int read;
            while ((read = await contentStream.ReadAsync(buffer, cancellationToken)) > 0)
            {
                await fileStream.WriteAsync(buffer.AsMemory(0, read), cancellationToken);
                bytesRead += read;

                var progress = (int)(bytesRead * 100 / totalBytes);
                if (progress != lastProgress)
                {
                    lastProgress = progress;
                    DownloadProgressChanged?.Invoke(this, progress);
                }
            }

            DownloadProgressChanged?.Invoke(this, 100);

            // SHA256検証
            if (!string.IsNullOrEmpty(variant.Sha256))
            {
                StatusChanged?.Invoke(this, "Verifying checksum...");
                var actualHash = await ComputeSha256Async(destinationPath, cancellationToken);

                if (!string.Equals(actualHash, variant.Sha256, StringComparison.OrdinalIgnoreCase))
                {
                    StatusChanged?.Invoke(this, "Checksum verification failed!");
                    File.Delete(destinationPath);
                    return null;
                }
            }

            StatusChanged?.Invoke(this, $"Downloaded {variant.Name} successfully");
            return destinationPath;
        }
        catch (OperationCanceledException)
        {
            StatusChanged?.Invoke(this, "Download cancelled");
            if (File.Exists(destinationPath))
            {
                File.Delete(destinationPath);
            }
            return null;
        }
        catch (Exception ex)
        {
            StatusChanged?.Invoke(this, $"Download error: {ex.Message}");
            if (File.Exists(destinationPath))
            {
                File.Delete(destinationPath);
            }
            return null;
        }
    }

    /// <summary>
    /// サムネイル画像をダウンロード
    /// </summary>
    public async Task<byte[]?> DownloadThumbnailAsync(string? thumbnailUrl, CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrEmpty(thumbnailUrl))
        {
            return null;
        }

        // ローカルテストモード: ローカルファイルを読み込み
        if (UseLocalTest && File.Exists(thumbnailUrl))
        {
            try
            {
                return await File.ReadAllBytesAsync(thumbnailUrl, cancellationToken);
            }
            catch
            {
                return null;
            }
        }

        try
        {
            var response = await _httpClient.GetAsync(thumbnailUrl, cancellationToken);
            if (response.IsSuccessStatusCode)
            {
                return await response.Content.ReadAsByteArrayAsync(cancellationToken);
            }
        }
        catch
        {
            // サムネイル取得失敗は無視
        }

        return null;
    }

    /// <summary>
    /// ローカルテスト用: ローカルファイルをコピー
    /// </summary>
    private async Task<string?> CopyLocalFirmwareAsync(
        FirmwareVariant variant,
        string destinationPath,
        CancellationToken cancellationToken = default)
    {
        try
        {
            StatusChanged?.Invoke(this, $"[LOCAL TEST] Copying {variant.Name}...");
            System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] variant.Name={variant.Name}, variant.DownloadUrl={variant.DownloadUrl}");
            System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] destinationPath={destinationPath}");
            DownloadProgressChanged?.Invoke(this, 0);

            var sourcePath = variant.DownloadUrl!;
            System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] sourcePath={sourcePath}, exists={File.Exists(sourcePath)}");
            if (!File.Exists(sourcePath))
            {
                StatusChanged?.Invoke(this, $"[LOCAL TEST] File not found: {sourcePath}");
                return null;
            }

            // ディレクトリが存在しない場合は作成
            var directory = Path.GetDirectoryName(destinationPath);
            if (!string.IsNullOrEmpty(directory) && !Directory.Exists(directory))
            {
                Directory.CreateDirectory(directory);
            }

            // 既存ファイルがあれば削除
            if (File.Exists(destinationPath))
            {
                System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] Deleting existing file: {destinationPath}");
                File.Delete(destinationPath);
            }

            // ファイルをコピー（進捗表示付き）
            var sourceInfo = new FileInfo(sourcePath);
            var totalBytes = sourceInfo.Length;
            var buffer = new byte[81920];
            long bytesRead = 0;

            // ストリームをブロックスコープで管理してSHA256検証前に確実に閉じる
            {
                await using var sourceStream = new FileStream(sourcePath, FileMode.Open, FileAccess.Read, FileShare.Read, 81920, true);
                await using var destStream = new FileStream(destinationPath, FileMode.Create, FileAccess.Write, FileShare.None, 81920, true);

                int read;
                while ((read = await sourceStream.ReadAsync(buffer, cancellationToken)) > 0)
                {
                    await destStream.WriteAsync(buffer.AsMemory(0, read), cancellationToken);
                    bytesRead += read;
                    var progress = (int)(bytesRead * 100 / totalBytes);
                    DownloadProgressChanged?.Invoke(this, progress);
                }
            }

            DownloadProgressChanged?.Invoke(this, 100);

            // SHA256検証
            if (!string.IsNullOrEmpty(variant.Sha256))
            {
                StatusChanged?.Invoke(this, "[LOCAL TEST] Verifying checksum...");
                var actualHash = await ComputeSha256Async(destinationPath, cancellationToken);
                System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] Expected SHA256={variant.Sha256}");
                System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] Actual SHA256={actualHash}");

                if (!string.Equals(actualHash, variant.Sha256, StringComparison.OrdinalIgnoreCase))
                {
                    StatusChanged?.Invoke(this, $"[LOCAL TEST] Checksum verification failed! Expected={variant.Sha256}, Actual={actualHash}");
                    File.Delete(destinationPath);
                    return null;
                }
            }

            StatusChanged?.Invoke(this, $"[LOCAL TEST] Copied {variant.Name} successfully");
            return destinationPath;
        }
        catch (Exception ex)
        {
            StatusChanged?.Invoke(this, $"[LOCAL TEST] Error: {ex.GetType().Name}: {ex.Message}");
            System.Diagnostics.Debug.WriteLine($"[CopyLocalFirmware] Exception: {ex}");
            return null;
        }
    }

    /// <summary>
    /// ダウンロードURLを構築
    /// </summary>
    private static string GetDownloadUrl(string tag, string fileName)
    {
        return $"https://github.com/{RepoOwner}/{RepoName}/releases/download/{tag}/{fileName}";
    }

    /// <summary>
    /// ファイルのSHA256ハッシュを計算
    /// </summary>
    private static async Task<string> ComputeSha256Async(string filePath, CancellationToken cancellationToken = default)
    {
        await using var stream = new FileStream(filePath, FileMode.Open, FileAccess.Read, FileShare.Read, 81920, true);
        var hash = await SHA256.HashDataAsync(stream, cancellationToken);
        return Convert.ToHexString(hash).ToLowerInvariant();
    }

    public void Dispose()
    {
        if (!_disposed)
        {
            _httpClient.Dispose();
            _disposed = true;
        }
        GC.SuppressFinalize(this);
    }
}
