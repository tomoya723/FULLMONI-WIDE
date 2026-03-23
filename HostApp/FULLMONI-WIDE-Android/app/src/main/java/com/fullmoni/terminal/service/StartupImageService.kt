package com.fullmoni.terminal.service

import android.graphics.Bitmap
import android.util.Log
import kotlinx.coroutines.CancellationException
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.withContext
import java.nio.ByteBuffer
import java.nio.ByteOrder

/**
 * 起動画面書き込みサービス
 * RX72NのフラッシュメモリへUSB CDC経由でBMP画像を書き込む
 */
class StartupImageService(private val serialService: UsbSerialService) {

    companion object {
        private const val TAG = "StartupImageService"

        // フラッシュ書き込み設定
        private const val CHUNK_SIZE = 256  // バイト単位（FW側のプログラミング単位に合わせる）
        private const val SIZE_ACK_TIMEOUT_MS = 30000L  // サイズACK + 消去完了待ち
        private const val CHUNK_ACK_TIMEOUT_MS = 15000L  // チャンクACK待ち（実機通信遅延対応）
        private const val ACK_CHAR = 0x06.toChar()  // ASCII ACK制御文字

        // 読み込み設定
        private const val EXPECTED_IMAGE_SIZE = 391696  // 765x256 24-bit AppWizard形式
        private val END_MARKER = byteArrayOf(0xED.toByte(), 0x0F, 0xAA.toByte(), 0x55)
        private const val READ_TIMEOUT_MS = 30000L  // 読み込みタイムアウト
    }

    // 進捗 (0-100)
    private val _progress = MutableStateFlow(0f)
    val progress: StateFlow<Float> = _progress.asStateFlow()

    // ステータスメッセージ
    private val _statusMessage = MutableStateFlow("")
    val statusMessage: StateFlow<String> = _statusMessage.asStateFlow()

    // ログメッセージ
    private val _logMessages = MutableStateFlow("")
    val logMessages: StateFlow<String> = _logMessages.asStateFlow()

    // 書き込み中フラグ
    private val _isWriting = MutableStateFlow(false)
    val isWriting: StateFlow<Boolean> = _isWriting.asStateFlow()

    // 読み込み中フラグ
    private val _isReading = MutableStateFlow(false)
    val isReading: StateFlow<Boolean> = _isReading.asStateFlow()

    // 読み込んだ画像データ
    private val _readImageData = MutableStateFlow<ByteArray?>(null)
    val readImageData: StateFlow<ByteArray?> = _readImageData.asStateFlow()

    // キャンセルフラグ
    @Volatile
    private var isCancelled = false

