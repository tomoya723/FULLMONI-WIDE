package com.fullmoni.terminal.service

import kotlinx.coroutines.*
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import java.io.InputStream

/**
 * ファームウェアアップデートサービス
 *
 * プロトコル:
 * 1. Parameter Consoleに接続
 * 2. fwupdateコマンド送信
 * 3. "yes"で確認
 * 4. デバイスリブート → USB再接続
 * 5. Bootloaderで 'U' コマンド送信 → Flash消去
 * 6. "Erase OK" 受信
 * 7. Size (4byte LE) 送信 → ACK '.' 待ち
 * 8. データストリーミング送信
 * 9. "Done!" 受信 → 自動リセット
 */
class FirmwareUpdateService(
    private val usbSerialService: UsbSerialService
) {
    companion object {
        private const val ACK_CHAR = '.'
        private const val CHUNK_SIZE = 4096  // 4KB chunks
        private const val ACK_TIMEOUT_MS = 10000L
        private const val ERASE_TIMEOUT_MS = 60000L
        private const val DONE_TIMEOUT_MS = 30000L
    }

    // 状態
    private val _progress = MutableStateFlow(0f)
    val progress: StateFlow<Float> = _progress

    private val _statusMessage = MutableStateFlow("")
    val statusMessage: StateFlow<String> = _statusMessage

    private val _logMessages = MutableStateFlow("")
    val logMessages: StateFlow<String> = _logMessages

    private val _isUpdating = MutableStateFlow(false)
    val isUpdating: StateFlow<Boolean> = _isUpdating

    // FW Update成功後、デバイスリブート中のUSBエラーを無視するためのフラグ
    private val _updateSucceeded = MutableStateFlow(false)
    val updateSucceeded: StateFlow<Boolean> = _updateSucceeded

    private var cancelled = false

    /**
     * ログをクリア
     */
    fun clearLog() {
        _logMessages.value = ""
        _updateSucceeded.value = false
    }

    /**
     * .binファイルからファームウェアデータを読み込み
     */
    fun loadFirmwareFromStream(inputStream: InputStream): ByteArray {
        return inputStream.readBytes()
    }

    /**
     * ファームウェア更新を開始
     *
     * @param firmwareData ファームウェアバイナリデータ
     * @param onComplete 完了時コールバック (成功/失敗)
     */
    suspend fun startUpdate(firmwareData: ByteArray, onComplete: (Boolean, String) -> Unit) {
        if (_isUpdating.value) {
            onComplete(false, "Already updating")
            return
        }

        _isUpdating.value = true
        _progress.value = 0f
        _updateSucceeded.value = false
        cancelled = false
        clearLog()

        try {
            log("=== Firmware Update Started ===")
            log("Firmware size: ${firmwareData.size} bytes")

            // Step 1: Parameter Consoleに入る
            updateStatus("Connecting to Parameter Console...")
            _progress.value = 1f
            usbSerialService.clearBuffer()
            usbSerialService.sendCommand("")
            delay(500)

            log("TX: (empty line)")

            // バナー確認
            if (!waitForResponse("Parameter Console", ">", timeoutMs = 5000)) {
                // Bootloaderモードかもしれない - 続行
                log("Warning: No Parameter Console banner, trying Bootloader...")
            } else {
                log("RX: Parameter Console detected")
            }

            // Step 2: fwupdateコマンド送信
            updateStatus("Sending fwupdate command...")
            _progress.value = 2f
            usbSerialService.clearBuffer()
            usbSerialService.sendCommand("fwupdate")
            log("TX: fwupdate")
            delay(500)

            // Step 3: 確認プロンプト待ち & yes送信
            if (waitForResponse("yes", "confirm", timeoutMs = 5000)) {
                log("RX: Confirmation prompt")
                usbSerialService.clearBuffer()
                usbSerialService.sendCommand("yes")
                log("TX: yes")
            }

            // Step 4: デバイスリブート & 再接続
            updateStatus("Device rebooting...")
            _progress.value = 4f
            log("Waiting for device reboot...")

            // デバイスがリブートするまで待機
            delay(2000)

            // Bootloaderデバイスへの自動再接続を試みる
            updateStatus("Reconnecting to Bootloader...")
            log("Attempting auto-reconnect to Bootloader...")

            val reconnected = usbSerialService.waitForDeviceAndConnect(timeoutMs = 15000, pollIntervalMs = 500)
            if (!reconnected) {
                throw Exception("Failed to reconnect to Bootloader - please check USB connection")
            }
            log("Reconnected to device")

            // 接続後、少し待機
            delay(1000)

            // Step 5: Bootloaderバナー待ち
            updateStatus("Waiting for Bootloader...")
            _progress.value = 5f
            usbSerialService.clearBuffer()

            // 改行送信してBootloaderのレスポンスを得る
            usbSerialService.sendCommand("")
            delay(500)

            val bootloaderFound = waitForResponse("Bootloader", "U)pdate", "Commands:", ">", timeoutMs = 10000)
            if (!bootloaderFound) {
                throw Exception("Bootloader not detected - please reconnect USB")
            }
            log("RX: Bootloader detected")

            // Step 6: 'U' コマンド送信 → Flash消去
            updateStatus("Erasing Flash...")
            _progress.value = 6f
            usbSerialService.clearBuffer()
            usbSerialService.sendCommand("U")
            log("TX: U (Update command)")

            // Erase完了待ち（最大60秒）
            val eraseStart = System.currentTimeMillis()
            var eraseOk = false
            while (System.currentTimeMillis() - eraseStart < ERASE_TIMEOUT_MS) {
                checkCancelled()

                val elapsed = System.currentTimeMillis() - eraseStart
                _progress.value = 6f + (elapsed.toFloat() / ERASE_TIMEOUT_MS * 9f).coerceAtMost(9f) // 6-15%

                val response = usbSerialService.receivedData.value
                if (response.contains("Erase OK") || response.contains("Send size")) {
                    log("RX: Erase OK")
                    eraseOk = true
                    break
                }
                if (response.contains("Fail")) {
                    throw Exception("Flash erase failed")
                }

                delay(100)
            }

            if (!eraseOk) {
                throw Exception("Flash erase timeout")
            }

            _progress.value = 15f

            // Step 7: サイズ送信
            updateStatus("Sending firmware size...")
            _progress.value = 17f
            usbSerialService.clearBuffer()

            val sizeBytes = ByteArray(4)
            sizeBytes[0] = (firmwareData.size and 0xFF).toByte()
            sizeBytes[1] = ((firmwareData.size shr 8) and 0xFF).toByte()
            sizeBytes[2] = ((firmwareData.size shr 16) and 0xFF).toByte()
            sizeBytes[3] = ((firmwareData.size shr 24) and 0xFF).toByte()

            usbSerialService.sendRaw(sizeBytes)
            log("TX: Size = ${firmwareData.size} bytes")

            // ACK待ち
            if (!waitForAck(ACK_TIMEOUT_MS)) {
                throw Exception("Size ACK timeout")
            }
            log("RX: ACK (.)")

            // Step 8: ファームウェア転送
            updateStatus("Transferring firmware...")
            _progress.value = 20f

            var sentBytes = 0
            val totalBytes = firmwareData.size
            val startTime = System.currentTimeMillis()

            while (sentBytes < totalBytes) {
                checkCancelled()

                val remaining = totalBytes - sentBytes
                val chunkSize = minOf(CHUNK_SIZE, remaining)
                val chunk = firmwareData.copyOfRange(sentBytes, sentBytes + chunkSize)

                usbSerialService.sendRaw(chunk)
                sentBytes += chunkSize

                // 進捗更新 (20% - 98%)
                val transferProgress = (sentBytes.toFloat() / totalBytes * 100f).toInt()
                _progress.value = 20f + (transferProgress * 0.78f)  // 20 + 78 = 98

                val elapsed = (System.currentTimeMillis() - startTime) / 1000.0
                val speedKBps = if (elapsed > 0) (sentBytes / 1024.0) / elapsed else 0.0
                updateStatus("Transferring: $sentBytes / $totalBytes bytes (${String.format("%.1f", speedKBps)} KB/s)")

                if (transferProgress % 10 == 0) {
                    log("TX: $sentBytes/$totalBytes bytes ($transferProgress%)")
                }

                // 少し待機してUIを更新＆USB安定化
                delay(1)
            }

            val totalTime = (System.currentTimeMillis() - startTime) / 1000.0
            val avgSpeed = if (totalTime > 0) (totalBytes / 1024.0) / totalTime else 0.0
            log("Transfer complete: $totalBytes bytes in ${String.format("%.1f", totalTime)}s (${String.format("%.1f", avgSpeed)} KB/s)")

            // Step 9: 完了メッセージ待ち
            updateStatus("Verifying...")
            _progress.value = 98f

            val doneReceived = waitForResponse("Done", timeoutMs = DONE_TIMEOUT_MS.toInt())
            if (doneReceived) {
                log("RX: Done! Update successful")
                _updateSucceeded.value = true  // 成功フラグを設定（USBエラー抑制用）

                // Step 10: Firmware再起動後に自動再接続
                updateStatus("Device restarting...")
                _progress.value = 99f
                log("Waiting for device to restart to Firmware...")

                delay(2000)  // デバイスがリブートするまで待機

                updateStatus("Reconnecting to Firmware...")
                log("Attempting auto-reconnect to Firmware...")

                val reconnected = usbSerialService.waitForDeviceAndConnect(timeoutMs = 15000, pollIntervalMs = 500)
                if (reconnected) {
                    log("Reconnected to Firmware successfully")
                    _progress.value = 100f
                    updateStatus("Update complete! Reconnected.")
                    onComplete(true, "Firmware update successful")
                } else {
                    log("Warning: Could not auto-reconnect, but update was successful")
                    _progress.value = 100f
                    updateStatus("Update complete! Please reconnect manually.")
                    onComplete(true, "Firmware update successful (reconnect manually)")
                }
            } else {
                val response = usbSerialService.receivedData.value
                if (response.contains("Write ERR")) {
                    throw Exception("Flash write error")
                }
                // 成功と仮定（Doneメッセージを見逃した可能性）
                log("Warning: Done message not received, assuming success")
                _updateSucceeded.value = true  // 成功フラグを設定（USBエラー抑制用）

                // 自動再接続を試みる
                updateStatus("Device restarting...")
                delay(2000)

                val reconnected = usbSerialService.waitForDeviceAndConnect(timeoutMs = 15000, pollIntervalMs = 500)
                if (reconnected) {
                    log("Reconnected to Firmware")
                }

                _progress.value = 100f
                updateStatus("Update possibly complete.")
                onComplete(true, "Firmware update possibly successful")
            }

        } catch (e: CancellationException) {
            log("Update cancelled")
            updateStatus("Update cancelled")
            onComplete(false, "Cancelled")
        } catch (e: Exception) {
            log("Error: ${e.message}")
            updateStatus("Error: ${e.message}")
            onComplete(false, e.message ?: "Unknown error")
        } finally {
            _isUpdating.value = false
        }
    }

    /**
     * 更新をキャンセル
     */
    fun cancel() {
        cancelled = true
    }

    private fun checkCancelled() {
        if (cancelled) {
            throw CancellationException("Update cancelled by user")
        }
    }

    private suspend fun waitForResponse(vararg keywords: String, timeoutMs: Int): Boolean {
        val start = System.currentTimeMillis()
        while (System.currentTimeMillis() - start < timeoutMs) {
            checkCancelled()
            val response = usbSerialService.receivedData.value
            for (keyword in keywords) {
                if (response.contains(keyword, ignoreCase = true)) {
                    return true
                }
            }
            delay(100)
        }
        return false
    }

    private suspend fun waitForAck(timeoutMs: Long): Boolean {
        val start = System.currentTimeMillis()
        while (System.currentTimeMillis() - start < timeoutMs) {
            checkCancelled()
            val response = usbSerialService.receivedData.value
            if (response.contains(ACK_CHAR)) {
                return true
            }
            delay(50)
        }
        return false
    }

    private fun updateStatus(message: String) {
        _statusMessage.value = message
    }

    private fun log(message: String) {
        val timestamp = java.text.SimpleDateFormat("HH:mm:ss", java.util.Locale.US).format(java.util.Date())
        _logMessages.value += "[$timestamp] $message\n"
    }
}
