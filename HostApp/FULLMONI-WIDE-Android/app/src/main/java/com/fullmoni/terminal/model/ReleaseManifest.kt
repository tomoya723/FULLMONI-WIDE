package com.fullmoni.terminal.model

import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable

/**
 * リリースマニフェスト全体
 * GitHub Releases の release-manifest.json をパースするためのデータクラス
 */
@Serializable
data class ReleaseManifest(
    @SerialName("schemaVersion")
    val schemaVersion: Int = 1,
    
    @SerialName("version")
    val version: String = "",
    
    @SerialName("releaseDate")
    val releaseDate: String = "",
    
    @SerialName("releaseNotes")
    val releaseNotes: String? = null,
    
    @SerialName("firmware")
    val firmware: FirmwareInfo? = null,
    
    @SerialName("hostApps")
    val hostApps: HostAppsInfo? = null
)

/**
 * ファームウェア情報
 */
@Serializable
data class FirmwareInfo(
    @SerialName("minimumBootloaderVersion")
    val minimumBootloaderVersion: String = "",
    
    @SerialName("variants")
    val variants: List<FirmwareVariant> = emptyList()
)

/**
 * ファームウェアバリアント（AppWizardリソース単位）
 */
@Serializable
data class FirmwareVariant(
    @SerialName("id")
    val id: String = "",
    
    @SerialName("name")
    val name: String = "",
    
    @SerialName("description")
    val description: String? = null,
    
    @SerialName("file")
    val file: String = "",
    
    @SerialName("thumbnail")
    val thumbnail: String? = null,
    
    @SerialName("size")
    val size: Long = 0,
    
    @SerialName("sha256")
    val sha256: String = ""
) {
    /**
     * ダウンロード用の完全URL（実行時に設定）
     */
    @kotlinx.serialization.Transient
    var downloadUrl: String = ""
    
    /**
     * サムネイルURL（実行時に設定）
     */
    @kotlinx.serialization.Transient
    var thumbnailUrl: String = ""
    
    /**
     * サイズを読みやすい形式で表示
     */
    val sizeFormatted: String
        get() {
            return when {
                size >= 1024 * 1024 -> String.format("%.2f MB", size / (1024.0 * 1024.0))
                size >= 1024 -> String.format("%.1f KB", size / 1024.0)
                else -> "$size bytes"
            }
        }
}

/**
 * ホストアプリ情報
 */
@Serializable
data class HostAppsInfo(
    @SerialName("android")
    val android: HostAppInfo? = null,
    
    @SerialName("windows")
    val windows: HostAppInfo? = null
)

/**
 * 個別ホストアプリ情報
 */
@Serializable
data class HostAppInfo(
    @SerialName("version")
    val version: String = "",
    
    @SerialName("file")
    val file: String? = null,
    
    @SerialName("size")
    val size: Long = 0,
    
    @SerialName("sha256")
    val sha256: String = ""
)