    /**
     * Bitmapをフラッシュメモリに書き込む
     *
     * @param bitmap 765x256のBitmap
     * @param onComplete 完了コールバック (success, message)
     */
    suspend fun writeToFlash(bitmap: Bitmap, onComplete: (Boolean, String) -> Unit) {
        if (!serialService.isConnected.value) {
            onComplete(false, "シリアルポートが接続されていません")
            return
        }

        if (bitmap.width != AppWizardImageConverter.EXPECTED_WIDTH ||
            bitmap.height != AppWizardImageConverter.EXPECTED_HEIGHT) {
            onComplete(false, "画像サイズが不正です。${AppWizardImageConverter.EXPECTED_WIDTH}×${AppWizardImageConverter.EXPECTED_HEIGHT}が必要です")
            return
        }

        _isWriting.value = true
        isCancelled = false

        try {
            // 転送中のUSBエラーダイアログを抑制
            serialService.suppressUsbReadErrors = true

            // Bitmap → AppWizard形式変換
            log("画像をAppWizard形式に変換中...")
            updateStatus("画像を変換中...")
            _progress.value = 2f

            val imageData = AppWizardImageConverter.convertBitmapToAppWizard(bitmap)
            log("変換完了: ${imageData.size} bytes")

            // フラッシュ書き込み
            writeImageDataToFlash(imageData)

            onComplete(true, "起動画面の書き込みが完了しました")

        } catch (e: CancellationException) {
            log("キャンセルされました")
            updateStatus("書き込みがキャンセルされました")
            onComplete(false, "キャンセルされました")
        } catch (e: Exception) {
            Log.e(TAG, "Write error", e)
            log("エラー: ${e.message}")

            // USB切断の場合は自動再接続を試みる
            if (e.message?.contains("USB接続") == true || !serialService.isConnected.value) {
                // エラーメッセージをクリア（ダイアログが表示されないように）
                serialService.clearError()
                log("USB切断を検出、自動再接続を試行中...")
                updateStatus("USB切断を検出、再接続中...")
                val reconnected = serialService.waitForDeviceAndConnect(timeoutMs = 10000, pollIntervalMs = 500)
                if (reconnected) {
                    log("自動再接続成功")
                    updateStatus("再接続しました（転送は失敗）")
                } else {
                    log("自動再接続失敗")
                    updateStatus("再接続できませんでした")
                }
                // USB切断時はダイアログを表示しない（ログで通知済み）
                onComplete(false, "")
            } else {
                updateStatus("エラー: ${e.message}")
                onComplete(false, e.message ?: "不明なエラー")
            }
        } finally {
            serialService.suppressUsbReadErrors = false
            _isWriting.value = false
        }
    }

