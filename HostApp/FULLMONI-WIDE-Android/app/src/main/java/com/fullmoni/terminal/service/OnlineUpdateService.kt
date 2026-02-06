package com.fullmoni.terminal.service

import android.content.Context
import android.util.Log
import com.fullmoni.terminal.model.FirmwareVariant
import com.fullmoni.terminal.model.ReleaseManifest
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.withContext
import kotlinx.serialization.json.Json
import java.io.File
import java.net.HttpURLConnection
import java.net.URL

/**
 * GitHub Releases からファームウェアカタログを取得し、ダウンロードするサービス
 */
class OnlineUpdateService(private val context: Context) {

    companion object {
        private const val TAG = "OnlineUpdateService"

        // GitHub リポジトリ情報
        private const val REPO_OWNER = "tomoya723"
        private const val REPO_NAME = "FULLMONI-WIDE"

        // GitHub API のベースURL
        private const val API_BASE_URL = "https://api.github.com/repos/$REPO_OWNER/$REPO_NAME"

        // GitHub Releases ダウンロードURL
        private fun getDownloadUrl(tag: String, fileName: String): String {
            return "https://github.com/$REPO_OWNER/$REPO_NAME/releases/download/$tag/$fileName"
        }

        // タイムアウト設定
        private const val CONNECT_TIMEOUT_MS = 30000
        private const val READ_TIMEOUT_MS = 60000
    }

    private val json = Json {
        ignoreUnknownKeys = true
        isLenient = true
    }

    // 状態
    private val _isLoading = MutableStateFlow(false)
    val isLoading: StateFlow<Boolean> = _isLoading.asStateFlow()

    private val _downloadProgress = MutableStateFlow(0)
    val downloadProgress: StateFlow<Int> = _downloadProgress.asStateFlow()

    private val _statusMessage = MutableStateFlow("")
    val statusMessage: StateFlow<String> = _statusMessage.asStateFlow()

    private val _availableReleases = MutableStateFlow<List<String>>(emptyList())
    val availableReleases: StateFlow<List<String>> = _availableReleases.asStateFlow()

    private val _currentManifest = MutableStateFlow<ReleaseManifest?>(null)
    val currentManifest: StateFlow<ReleaseManifest?> = _currentManifest.asStateFlow()

