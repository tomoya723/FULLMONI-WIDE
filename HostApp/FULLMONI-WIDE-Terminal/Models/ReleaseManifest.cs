using System.Text.Json.Serialization;

namespace FullmoniTerminal.Models;

/// <summary>
/// リリースマニフェスト全体
/// </summary>
public class ReleaseManifest
{
    [JsonPropertyName("schemaVersion")]
    public int SchemaVersion { get; set; }

    [JsonPropertyName("version")]
    public string Version { get; set; } = string.Empty;

    [JsonPropertyName("releaseDate")]
    public string ReleaseDate { get; set; } = string.Empty;

    [JsonPropertyName("releaseNotes")]
    public string? ReleaseNotes { get; set; }

    [JsonPropertyName("firmware")]
    public FirmwareInfo? Firmware { get; set; }

    [JsonPropertyName("hostApps")]
    public HostAppsInfo? HostApps { get; set; }
}

/// <summary>
/// ファームウェア情報
/// </summary>
public class FirmwareInfo
{
    [JsonPropertyName("minimumBootloaderVersion")]
    public string MinimumBootloaderVersion { get; set; } = "1.0.0";

    [JsonPropertyName("variants")]
    public List<FirmwareVariant> Variants { get; set; } = new();
}

/// <summary>
/// ファームウェアバリアント（AppWizardリソース単位）
/// </summary>
public class FirmwareVariant
{
    [JsonPropertyName("id")]
    public string Id { get; set; } = string.Empty;

    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("description")]
    public string? Description { get; set; }

    [JsonPropertyName("file")]
    public string File { get; set; } = string.Empty;

    [JsonPropertyName("thumbnail")]
    public string? Thumbnail { get; set; }

    [JsonPropertyName("size")]
    public long Size { get; set; }

    [JsonPropertyName("sha256")]
    public string Sha256 { get; set; } = string.Empty;

    /// <summary>
    /// ダウンロード用の完全URL（実行時に設定）
    /// </summary>
    [JsonIgnore]
    public string? DownloadUrl { get; set; }

    /// <summary>
    /// サムネイル用の完全URL（実行時に設定）
    /// </summary>
    [JsonIgnore]
    public string? ThumbnailUrl { get; set; }
}

/// <summary>
/// ホストアプリケーション情報
/// </summary>
public class HostAppsInfo
{
    [JsonPropertyName("windows")]
    public HostAppInfo? Windows { get; set; }

    [JsonPropertyName("android")]
    public HostAppInfo? Android { get; set; }
}

/// <summary>
/// 個別のホストアプリ情報
/// </summary>
public class HostAppInfo
{
    [JsonPropertyName("version")]
    public string Version { get; set; } = string.Empty;

    [JsonPropertyName("file")]
    public string File { get; set; } = string.Empty;

    [JsonPropertyName("size")]
    public long Size { get; set; }

    [JsonPropertyName("minSdk")]
    public int? MinSdk { get; set; }

    [JsonPropertyName("sha256")]
    public string Sha256 { get; set; } = string.Empty;
}