    /**
     * AppWizard形式データをフラッシュに書き込む
     */
    private suspend fun writeImageDataToFlash(imageData: ByteArray) = withContext(Dispatchers.IO) {
        log("=== 起動画面書き込み開始 ===")
        updateStatus("準備中...")
        _progress.value = 0f

        // Parameter Consoleに入る
        log("Parameter Consoleに接続中...")
        updateStatus("Parameter Consoleに接続中...")
        if (!serialService.sendCommand("")) {
            throw RuntimeException("コマンド送信失敗: 接続を確認してください")
        }
        delay(500)
        log("Parameter Console接続完了")

        checkCancelled()

        // imgwriteコマンドを送信
        log("TX: imgwrite")
        updateStatus("起動画面書き込みモードに切り替え中...")
        _progress.value = 5f
        serialService.clearBuffer()
        if (!serialService.sendCommand("imgwrite")) {
            throw RuntimeException("imgwriteコマンド送信失敗")
        }

        // FWが "Waiting for size" を出力するまで待つ
        // FWは受信バッファをクリアしてからサイズ待機に入るので、
        // この文字列が来る前にサイズを送ると捨てられる
        log("FWの準備完了待ち...")
        var waitCount = 0
        while (!serialService.containsInBuffer("Waiting for size") && waitCount < 100) {
            delay(100)
            waitCount++
        }
        if (waitCount >= 100) {
            log("Warning: 'Waiting for size' not received, proceeding anyway...")
        } else {
            log("FW準備完了")
        }
        delay(200)  // flush_rx完了を確実に待つ

        checkCancelled()

        // サイズ送信（4バイト、little-endian）
        val totalBytes = imageData.size
        val sizeBytes = ByteBuffer.allocate(4)
            .order(ByteOrder.LITTLE_ENDIAN)
            .putInt(totalBytes)
            .array()

        val sizeHex = sizeBytes.joinToString(" ") { "%02X".format(it) }
        log("TX: Size = $totalBytes bytes (0x${totalBytes.toString(16).uppercase()})")
        log("TX: Size bytes (hex) = [$sizeHex]")
        Log.d(TAG, "TX: Size = $totalBytes, bytes = [$sizeHex]")
        updateStatus("サイズ送信中: ${"%,d".format(totalBytes)} bytes")
        _progress.value = 10f

        // バッファクリアしてからサイズ送信
        serialService.clearBuffer()
        if (!serialService.sendRaw(sizeBytes)) {
            throw RuntimeException("サイズ送信失敗")
        }

        // ACKを待つ (サイズ確認)
        log("サイズACK待ち...")
        if (!serialService.waitForAck(SIZE_ACK_TIMEOUT_MS)) {
            throw RuntimeException("サイズACK待ちタイムアウト")
        }
        log("RX: ACK (サイズ確認)")

        checkCancelled()

        // フラッシュ消去完了のACKを待つ
        updateStatus("フラッシュ消去中... (約20秒かかります)")
        _progress.value = 12f
        log("フラッシュ消去ACK待ち...")
        if (!serialService.waitForAck(SIZE_ACK_TIMEOUT_MS)) {
            throw RuntimeException("フラッシュ消去完了待ちタイムアウト")
        }
        log("RX: ACK (フラッシュ消去完了)")

        checkCancelled()

        // データをチャンク単位で送信
        updateStatus("転送中: 0/${"%,d".format(totalBytes)} bytes")
        _progress.value = 15f
        var sentBytes = 0
        var lastProgressPercent = -1
        val startTime = System.currentTimeMillis()

        while (sentBytes < totalBytes) {
            checkCancelled()

            // USB接続チェック
            if (!serialService.isConnected.value) {
                throw RuntimeException("USB接続が切断されました")
            }

            // チャンクサイズ計算
            val remaining = totalBytes - sentBytes
            val chunkSize = minOf(CHUNK_SIZE, remaining)

            // チャンク送信
            val chunk = imageData.copyOfRange(sentBytes, sentBytes + chunkSize)
            if (!serialService.sendRaw(chunk)) {
                throw RuntimeException("チャンク送信失敗 (offset: $sentBytes)")
            }

            // チャンクごとにACK待ち
            if (!serialService.waitForAck(CHUNK_ACK_TIMEOUT_MS)) {
                throw RuntimeException("チャンクACK待ちタイムアウト (offset: $sentBytes)")
            }

            sentBytes += chunkSize

            // 進捗更新（15%-95%の範囲）
            val transferProgress = (sentBytes * 100) / totalBytes
            val overallProgress = 15 + (transferProgress * 80) / 100

            if (transferProgress != lastProgressPercent) {
                lastProgressPercent = transferProgress
                _progress.value = overallProgress.toFloat()

                val elapsedSec = (System.currentTimeMillis() - startTime) / 1000.0
                val speedKBps = if (elapsedSec > 0) (sentBytes / 1024.0) / elapsedSec else 0.0
                updateStatus("転送中: ${"%,d".format(sentBytes)}/${"%,d".format(totalBytes)} bytes ($transferProgress%) - ${"%.1f".format(speedKBps)} KB/s")

                if (transferProgress % 10 == 0) {
                    log("TX: ${"%,d".format(sentBytes)}/${"%,d".format(totalBytes)} bytes ($transferProgress%)")
                }
            }
        }

        val totalElapsedSec = (System.currentTimeMillis() - startTime) / 1000.0
        val avgSpeedKBps = if (totalElapsedSec > 0) (totalBytes / 1024.0) / totalElapsedSec else 0.0
        log("転送完了: ${"%,d".format(totalBytes)} bytes in ${"%.1f".format(totalElapsedSec)}s (${"%.1f".format(avgSpeedKBps)} KB/s)")

        // 完了確認
        updateStatus("書き込み完了を確認中...")
        _progress.value = 95f
        delay(2000)

        // 完了メッセージをチェック
        val bufferContainsDone = serialService.containsInBuffer("Done") || serialService.containsInBuffer("OK")
        val bufferContainsError = serialService.containsInBuffer("ERR") || serialService.containsInBuffer("Fail")

        if (bufferContainsError) {
            throw RuntimeException("書き込みエラーが発生しました")
        }

        if (bufferContainsDone) {
            log("RX: Done! 書き込み成功")
        } else {
            log("警告: 完了メッセージを受信できませんでしたが、転送は完了しました")
        }

        _progress.value = 98f
        updateStatus("デバイス再接続中...")

        // exitコマンドでParameter Consoleから抜ける（接続中の場合のみ）
        if (serialService.isConnected.value) {
            serialService.sendCommand("exit")
            delay(500)
        }

        // デバイスの再接続を試みる（FW側が再起動する場合に対応）
        log("デバイス再接続を試行中...")
        val reconnected = serialService.waitForDeviceAndConnect(timeoutMs = 10000, pollIntervalMs = 500)
        if (reconnected) {
            log("デバイス再接続成功")
        } else {
            log("警告: デバイス自動再接続できませんでしたが、書き込みは成功しています")
        }

        _progress.value = 100f
        updateStatus("起動画面の書き込みが完了しました")
    }