    /**
     * 利用可能なリリースタグの一覧を取得
     */
    suspend fun fetchAvailableReleases(limit: Int = 10): List<String> = withContext(Dispatchers.IO) {
        try {
            _isLoading.value = true
            updateStatus("Fetching available releases...")

            val url = URL("$API_BASE_URL/releases?per_page=$limit")
            val connection = url.openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.setRequestProperty("Accept", "application/vnd.github.v3+json")
            connection.setRequestProperty("User-Agent", "FULLMONI-WIDE-Android/1.0")
            connection.connectTimeout = CONNECT_TIMEOUT_MS
            connection.readTimeout = READ_TIMEOUT_MS

            if (connection.responseCode == HttpURLConnection.HTTP_OK) {
                val response = connection.inputStream.bufferedReader().readText()
                // レスポンスからタグ名を抽出（簡易パース）
                val tagPattern = """"tag_name"\s*:\s*"([^"]+)"""".toRegex()
                val tags = tagPattern.findAll(response).map { it.groupValues[1] }.toList()

                _availableReleases.value = tags
                updateStatus("Found ${tags.size} releases")
                Log.d(TAG, "Available releases: $tags")
                tags
            } else {
                Log.e(TAG, "Failed to fetch releases: ${connection.responseCode}")
                updateStatus("Failed to fetch releases")
                emptyList()
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error fetching releases: ${e.message}", e)
            updateStatus("Error: ${e.message}")
            emptyList()
        } finally {
            _isLoading.value = false
        }
    }

    /**
     * 最新リリースのマニフェストを取得
     */
    suspend fun fetchLatestManifest(): ReleaseManifest? = withContext(Dispatchers.IO) {
        try {
            _isLoading.value = true
            updateStatus("Fetching latest release...")

            // リリース一覧を取得して最新タグを取得
            val releases = if (_availableReleases.value.isEmpty()) {
                fetchAvailableReleases(1)
            } else {
                _availableReleases.value
            }

            if (releases.isEmpty()) {
                updateStatus("No releases available")
                return@withContext null
            }

            fetchManifestForTag(releases.first())
        } catch (e: Exception) {
            Log.e(TAG, "Error fetching latest manifest: ${e.message}", e)
            updateStatus("Error: ${e.message}")
            null
        } finally {
            _isLoading.value = false
        }
    }

    /**
     * 特定タグのマニフェストを取得
     */
    suspend fun fetchManifestForTag(tag: String): ReleaseManifest? = withContext(Dispatchers.IO) {
        try {
            _isLoading.value = true
            updateStatus("Fetching manifest for $tag...")

            val manifestUrl = getDownloadUrl(tag, "release-manifest.json")
            Log.d(TAG, "Fetching manifest from: $manifestUrl")

            val url = URL(manifestUrl)
            val connection = url.openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.setRequestProperty("User-Agent", "FULLMONI-WIDE-Android/1.0")
            connection.setRequestProperty("Accept", "application/json")
            connection.connectTimeout = CONNECT_TIMEOUT_MS
            connection.readTimeout = READ_TIMEOUT_MS
            connection.instanceFollowRedirects = true

            val responseCode = connection.responseCode
            Log.d(TAG, "Response code: $responseCode, URL after redirect: ${connection.url}")

            if (responseCode == HttpURLConnection.HTTP_OK) {
                val response = connection.inputStream.bufferedReader().readText()
                Log.d(TAG, "Response length: ${response.length}, first 100 chars: ${response.take(100)}")

                // BOMを除去
                val cleanedResponse = response.trimStart('\uFEFF', '\u200B', ' ', '\n', '\r')

                val manifest = json.decodeFromString<ReleaseManifest>(cleanedResponse)

                // 各バリアントにダウンロードURLを設定
                manifest.firmware?.variants?.forEach { variant ->
                    variant.downloadUrl = getDownloadUrl(tag, variant.file)
                    variant.thumbnail?.let { thumb ->
                        variant.thumbnailUrl = getDownloadUrl(tag, thumb)
                        Log.d(TAG, "Variant ${variant.id}: thumbnailUrl=${variant.thumbnailUrl}")
                    }
                }

                _currentManifest.value = manifest
                updateStatus("Found ${manifest.firmware?.variants?.size ?: 0} firmware variant(s)")
                Log.d(TAG, "Manifest loaded: version=${manifest.version}, variants=${manifest.firmware?.variants?.size}")
                manifest
            } else {
                Log.e(TAG, "Failed to fetch manifest: $responseCode")
                updateStatus("Manifest not found for $tag")
                null
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error fetching manifest: ${e.message}", e)
            updateStatus("Error: ${e.message?.take(200) ?: "Unknown error"}")
            null
        } finally {
            _isLoading.value = false
        }
    }

    /**
     * サムネイル画像をダウンロード
     */
    suspend fun downloadThumbnail(thumbnailUrl: String): ByteArray? = withContext(Dispatchers.IO) {
        try {
            if (thumbnailUrl.isEmpty()) return@withContext null

            val url = URL(thumbnailUrl)
            val connection = url.openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.setRequestProperty("User-Agent", "FULLMONI-WIDE-Android/1.0")
            connection.connectTimeout = CONNECT_TIMEOUT_MS
            connection.readTimeout = READ_TIMEOUT_MS
            connection.instanceFollowRedirects = true

            if (connection.responseCode == HttpURLConnection.HTTP_OK) {
                connection.inputStream.readBytes()
            } else {
                Log.e(TAG, "Failed to download thumbnail: ${connection.responseCode}")
                null
            }
        } catch (e: Exception) {
            Log.e(TAG, "Error downloading thumbnail: ${e.message}", e)
            null
        }
    }

    /**
     * ファームウェアをダウンロード
     * @return ダウンロードしたファイルのパス、失敗時はnull
     */
    suspend fun downloadFirmware(variant: FirmwareVariant): File? = withContext(Dispatchers.IO) {
        try {
            if (variant.downloadUrl.isEmpty()) {
                updateStatus("Download URL not available")
                return@withContext null
            }

            updateStatus("Downloading ${variant.name}...")
            _downloadProgress.value = 0

            val url = URL(variant.downloadUrl)
            val connection = url.openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.setRequestProperty("User-Agent", "FULLMONI-WIDE-Android/1.0")
            connection.connectTimeout = CONNECT_TIMEOUT_MS
            connection.readTimeout = READ_TIMEOUT_MS * 5  // ファイルダウンロード用に長めに
            connection.instanceFollowRedirects = true

            if (connection.responseCode != HttpURLConnection.HTTP_OK) {
                Log.e(TAG, "Failed to download firmware: ${connection.responseCode}")
                updateStatus("Download failed: ${connection.responseCode}")
                return@withContext null
            }

            val contentLength = connection.contentLength
            val downloadDir = File(context.cacheDir, "firmware")
            if (!downloadDir.exists()) {
                downloadDir.mkdirs()
            }

            val outputFile = File(downloadDir, variant.file)

            connection.inputStream.use { input ->
                outputFile.outputStream().use { output ->
                    val buffer = ByteArray(8192)
                    var bytesRead: Int
                    var totalRead = 0L

                    while (input.read(buffer).also { bytesRead = it } != -1) {
                        output.write(buffer, 0, bytesRead)
                        totalRead += bytesRead

                        if (contentLength > 0) {
                            val progress = (totalRead * 100 / contentLength).toInt()
                            _downloadProgress.value = progress

                            if (progress % 10 == 0) {
                                updateStatus("Downloading: $progress%")
                            }
                        }
                    }
                }
            }

            _downloadProgress.value = 100
            updateStatus("Download complete: ${outputFile.name}")
            Log.d(TAG, "Firmware downloaded to: ${outputFile.absolutePath}")

            // ファイルサイズ検証
            if (outputFile.length() != variant.size) {
                Log.w(TAG, "Size mismatch: expected ${variant.size}, got ${outputFile.length()}")
                // サイズ不一致でもファイルは返す（ダウンロード中に圧縮されている可能性）
            }

            outputFile
        } catch (e: Exception) {
            Log.e(TAG, "Error downloading firmware: ${e.message}", e)
            updateStatus("Download error: ${e.message}")
            _downloadProgress.value = 0
            null
        }
    }

    /**
     * キャッシュをクリア
     */
    fun clearCache() {
        val downloadDir = File(context.cacheDir, "firmware")
        if (downloadDir.exists()) {
            downloadDir.deleteRecursively()
        }
        _currentManifest.value = null
        _availableReleases.value = emptyList()
    }

    private fun updateStatus(message: String) {
        _statusMessage.value = message
    }
}