    /**
     * デバイスから起動画像を読み込む (imgread)
     *
     * @param onComplete 完了コールバック (success, imageData, message)
     */
    suspend fun readFromFlash(onComplete: (Boolean, ByteArray?, String) -> Unit) {
        if (!serialService.isConnected.value) {
            onComplete(false, null, "シリアルポートが接続されていません")
            return
        }

        _isReading.value = true
        isCancelled = false

        try {
            // USBエラー抑制
            serialService.suppressUsbReadErrors = true

            val imageData = readImageDataFromFlash()
            _readImageData.value = imageData
            onComplete(true, imageData, "起動画像の読み込みが完了しました")

        } catch (e: CancellationException) {
            log("キャンセルされました")
            updateStatus("読み込みがキャンセルされました")
            onComplete(false, null, "キャンセルされました")
        } catch (e: Exception) {
            Log.e(TAG, "Read error", e)
            log("エラー: ${e.message}")
            updateStatus("エラー: ${e.message}")
            onComplete(false, null, e.message ?: "不明なエラー")
        } finally {
            serialService.suppressUsbReadErrors = false
            serialService.resumeInputManager()  // SerialInputOutputManagerを再開（必要な場合）
            _isReading.value = false
        }
    }

    /**
     * フラッシュから画像データを読み込む
     */
    private suspend fun readImageDataFromFlash(): ByteArray = withContext(Dispatchers.IO) {
        log("=== 起動画像読み込み開始 ===")
        updateStatus("準備中...")
        _progress.value = 0f

        // Parameter Consoleに入る
        log("Parameter Consoleに接続中...")
        updateStatus("Parameter Consoleに接続中...")
        serialService.sendCommand("")
        delay(500)

        checkCancelled()

        // SerialInputOutputManagerを停止して直接読み取りモードに切り替え
        // ※imgreadコマンド送信前に停止しないとFWからの応答が吸い取られる
        serialService.pauseInputManager()
        serialService.clearBuffer()

        // imgreadコマンドを送信
        log("TX: imgread")
        updateStatus("起動画像読み込みモードに切り替え中...")
        _progress.value = 5f
        serialService.sendCommand("imgread")

        // FWがサイズ送信を開始するまで少し待つ
        delay(200)

        checkCancelled()

        // サイズ受信（391696バイトのパターンを探す）
        log("サイズ情報を受信中...")
        updateStatus("サイズ情報を受信中...")
        _progress.value = 8f

        val headerBuffer = ByteArray(64)
        var headerCount = 0
        var sizeOffset = -1
        var totalSize = 0
        var timeout = 5000L
        val startTime = System.currentTimeMillis()

        while (System.currentTimeMillis() - startTime < timeout) {
            checkCancelled()

            val readData = serialService.readRawDirect(64, 50)
            if (readData != null && readData.isNotEmpty()) {
                val toCopy = minOf(readData.size, headerBuffer.size - headerCount)
                System.arraycopy(readData, 0, headerBuffer, headerCount, toCopy)
                headerCount += toCopy
            }

            // サイズパターンを探す
            for (i in 0..headerCount - 4) {
                val testSize = (headerBuffer[i].toInt() and 0xFF) or
                        ((headerBuffer[i + 1].toInt() and 0xFF) shl 8) or
                        ((headerBuffer[i + 2].toInt() and 0xFF) shl 16) or
                        ((headerBuffer[i + 3].toInt() and 0xFF) shl 24)
                if (testSize == EXPECTED_IMAGE_SIZE) {
                    totalSize = testSize
                    sizeOffset = i
                    break
                }
            }

            if (sizeOffset >= 0) break
            delay(10)
        }

        val hexDump = headerBuffer.take(minOf(32, headerCount)).joinToString(" ") { "%02X".format(it) }
        log("RX header ($headerCount bytes): $hexDump")

        if (sizeOffset < 0) {
            throw RuntimeException("サイズパターンが見つかりません (受信: $headerCount bytes)")
        }

        log("RX: Size = $totalSize bytes")
        _progress.value = 10f

        checkCancelled()

        // データ + CRC + 終端マーカー受信
        val totalNeeded = totalSize + 2 + 4  // データ + CRC(2) + 終端マーカー(4)
        val receiveBuffer = mutableListOf<Byte>()

        // ヘッダーバッファに既にデータが含まれている場合
        val dataStartInHeader = sizeOffset + 4
        if (dataStartInHeader < headerCount) {
            for (i in dataStartInHeader until headerCount) {
                receiveBuffer.add(headerBuffer[i])
            }
            log("ヘッダーから ${headerCount - dataStartInHeader} bytes 取得済み")
        }

        var noDataCount = 0
        var lastProgressPercent = -1
        var endMarkerFound = false
        val receiveStartTime = System.currentTimeMillis()

        updateStatus("画像データ受信中: ${"%,d".format(totalSize)} bytes")

        // 終端マーカーを検出するまで受信を継続
        while (!endMarkerFound) {
            checkCancelled()

            val readData = serialService.readRawDirect(4096, 500)
            if (readData != null && readData.isNotEmpty()) {
                for (b in readData) {
                    receiveBuffer.add(b)
                }
                noDataCount = 0

                // 終端マーカーをチェック
                val minDataBeforeMarker = totalSize + 2
                if (receiveBuffer.size >= minDataBeforeMarker + 4) {
                    val lastIdx = receiveBuffer.size - 1
                    if (receiveBuffer[lastIdx - 3] == END_MARKER[0] &&
                        receiveBuffer[lastIdx - 2] == END_MARKER[1] &&
                        receiveBuffer[lastIdx - 1] == END_MARKER[2] &&
                        receiveBuffer[lastIdx] == END_MARKER[3]
                    ) {
                        log("終端マーカー検出! 総受信: ${receiveBuffer.size} bytes")
                        endMarkerFound = true
                    }
                }

                // 進捗表示
                val dataReceived = minOf(receiveBuffer.size, totalSize)
                val progress = (dataReceived * 100) / totalSize
                val overallProgress = 10 + (progress * 85) / 100

                if (progress / 10 != lastProgressPercent / 10) {
                    log("RX: ${"%,d".format(dataReceived)}/${"%,d".format(totalSize)} bytes ($progress%)")
                    lastProgressPercent = progress
                }
                _progress.value = overallProgress.toFloat()

                val elapsedSec = (System.currentTimeMillis() - receiveStartTime) / 1000.0
                val speedKBps = if (elapsedSec > 0) (dataReceived / 1024.0) / elapsedSec else 0.0
                updateStatus("受信中: ${"%,d".format(dataReceived)}/${"%,d".format(totalSize)} bytes ($progress%) - ${"%.1f".format(speedKBps)} KB/s")
            } else {
                noDataCount++
                // 既に十分なデータを受信していれば、終端マーカー待ちを延長
                val maxNoDataCount = if (receiveBuffer.size >= totalSize) 60 else 40  // 30秒 or 20秒
                if (noDataCount > maxNoDataCount) {
                    log("タイムアウト: 受信=${receiveBuffer.size}, 期待=$totalNeeded")
                    break
                }
            }

            // 明らかに過剰な受信をした場合は中断
            if (receiveBuffer.size > totalNeeded + 1024) {
                log("警告: 過剰受信 (${receiveBuffer.size} bytes), 処理を試みます")
                break
            }
        }

        val totalElapsedSec = (System.currentTimeMillis() - receiveStartTime) / 1000.0
        val avgSpeedKBps = if (totalElapsedSec > 0) (receiveBuffer.size / 1024.0) / totalElapsedSec else 0.0
        log("受信完了: ${"%,d".format(receiveBuffer.size)} bytes in ${"%.1f".format(totalElapsedSec)}s (${"%.1f".format(avgSpeedKBps)} KB/s)")

        // 受信データの検証
        if (receiveBuffer.size < totalSize + 2) {
            throw RuntimeException("受信データ不足: ${receiveBuffer.size}/${totalSize + 2} bytes")
        }

        // 終端マーカーを除去
        val imageData = receiveBuffer.toByteArray()
        var dataEndIndex = imageData.size
        if (endMarkerFound && imageData.size >= 4) {
            dataEndIndex -= 4
        }

        // CRC検証
        updateStatus("CRC検証中...")
        _progress.value = 95f

        if (dataEndIndex < totalSize + 2) {
            throw RuntimeException("CRCデータが不足: dataEndIndex=$dataEndIndex, required=${totalSize + 2}")
        }

        val receivedCrc = (imageData[totalSize].toInt() and 0xFF) or
                ((imageData[totalSize + 1].toInt() and 0xFF) shl 8)

        val calculatedCrc = calculateCrc16(imageData, 0, totalSize)

        if (receivedCrc != calculatedCrc) {
            log("CRCエラー: 受信=0x${receivedCrc.toString(16).uppercase()}, 計算=0x${calculatedCrc.toString(16).uppercase()}")
            throw RuntimeException("CRC検証失敗")
        }
        log("CRC OK: 0x${receivedCrc.toString(16).uppercase()}")

        _progress.value = 98f
        updateStatus("デバイス再接続中...")

        // exitコマンドでParameter Consoleから抜ける
        if (serialService.isConnected.value) {
            serialService.sendCommand("exit")
            delay(500)
        }

        // デバイスの再接続を試みる
        log("デバイス再接続を試行中...")
        val reconnected = serialService.waitForDeviceAndConnect(timeoutMs = 10000, pollIntervalMs = 500)
        if (reconnected) {
            log("デバイス再接続成功")
        } else {
            log("警告: デバイス自動再接続できませんでしたが、読み込みは成功しています")
        }

        _progress.value = 100f
        updateStatus("起動画像の読み込みが完了しました")

        // データ部分のみを返す（CRCと終端マーカーを除去）
        imageData.copyOf(totalSize)
    }

    /**
     * CRC-16/CCITT-FALSE 計算
     */
    private fun calculateCrc16(data: ByteArray, offset: Int, length: Int): Int {
        var crc = 0xFFFF
        for (i in offset until offset + length) {
            crc = crc xor ((data[i].toInt() and 0xFF) shl 8)
            for (j in 0 until 8) {
                crc = if ((crc and 0x8000) != 0) {
                    (crc shl 1) xor 0x1021
                } else {
                    crc shl 1
                }
            }
        }
        return crc and 0xFFFF
    }

    /**
     * 書き込みをキャンセル
     */
    fun cancel() {
        isCancelled = true
    }

    /**
     * ログをクリア
     */
    fun clearLog() {
        _logMessages.value = ""
    }

    private fun checkCancelled() {
        if (isCancelled) {
            throw CancellationException("ユーザーによるキャンセル")
        }
    }

    private fun log(message: String) {
        Log.d(TAG, message)
        _logMessages.value += "> $message\n"
    }

    private fun updateStatus(message: String) {
        _statusMessage.value = message
    }
}
