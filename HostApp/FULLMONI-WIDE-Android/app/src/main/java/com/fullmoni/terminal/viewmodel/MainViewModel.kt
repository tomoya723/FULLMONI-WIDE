package com.fullmoni.terminal.viewmodel

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.net.Uri
import android.os.Build
import android.widget.Toast
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import com.fullmoni.terminal.service.AppWizardImageConverter
import com.fullmoni.terminal.service.FirmwareUpdateService
import com.fullmoni.terminal.service.UsbSerialService
import com.hoho.android.usbserial.driver.UsbSerialDriver
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.combine
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.text.SimpleDateFormat
import java.util.*

/**
 * CAN チャンネル設定
 */
data class CanChannel(
    val number: Int,
    val id: String = "0x000",
    val enabled: Boolean = false
)

/**
 * CAN フィールド定義
 */
data class CanField(
    val enabled: Boolean = false,
    val channel: Int = 1,
    val startByte: Int = 0,
    val byteCount: Int = 1,
    val dataType: String = "U",
    val endian: String = "B",
    val targetVar: String = "",
    val offset: Float = 0f,
    val multiplier: Float = 1f,
    val divisor: Float = 1f,
    val decimals: Int = 0,
    val name: String = "",
    val unit: String = "",
    val warnLowEnabled: Boolean = false,
    val warnLow: Float = 0f,
    val warnHighEnabled: Boolean = false,
    val warnHigh: Float = 0f
)

/**
 * メイン画面のViewModel
 * Windows版と同等の機能を提供
 */
class MainViewModel(private val context: Context) : ViewModel() {

    companion object {
        private const val ACTION_USB_PERMISSION = "com.fullmoni.terminal.USB_PERMISSION"
    }

    private val usbSerialService = UsbSerialService(context)
    private val firmwareUpdateService = FirmwareUpdateService(usbSerialService)
    private val onlineUpdateService = com.fullmoni.terminal.service.OnlineUpdateService(context)
    private val startupImageService = com.fullmoni.terminal.service.StartupImageService(usbSerialService)

    // Connection
    val isConnected: StateFlow<Boolean> = usbSerialService.isConnected
    val receivedData: StateFlow<String> = usbSerialService.receivedData

    // FW Update中/成功後のUSBエラーを無視するフィルタリング済みerrorMessage
    val errorMessage: StateFlow<String?> = combine(
        usbSerialService.errorMessage,
        firmwareUpdateService.isUpdating,
        firmwareUpdateService.updateSucceeded
    ) { error, isUpdating, succeeded ->
        // FW Update中または成功後はUSBエラーを無視
        if (isUpdating || succeeded) null else error
    }.stateIn(viewModelScope, SharingStarted.WhileSubscribed(5000), null)

    private val _availableDrivers = MutableStateFlow<List<UsbSerialDriver>>(emptyList())
    val availableDrivers: StateFlow<List<UsbSerialDriver>> = _availableDrivers.asStateFlow()

    private val _selectedDriver = MutableStateFlow<UsbSerialDriver?>(null)
    val selectedDriver: StateFlow<UsbSerialDriver?> = _selectedDriver.asStateFlow()

    private val _firmwareVersion = MutableStateFlow<String?>(null)
    val firmwareVersion: StateFlow<String?> = _firmwareVersion.asStateFlow()

    private val _connectionPort = MutableStateFlow<String?>(null)
    val connectionPort: StateFlow<String?> = _connectionPort.asStateFlow()

    private val _isBootloaderMode = MutableStateFlow(false)
    val isBootloaderMode: StateFlow<Boolean> = _isBootloaderMode.asStateFlow()

    // Status
    private val _odoValue = MutableStateFlow("0")
    val odoValue: StateFlow<String> = _odoValue.asStateFlow()

    private val _tripValue = MutableStateFlow("0")
    val tripValue: StateFlow<String> = _tripValue.asStateFlow()

    private val _rtcValue = MutableStateFlow("---")
    val rtcValue: StateFlow<String> = _rtcValue.asStateFlow()

    // Settings - Tyre
    private val _tyreWidth = MutableStateFlow("")
    val tyreWidth: StateFlow<String> = _tyreWidth.asStateFlow()

    private val _tyreAspect = MutableStateFlow("")
    val tyreAspect: StateFlow<String> = _tyreAspect.asStateFlow()

    private val _wheelDia = MutableStateFlow("")
    val wheelDia: StateFlow<String> = _wheelDia.asStateFlow()

    // Settings - Gear
    private val _gear1 = MutableStateFlow("")
    val gear1: StateFlow<String> = _gear1.asStateFlow()

    private val _gear2 = MutableStateFlow("")
    val gear2: StateFlow<String> = _gear2.asStateFlow()

    private val _gear3 = MutableStateFlow("")
    val gear3: StateFlow<String> = _gear3.asStateFlow()

    private val _gear4 = MutableStateFlow("")
    val gear4: StateFlow<String> = _gear4.asStateFlow()

    private val _gear5 = MutableStateFlow("")
    val gear5: StateFlow<String> = _gear5.asStateFlow()

    private val _gear6 = MutableStateFlow("")
    val gear6: StateFlow<String> = _gear6.asStateFlow()

    private val _finalGear = MutableStateFlow("")
    val finalGear: StateFlow<String> = _finalGear.asStateFlow()

    private val _selectedGearPreset = MutableStateFlow("-- Select --")
    val selectedGearPreset: StateFlow<String> = _selectedGearPreset.asStateFlow()

    // Settings - Warning
    private val _waterTempLow = MutableStateFlow("")
    val waterTempLow: StateFlow<String> = _waterTempLow.asStateFlow()

    private val _waterTempHigh = MutableStateFlow("")
    val waterTempHigh: StateFlow<String> = _waterTempHigh.asStateFlow()

    private val _fuelWarn = MutableStateFlow("")
    val fuelWarn: StateFlow<String> = _fuelWarn.asStateFlow()

    // Settings - Shift RPM
    private val _shiftRpm1 = MutableStateFlow("")
    val shiftRpm1: StateFlow<String> = _shiftRpm1.asStateFlow()

    private val _shiftRpm2 = MutableStateFlow("")
    val shiftRpm2: StateFlow<String> = _shiftRpm2.asStateFlow()

    private val _shiftRpm3 = MutableStateFlow("")
    val shiftRpm3: StateFlow<String> = _shiftRpm3.asStateFlow()

    private val _shiftRpm4 = MutableStateFlow("")
    val shiftRpm4: StateFlow<String> = _shiftRpm4.asStateFlow()

    private val _shiftRpm5 = MutableStateFlow("")
    val shiftRpm5: StateFlow<String> = _shiftRpm5.asStateFlow()

    // CAN Config
    private val _canWarningEnabled = MutableStateFlow(false)
    val canWarningEnabled: StateFlow<Boolean> = _canWarningEnabled.asStateFlow()

    private val _canSoundEnabled = MutableStateFlow(false)
    val canSoundEnabled: StateFlow<Boolean> = _canSoundEnabled.asStateFlow()

    private val _canChannels = MutableStateFlow(List(6) { CanChannel(it + 1) })
    val canChannels: StateFlow<List<CanChannel>> = _canChannels.asStateFlow()

    private val _canFields = MutableStateFlow(List(16) { CanField() })
    val canFields: StateFlow<List<CanField>> = _canFields.asStateFlow()

    // Boot Screen
    private val _bootImagePath = MutableStateFlow("")
    val bootImagePath: StateFlow<String> = _bootImagePath.asStateFlow()

    private val _bootPreviewBitmap = MutableStateFlow<Bitmap?>(null)
    val bootPreviewBitmap: StateFlow<Bitmap?> = _bootPreviewBitmap.asStateFlow()

    // StartupImageServiceからStateFlowを公開
    val bootLog: StateFlow<String> = startupImageService.logMessages
    val isBootUploading: StateFlow<Boolean> = startupImageService.isWriting
    val isBootReading: StateFlow<Boolean> = startupImageService.isReading
    // サービスは0-100、UIは0-1を期待するので変換
    val bootUploadProgress: StateFlow<Float> = startupImageService.progress.let { flow ->
        kotlinx.coroutines.flow.MutableStateFlow(0f).also { mutable ->
            viewModelScope.launch {
                flow.collect { mutable.value = it / 100f }
            }
        }
    }
    val bootStatusMessage: StateFlow<String> = startupImageService.statusMessage

    // Firmware Update
    private val _selectedFirmwarePath = MutableStateFlow("")
    val selectedFirmwarePath: StateFlow<String> = _selectedFirmwarePath.asStateFlow()

    private var _selectedFirmwareUri: Uri? = null
    private var _downloadedFirmwareFile: java.io.File? = null

    val firmwareProgress: StateFlow<Float> = firmwareUpdateService.progress
    val firmwareProgressText: StateFlow<String> = firmwareUpdateService.statusMessage
    val firmwareLog: StateFlow<String> = firmwareUpdateService.logMessages
    val isFirmwareUpdating: StateFlow<Boolean> = firmwareUpdateService.isUpdating
    val firmwareUpdateSucceeded: StateFlow<Boolean> = firmwareUpdateService.updateSucceeded

    // Online Update
    val onlineReleases: StateFlow<List<String>> = onlineUpdateService.availableReleases
    val onlineManifest: StateFlow<com.fullmoni.terminal.model.ReleaseManifest?> = onlineUpdateService.currentManifest
    val onlineDownloadProgress: StateFlow<Int> = onlineUpdateService.downloadProgress
    val onlineStatusMessage: StateFlow<String> = onlineUpdateService.statusMessage
    val isOnlineLoading: StateFlow<Boolean> = onlineUpdateService.isLoading

    private val _selectedReleaseTag = MutableStateFlow("")
    val selectedReleaseTag: StateFlow<String> = _selectedReleaseTag.asStateFlow()

    // Validation
    private val _validationErrors = MutableStateFlow<List<String>>(emptyList())
    val validationErrors: StateFlow<List<String>> = _validationErrors.asStateFlow()

    private val _selectedVariant = MutableStateFlow<com.fullmoni.terminal.model.FirmwareVariant?>(null)
    val selectedVariant: StateFlow<com.fullmoni.terminal.model.FirmwareVariant?> = _selectedVariant.asStateFlow()

    private var pendingDriver: UsbSerialDriver? = null

    private val usbPermissionReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            if (ACTION_USB_PERMISSION == intent.action) {
                synchronized(this) {
                    val device: UsbDevice? = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                        intent.getParcelableExtra(UsbManager.EXTRA_DEVICE, UsbDevice::class.java)
                    } else {
                        @Suppress("DEPRECATION")
                        intent.getParcelableExtra(UsbManager.EXTRA_DEVICE)
                    }

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        pendingDriver?.let { driver ->
                            if (driver.device == device) {
                                usbSerialService.connect(driver)
                                onConnected(driver)
                            }
                        }
                    }
                    pendingDriver = null
                }
            }
        }
    }

    init {
        val filter = IntentFilter(ACTION_USB_PERMISSION)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            context.registerReceiver(usbPermissionReceiver, filter, Context.RECEIVER_NOT_EXPORTED)
        } else {
            context.registerReceiver(usbPermissionReceiver, filter)
        }
        refreshDevices()
    }

    override fun onCleared() {
        super.onCleared()
        try {
            context.unregisterReceiver(usbPermissionReceiver)
        } catch (e: Exception) { }
        usbSerialService.disconnect()
    }

    // ========== Connection ==========

    fun refreshDevices() {
        viewModelScope.launch {
            val drivers = usbSerialService.refreshDevices()
            _availableDrivers.value = drivers
            if (drivers.size == 1) {
                _selectedDriver.value = drivers[0]
            }
        }
    }

    fun selectDriver(driver: UsbSerialDriver) {
        _selectedDriver.value = driver
    }

    fun connect() {
        val driver = _selectedDriver.value ?: return
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager

        if (usbManager.hasPermission(driver.device)) {
            if (usbSerialService.connect(driver)) {
                onConnected(driver)
            }
        } else {
            pendingDriver = driver
            val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                PendingIntent.FLAG_MUTABLE
            } else { 0 }
            val permissionIntent = PendingIntent.getBroadcast(context, 0, Intent(ACTION_USB_PERMISSION), flags)
            usbManager.requestPermission(driver.device, permissionIntent)
        }
    }

    /**
     * シミュレーターモードで接続（エミュレーターでのテスト用）
     */
    fun connectSimulator() {
        if (usbSerialService.connectSimulator()) {
            _connectionPort.value = "Simulator"
            viewModelScope.launch {
                delay(300)
                // シミュレーターモードでは直接Firmwareモードとして動作
                _isBootloaderMode.value = false

                // 空文字送信でパラメータモードに入る
                sendCommand("")
                delay(500)

                // バージョン取得
                sendCommand("version")
                delay(500)
                parseVersionFromResponse()

                // パラメータ読み込み
                loadParameters()
            }
        }
    }

    /**
     * TCPブリッジモードで実機に接続（エミュレーターから実機テスト用）
     * 事前に以下を実行：
     * 1. PC側: .\tools\com_bridge.ps1 -ComPort COM19
     * 2. adb reverse tcp:9999 tcp:9999
     */
    fun connectTcpBridge() {
        viewModelScope.launch {
            val success = usbSerialService.connectTcpBridge()
            if (success) {
                _connectionPort.value = "TCP Bridge (Real Device)"
                delay(300)
                _isBootloaderMode.value = false

                // パラメータモードに入る
                sendCommand("")
                delay(500)

                // バージョン取得
                sendCommand("version")
                delay(500)
                parseVersionFromResponse()

                // パラメータ読み込み
                loadParameters()
            }
        }
    }

    /**
     * シミュレーションモードかどうか
     */
    val isSimulationMode: Boolean
        get() = usbSerialService.isSimulationMode

    /**
     * TCPブリッジモードかどうか
     */
    val isTcpBridgeMode: Boolean
        get() = usbSerialService.isTcpBridgeMode

    private fun onConnected(driver: UsbSerialDriver) {
        _connectionPort.value = driver.device.deviceName
        viewModelScope.launch {
            delay(500)
            // Bootloaderモード検出
            detectConnectionMode()
        }
    }

    /**
     * Bootloaderモードかどうかを検出
     */
    private suspend fun detectConnectionMode() {
        _isBootloaderMode.value = false

        // キー送信してメニューを表示させる
        sendCommand("\r")
        delay(800)

        val received = receivedData.value

        // Bootloaderの特徴的な文字列をチェック
        if (received.contains("Bootloader") ||
            received.contains("U=Update") ||
            received.contains("B=Boot") ||
            received.contains("S=Status")) {
            _isBootloaderMode.value = true
            return
        }

        // Firmwareとして接続 - パラメータ取得
        // バージョン取得
        sendCommand("version")
        delay(300)
        parseVersionFromResponse()
        // パラメータ読み込み
        loadParameters()
    }

    fun disconnect() {
        usbSerialService.disconnect()
        _connectionPort.value = null
        _firmwareVersion.value = null
    }

    fun sendCommand(command: String) {
        usbSerialService.sendCommand(command)
    }

    fun clearError() {
        usbSerialService.clearError()
    }

    // ========== Status ==========

    fun resetTrip() {
        viewModelScope.launch {
            sendCommand("set trip_reset 1")
            delay(200)
            sendCommand("save")
            delay(300)
            loadParameters()
        }
    }

    fun syncRtc() {
        viewModelScope.launch {
            // キー送信でパラメータモードに入る
            sendCommand("")
            delay(500)

            // Android時刻を取得してrtcコマンド発行 (フォーマット: yy MM dd HH mm ss)
            val sdf = SimpleDateFormat("yy MM dd HH mm ss", Locale.US)
            val dateStr = sdf.format(Date())
            sendCommand("rtc $dateStr")
            delay(300)

            // RTC値を再取得
            loadParameters()

            // exitコマンドで通信待ちを終了
            sendCommand("exit")
        }
    }

    // ========== Settings Updates ==========

    fun updateTyreWidth(value: String) { _tyreWidth.value = value }
    fun updateTyreAspect(value: String) { _tyreAspect.value = value }
    fun updateWheelDia(value: String) { _wheelDia.value = value }

    fun updateGear1(value: String) { _gear1.value = value }
    fun updateGear2(value: String) { _gear2.value = value }
    fun updateGear3(value: String) { _gear3.value = value }
    fun updateGear4(value: String) { _gear4.value = value }
    fun updateGear5(value: String) { _gear5.value = value }
    fun updateGear6(value: String) { _gear6.value = value }
    fun updateFinalGear(value: String) { _finalGear.value = value }

    fun selectGearPreset(preset: String) {
        _selectedGearPreset.value = preset
        when (preset) {
            "NA/NB 5-speed" -> {
                _gear1.value = "3.163"; _gear2.value = "1.888"; _gear3.value = "1.333"
                _gear4.value = "1.000"; _gear5.value = "0.814"; _gear6.value = "0"
            }
            "NB 6-speed" -> {
                _gear1.value = "3.760"; _gear2.value = "2.269"; _gear3.value = "1.645"
                _gear4.value = "1.257"; _gear5.value = "1.000"; _gear6.value = "0.843"
            }
        }
    }

    fun updateWaterTempLow(value: String) { _waterTempLow.value = value }
    fun updateWaterTempHigh(value: String) { _waterTempHigh.value = value }
    fun updateFuelWarn(value: String) { _fuelWarn.value = value }

    fun updateShiftRpm1(value: String) { _shiftRpm1.value = value }
    fun updateShiftRpm2(value: String) { _shiftRpm2.value = value }
    fun updateShiftRpm3(value: String) { _shiftRpm3.value = value }
    fun updateShiftRpm4(value: String) { _shiftRpm4.value = value }
    fun updateShiftRpm5(value: String) { _shiftRpm5.value = value }

    // ========== Parameters ==========

    fun loadParameters() {
        android.util.Log.d("MainViewModel", "loadParameters called")
        viewModelScope.launch {
            // キー送信でパラメータモードに入る（Windows版と同様）
            sendCommand("")
            delay(500)
            
            android.util.Log.d("MainViewModel", "loadParameters: clearing buffer")
            usbSerialService.clearBuffer()
            android.util.Log.d("MainViewModel", "loadParameters: sending 'list' command")
            sendCommand("list")
            android.util.Log.d("MainViewModel", "loadParameters: waiting 800ms")
            delay(800)  // シミュレーターの応答待ち時間を増やす
            android.util.Log.d("MainViewModel", "loadParameters: calling parseParametersFromResponse")
            parseParametersFromResponse()
            android.util.Log.d("MainViewModel", "loadParameters: done, tyreWidth=${_tyreWidth.value}")

            // rtcコマンドでRTC取得（Windows版と同様）
            usbSerialService.clearBuffer()
            sendCommand("rtc")
            delay(500)
            parseRtcFromResponse()

            // CAN設定読み込み（Windows版と同様）
            usbSerialService.clearBuffer()
            sendCommand("can_list")
            delay(2000)  // CAN設定は応答が長いので長めに待つ
            parseCanListResponse()
            android.util.Log.d("MainViewModel", "loadParameters: CAN config loaded")

            // exitコマンドでパラメータモードを抜ける（Windows版と同様）
            sendCommand("exit")
            delay(100)
        }
    }

    /**
     * ギア比を1000倍した整数値に変換して送信
     * ファームウェアは1000倍の整数値を期待している（例: 3.163 -> 3163）
     */
    private suspend fun sendGearRatioCommand(paramName: String, value: String) {
        if (value.isBlank()) return
        val ratio = value.toDoubleOrNull() ?: return
        val intValue = (ratio * 1000).toInt()
        sendCommand("set $paramName $intValue")
        delay(100)
    }

    fun saveParameters(): Boolean {
        val errors = validateAllParameters()
        _validationErrors.value = errors

        if (errors.isNotEmpty()) {
            return false
        }

        viewModelScope.launch {
            _tyreWidth.value.takeIf { it.isNotBlank() }?.let { sendCommand("set tyre_width $it"); delay(100) }
            _tyreAspect.value.takeIf { it.isNotBlank() }?.let { sendCommand("set tyre_aspect $it"); delay(100) }
            _wheelDia.value.takeIf { it.isNotBlank() }?.let { sendCommand("set tyre_rim $it"); delay(100) }
            // ギア比は1000倍の整数値で送信（ファームウェアがatoi()で整数パースするため）
            sendGearRatioCommand("gear1", _gear1.value)
            sendGearRatioCommand("gear2", _gear2.value)
            sendGearRatioCommand("gear3", _gear3.value)
            sendGearRatioCommand("gear4", _gear4.value)
            sendGearRatioCommand("gear5", _gear5.value)
            sendGearRatioCommand("gear6", _gear6.value)
            sendGearRatioCommand("final", _finalGear.value)
            _waterTempLow.value.takeIf { it.isNotBlank() }?.let { sendCommand("set water_low $it"); delay(100) }
            _waterTempHigh.value.takeIf { it.isNotBlank() }?.let { sendCommand("set water_high $it"); delay(100) }
            _fuelWarn.value.takeIf { it.isNotBlank() }?.let { sendCommand("set fuel_warn $it"); delay(100) }
            _shiftRpm1.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm1 $it"); delay(100) }
            _shiftRpm2.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm2 $it"); delay(100) }
            _shiftRpm3.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm3 $it"); delay(100) }
            _shiftRpm4.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm4 $it"); delay(100) }
            _shiftRpm5.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm5 $it"); delay(100) }
            sendCommand("save")
            delay(300)
            // exitコマンドでパラメータモードを終了
            sendCommand("exit")
            Toast.makeText(context, "Parameters saved", Toast.LENGTH_SHORT).show()
        }
        return true
    }

    fun clearValidationErrors() {
        _validationErrors.value = emptyList()
    }

    fun defaultParameters() {
        viewModelScope.launch {
            sendCommand("default")
            delay(500)
            loadParameters()
            Toast.makeText(context, "Parameters reset to default", Toast.LENGTH_SHORT).show()
        }
    }

    private fun parseVersionFromResponse() {
        val data = receivedData.value
        android.util.Log.d("MainViewModel", "parseVersion data: $data")
        // VERSION 2.0.0 or v2.0.0 format
        val versionMatch = Regex("""VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)|v([0-9]+\.[0-9]+\.[0-9]+)""").find(data)
        versionMatch?.let { match ->
            val version = match.groupValues[1].takeIf { it.isNotEmpty() } ?: match.groupValues[2]
            android.util.Log.d("MainViewModel", "Parsed version: $version")
            _firmwareVersion.value = version
        }
    }

    private fun parseParametersFromResponse() {
        val data = receivedData.value
        android.util.Log.d("MainViewModel", "parseParameters data length: ${data.length}")
        android.util.Log.d("MainViewModel", "parseParameters data: $data")

        // Parse firmware actual format:
        // Tyre: 195/50 R15
        val tyreRegex = Regex("""Tyre:\s*(\d+)/(\d+)\s*R(\d+)""")
        tyreRegex.find(data)?.let { match ->
            android.util.Log.d("MainViewModel", "Parsed tyre: ${match.groupValues}")
            _tyreWidth.value = match.groupValues[1]
            _tyreAspect.value = match.groupValues[2]
            _wheelDia.value = match.groupValues[3]
        }

        // Gear1: 3.760  Gear2: 2.269  Gear3: 1.645
        val gear1Regex = Regex("""Gear1:\s*([\d.]+)""")
        gear1Regex.find(data)?.let { _gear1.value = it.groupValues[1] }

        val gear2Regex = Regex("""Gear2:\s*([\d.]+)""")
        gear2Regex.find(data)?.let { _gear2.value = it.groupValues[1] }

        val gear3Regex = Regex("""Gear3:\s*([\d.]+)""")
        gear3Regex.find(data)?.let { _gear3.value = it.groupValues[1] }

        val gear4Regex = Regex("""Gear4:\s*([\d.]+)""")
        gear4Regex.find(data)?.let { _gear4.value = it.groupValues[1] }

        val gear5Regex = Regex("""Gear5:\s*([\d.]+)""")
        gear5Regex.find(data)?.let { _gear5.value = it.groupValues[1] }

        val gear6Regex = Regex("""Gear6:\s*([\d.]+)""")
        gear6Regex.find(data)?.let { _gear6.value = it.groupValues[1] }

        // Final: 4.300
        val finalRegex = Regex("""Final:\s*([\d.]+)""")
        finalRegex.find(data)?.let { _finalGear.value = it.groupValues[1] }

        // Water Temp Warning: 60 - 100 C
        val waterRegex = Regex("""Water Temp Warning:\s*(\d+)\s*-\s*(\d+)""")
        waterRegex.find(data)?.let { match ->
            _waterTempLow.value = match.groupValues[1]
            _waterTempHigh.value = match.groupValues[2]
        }

        // Fuel Warning: 10 %
        val fuelRegex = Regex("""Fuel Warning:\s*(\d+)""")
        fuelRegex.find(data)?.let { _fuelWarn.value = it.groupValues[1] }

        // Shift RPM: 5500 / 6000 / 6500 / 7000 / 7500
        val shiftRegex = Regex("""Shift RPM:\s*(\d+)\s*/\s*(\d+)\s*/\s*(\d+)\s*/\s*(\d+)\s*/\s*(\d+)""")
        shiftRegex.find(data)?.let { match ->
            _shiftRpm1.value = match.groupValues[1]
            _shiftRpm2.value = match.groupValues[2]
            _shiftRpm3.value = match.groupValues[3]
            _shiftRpm4.value = match.groupValues[4]
            _shiftRpm5.value = match.groupValues[5]
        }

        // ODO: 184692 km  TRIP: 114.7 km
        val odoRegex = Regex("""ODO:\s*([\d.]+)""")
        odoRegex.find(data)?.let { _odoValue.value = it.groupValues[1] }

        val tripRegex = Regex("""TRIP:\s*([\d.]+)""")
        tripRegex.find(data)?.let { _tripValue.value = it.groupValues[1] }
    }

    private fun parseRtcFromResponse() {
        val data = receivedData.value
        android.util.Log.d("MainViewModel", "parseRtc data: $data")

        // RTC出力形式: "RTC: 26/01/07 12:34:56" のような形式を想定
        val rtcRegex = Regex("""RTC:\s*(.+)""")
        rtcRegex.find(data)?.let { match ->
            _rtcValue.value = match.groupValues[1].trim()
            android.util.Log.d("MainViewModel", "Parsed RTC: ${_rtcValue.value}")
            return
        }

        // 別の形式: "20YY/MM/DD HH:MM:SS" or "YY/MM/DD HH:MM:SS"
        val dateRegex = Regex("""(\d{2,4}[/-]\d{2}[/-]\d{2}\s+\d{2}:\d{2}:\d{2})""")
        dateRegex.find(data)?.let { match ->
            _rtcValue.value = match.groupValues[1]
            android.util.Log.d("MainViewModel", "Parsed RTC (alt): ${_rtcValue.value}")
        }
    }

    // ========== CAN Config ==========

    fun updateCanWarningEnabled(enabled: Boolean) { _canWarningEnabled.value = enabled }
    fun updateCanSoundEnabled(enabled: Boolean) { _canSoundEnabled.value = enabled }

    fun updateCanChannelId(channelNum: Int, id: String) {
        _canChannels.value = _canChannels.value.map {
            if (it.number == channelNum) it.copy(id = id) else it
        }
    }

    fun updateCanChannelEnabled(channelNum: Int, enabled: Boolean) {
        _canChannels.value = _canChannels.value.map {
            if (it.number == channelNum) it.copy(enabled = enabled) else it
        }
    }

    fun updateCanField(index: Int, field: CanField) {
        _canFields.value = _canFields.value.toMutableList().also { it[index] = field }
    }

    fun loadCanConfig() {
        viewModelScope.launch {
            // パラメータモードに入る（単独呼び出し時のため）
            usbSerialService.clearBuffer()
            sendCommand("list")
            delay(500)

            usbSerialService.clearBuffer()
            sendCommand("can_list")
            delay(1500)
            parseCanListResponse()

            // パラメータモードを終了
            sendCommand("exit")
            delay(100)

            Toast.makeText(context, "CAN config loaded", Toast.LENGTH_SHORT).show()
        }
    }

    private fun parseCanListResponse() {
        val response = receivedData.value

        // Warning設定を解析: "Warning: ON, Sound: OFF"
        val warnSoundRegex = Regex("""Warning:\s*(ON|OFF),\s*Sound:\s*(ON|OFF)""", RegexOption.IGNORE_CASE)
        warnSoundRegex.find(response)?.let { match ->
            _canWarningEnabled.value = match.groupValues[1].equals("ON", ignoreCase = true)
            _canSoundEnabled.value = match.groupValues[2].equals("ON", ignoreCase = true)
        }

        // チャンネル設定を解析: "CH1: ID=0x3E8, ON"
        val chRegex = Regex("""CH(\d):\s*ID=0x([0-9A-Fa-f]+),\s*(ON|OFF)""", RegexOption.IGNORE_CASE)
        val updatedChannels = _canChannels.value.toMutableList()
        chRegex.findAll(response).forEach { match ->
            val chNum = match.groupValues[1].toInt()
            val canId = "0x${match.groupValues[2]}"
            val enabled = match.groupValues[3].equals("ON", ignoreCase = true)
            if (chNum in 1..6) {
                updatedChannels[chNum - 1] = CanChannel(chNum, canId, enabled)
            }
        }
        _canChannels.value = updatedChannels

        // フィールド定義を解析
        // Format: " 0  1   0   2   U    B   REV      0  1000  1000  0  Engine  rpm  N    ---  N    ---"
        val fieldRegex = Regex(
            """^\s*(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(U|S)\s+(B|L)\s+(\S+)\s+([-\d]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\S+)\s+(\S+)\s+(Y|N)\s+(\S+)\s+(Y|N)\s+(\S+)""",
            RegexOption.MULTILINE
        )
        val updatedFields = _canFields.value.toMutableList()
        fieldRegex.findAll(response).forEach { match ->
            val idx = match.groupValues[1].toInt()
            if (idx in 0..15) {
                val channel = match.groupValues[2].toInt()
                val name = match.groupValues[12].let { if (it == "-") "" else it }
                val unit = match.groupValues[13].let { if (it == "-") "" else it }
                updatedFields[idx] = CanField(
                    enabled = channel > 0,
                    channel = channel,
                    startByte = match.groupValues[3].toInt(),
                    byteCount = match.groupValues[4].toInt(),
                    dataType = match.groupValues[5].uppercase(),
                    endian = match.groupValues[6].uppercase(),
                    targetVar = match.groupValues[7],
                    offset = match.groupValues[8].toFloat(),
                    multiplier = match.groupValues[9].toFloat(),
                    divisor = match.groupValues[10].toFloat(),
                    decimals = match.groupValues[11].toInt(),
                    name = name,
                    unit = unit,
                    warnLowEnabled = match.groupValues[14].equals("Y", ignoreCase = true),
                    warnLow = match.groupValues[15].toFloatOrNull() ?: 0f,
                    warnHighEnabled = match.groupValues[16].equals("Y", ignoreCase = true),
                    warnHigh = match.groupValues[17].toFloatOrNull() ?: 0f
                )
            }
        }
        _canFields.value = updatedFields
    }

    fun saveCanConfig() {
        viewModelScope.launch {
            // マスターワーニング設定
            sendCommand("can_warning ${if (_canWarningEnabled.value) 1 else 0}")
            delay(100)
            sendCommand("can_sound ${if (_canSoundEnabled.value) 1 else 0}")
            delay(100)

            // チャンネル設定
            _canChannels.value.forEach { channel ->
                val canId = parseCanId(channel.id)
                val enabled = if (channel.enabled) 1 else 0
                sendCommand("can_ch ${channel.number} $canId $enabled")
                delay(100)
            }

            // フィールド設定
            _canFields.value.forEachIndexed { index, field ->
                val dataType = if (field.dataType == "S") 1 else 0
                val endian = if (field.endian == "L") 1 else 0
                val channel = if (field.enabled) field.channel else 0
                val targetVar = getTargetVarIndex(field.targetVar)
                val name = field.name.ifEmpty { "-" }
                val unit = field.unit.ifEmpty { "-" }
                val warnLoEnabled = if (field.warnLowEnabled) 1 else 0
                val warnHiEnabled = if (field.warnHighEnabled) 1 else 0

                // Field番号は0-based（0〜15）
                // warnLow/warnHighはfloatなので小数点以下も送信する
                sendCommand("can_field $index $channel ${field.startByte} ${field.byteCount} $dataType $endian $targetVar ${field.offset.toInt()} ${field.multiplier.toInt()} ${field.divisor.toInt()} $name $unit ${field.decimals} $warnLoEnabled ${field.warnLow} $warnHiEnabled ${field.warnHigh}")
                delay(150)
            }

            // 設定を保存
            sendCommand("can_save")
            delay(200)
            sendCommand("exit")

            Toast.makeText(context, "CAN config saved", Toast.LENGTH_SHORT).show()
        }
    }

    private fun parseCanId(text: String): Int {
        val trimmed = text.trim()
        return if (trimmed.startsWith("0x", ignoreCase = true)) {
            trimmed.substring(2).toInt(16)
        } else {
            trimmed.toIntOrNull(16) ?: 0
        }
    }

    private fun getTargetVarIndex(varName: String): Int {
        return when (varName.uppercase()) {
            "REV" -> 0
            "AF" -> 1
            "NUM1" -> 2
            "NUM2" -> 3
            "NUM3" -> 4
            "NUM4" -> 5
            "NUM5" -> 6
            "NUM6" -> 7
            "SPEED" -> 8
            "FUEL" -> 9
            else -> {
                if (varName.startsWith("USER", ignoreCase = true)) {
                    val num = varName.substring(4).toIntOrNull() ?: 1
                    9 + num // USER1=10, USER2=11, ...
                } else 10
            }
        }
    }

    fun defaultCanConfig() {
        viewModelScope.launch {
            usbSerialService.clearBuffer()
            sendCommand("list")  // Enter parameter mode
            delay(100)

            sendCommand("can_preset motec")  // Apply MoTeC M100 preset (default)
            delay(500)

            // Read back the config
            usbSerialService.clearBuffer()
            sendCommand("can_list")
            delay(1500)
            parseCanListResponse()

            // Exit parameter mode
            sendCommand("exit")
            delay(100)

            Toast.makeText(context, "CAN config reset to default (MoTeC M100)", Toast.LENGTH_SHORT).show()
        }
    }

    // ========== Boot Screen ==========

    fun selectBootImage(context: Context, uri: Uri) {
        viewModelScope.launch {
            try {
                val inputStream = context.contentResolver.openInputStream(uri)
                val bitmap = BitmapFactory.decodeStream(inputStream)
                inputStream?.close()

                if (bitmap != null) {
                    _bootPreviewBitmap.value = bitmap
                    // ContentResolverから正しいファイル名を取得
                    val fileName = context.contentResolver.query(uri, null, null, null, null)?.use { cursor ->
                        if (cursor.moveToFirst()) {
                            val nameIndex = cursor.getColumnIndex(android.provider.OpenableColumns.DISPLAY_NAME)
                            if (nameIndex >= 0) cursor.getString(nameIndex) else null
                        } else null
                    } ?: uri.lastPathSegment ?: "image.bmp"
                    _bootImagePath.value = fileName

                    // Log through StartupImageService or Toast
                    if (bitmap.width != com.fullmoni.terminal.service.AppWizardImageConverter.EXPECTED_WIDTH ||
                        bitmap.height != com.fullmoni.terminal.service.AppWizardImageConverter.EXPECTED_HEIGHT) {
                        Toast.makeText(context,
                            "警告: 画像サイズが${bitmap.width}×${bitmap.height}です。765×256が必要です。",
                            Toast.LENGTH_LONG).show()
                    } else {
                        Toast.makeText(context, "画像読み込み完了: ${bitmap.width}×${bitmap.height}", Toast.LENGTH_SHORT).show()
                    }
                }
            } catch (e: Exception) {
                Toast.makeText(context, "画像読み込みエラー: ${e.message}", Toast.LENGTH_SHORT).show()
            }
        }
    }

    fun readBootImageFromDevice() {
        viewModelScope.launch {
            startupImageService.readFromFlash { success, imageData, message ->
                if (success && imageData != null) {
                    try {
                        // AppWizard形式からBitmapに変換
                        val bitmap = AppWizardImageConverter.convertAppWizardToBitmap(imageData)
                        _bootPreviewBitmap.value = bitmap
                        Toast.makeText(context, "読み込み完了", Toast.LENGTH_SHORT).show()
                    } catch (e: Exception) {
                        Toast.makeText(context, "変換エラー: ${e.message}", Toast.LENGTH_SHORT).show()
                    }
                } else if (message.isNotEmpty()) {
                    Toast.makeText(context, message, Toast.LENGTH_SHORT).show()
                }
            }
        }
    }

    fun saveBootImageToUri(context: Context, uri: Uri) {
        val bitmap = _bootPreviewBitmap.value
        if (bitmap == null) {
            Toast.makeText(context, "No image to save", Toast.LENGTH_SHORT).show()
            return
        }

        viewModelScope.launch(Dispatchers.IO) {
            try {
                context.contentResolver.openOutputStream(uri)?.use { outputStream ->
                    bitmap.compress(android.graphics.Bitmap.CompressFormat.PNG, 100, outputStream)
                }
                withContext(Dispatchers.Main) {
                    Toast.makeText(context, "Image saved successfully", Toast.LENGTH_SHORT).show()
                }
            } catch (e: Exception) {
                withContext(Dispatchers.Main) {
                    Toast.makeText(context, "Failed to save: ${e.message}", Toast.LENGTH_LONG).show()
                }
            }
        }
    }

    fun uploadBootImage() {
        val bitmap = _bootPreviewBitmap.value
        if (bitmap == null) {
            Toast.makeText(context, "Please select an image first", Toast.LENGTH_SHORT).show()
            return
        }

        if (bitmap.width != com.fullmoni.terminal.service.AppWizardImageConverter.EXPECTED_WIDTH ||
            bitmap.height != com.fullmoni.terminal.service.AppWizardImageConverter.EXPECTED_HEIGHT) {
            Toast.makeText(context,
                "画像サイズが不正です。765×256が必要です。",
                Toast.LENGTH_LONG).show()
            return
        }

        viewModelScope.launch {
            startupImageService.writeToFlash(bitmap) { success, message ->
                viewModelScope.launch {
                    if (success) {
                        Toast.makeText(context, message, Toast.LENGTH_LONG).show()
                    } else {
                        Toast.makeText(context, "Error: $message", Toast.LENGTH_LONG).show()
                    }
                }
            }
        }
    }

    fun cancelBootUpload() {
        startupImageService.cancel()
    }

    fun clearBootLog() {
        startupImageService.clearLog()
    }

    fun copyBootLog(context: Context) {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        clipboard.setPrimaryClip(ClipData.newPlainText("Boot Log", bootLog.value))
        Toast.makeText(context, "Log copied", Toast.LENGTH_SHORT).show()
    }

    // ========== Firmware Update ==========

    fun selectFirmwareFile(context: Context, uri: Uri) {
        _selectedFirmwarePath.value = uri.lastPathSegment ?: "firmware.bin"
        _selectedFirmwareUri = uri
    }

    fun startFirmwareUpdate(context: Context) {
        val uri = _selectedFirmwareUri
        if (uri == null) {
            Toast.makeText(context, "Please select a firmware file first", Toast.LENGTH_SHORT).show()
            return
        }

        viewModelScope.launch {
            try {
                // ファームウェアファイルを読み込み
                val inputStream = context.contentResolver.openInputStream(uri)
                if (inputStream == null) {
                    Toast.makeText(context, "Cannot open firmware file", Toast.LENGTH_SHORT).show()
                    return@launch
                }

                val firmwareData = firmwareUpdateService.loadFirmwareFromStream(inputStream)
                inputStream.close()

                // ファームウェア更新開始
                firmwareUpdateService.startUpdate(firmwareData) { success, message ->
                    viewModelScope.launch {
                        if (success) {
                            Toast.makeText(context, "Firmware update successful!", Toast.LENGTH_LONG).show()
                        } else {
                            Toast.makeText(context, "Update failed: $message", Toast.LENGTH_LONG).show()
                        }
                    }
                }
            } catch (e: Exception) {
                Toast.makeText(context, "Error: ${e.message}", Toast.LENGTH_SHORT).show()
            }
        }
    }

    fun cancelFirmwareUpdate() {
        firmwareUpdateService.cancel()
    }

    fun clearFirmwareLog() {
        firmwareUpdateService.clearLog()
    }

    fun copyFirmwareLog(context: Context) {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        clipboard.setPrimaryClip(ClipData.newPlainText("Firmware Log", firmwareLog.value))
        Toast.makeText(context, "Log copied", Toast.LENGTH_SHORT).show()
    }

    // ========== Online Update ==========

    /**
     * 利用可能なリリースを取得
     */
    fun fetchOnlineReleases() {
        viewModelScope.launch {
            onlineUpdateService.fetchAvailableReleases()
        }
    }

    /**
     * リリースタグを選択してマニフェストを取得
     */
    fun selectReleaseTag(tag: String) {
        _selectedReleaseTag.value = tag
        _selectedVariant.value = null
        viewModelScope.launch {
            onlineUpdateService.fetchManifestForTag(tag)
        }
    }

    /**
     * ファームウェアバリアントを選択
     */
    fun selectFirmwareVariant(variant: com.fullmoni.terminal.model.FirmwareVariant) {
        _selectedVariant.value = variant
        _selectedFirmwarePath.value = variant.file
        _selectedFirmwareUri = null
        _downloadedFirmwareFile = null
    }

    /**
     * 選択したバリアントをダウンロードしてFW Updateを開始
     */
    fun downloadAndStartFirmwareUpdate(context: Context) {
        val variant = _selectedVariant.value
        if (variant == null) {
            Toast.makeText(context, "Please select a firmware variant first", Toast.LENGTH_SHORT).show()
            return
        }

        viewModelScope.launch {
            try {
                // ダウンロード
                val downloadedFile = onlineUpdateService.downloadFirmware(variant)
                if (downloadedFile == null) {
                    Toast.makeText(context, "Download failed", Toast.LENGTH_SHORT).show()
                    return@launch
                }

                _downloadedFirmwareFile = downloadedFile

                // ファームウェアファイルを読み込み
                val firmwareData = downloadedFile.inputStream().use { inputStream ->
                    firmwareUpdateService.loadFirmwareFromStream(inputStream)
                }

                // ファームウェア更新開始
                firmwareUpdateService.startUpdate(firmwareData) { success, message ->
                    viewModelScope.launch {
                        if (success) {
                            Toast.makeText(context, "Firmware update successful!", Toast.LENGTH_LONG).show()
                        } else {
                            Toast.makeText(context, "Update failed: $message", Toast.LENGTH_LONG).show()
                        }
                    }
                }
            } catch (e: Exception) {
                Toast.makeText(context, "Error: ${e.message}", Toast.LENGTH_SHORT).show()
            }
        }
    }

    /**
     * 最新リリースを取得
     */
    fun fetchLatestRelease() {
        viewModelScope.launch {
            val manifest = onlineUpdateService.fetchLatestManifest()
            if (manifest != null) {
                val releases = onlineUpdateService.availableReleases.value
                if (releases.isNotEmpty()) {
                    _selectedReleaseTag.value = releases.first()
                }
            }
        }
    }

    // ========== Validation ==========

    private fun validateAllParameters(): List<String> {
        val errors = mutableListOf<String>()

        // === タイヤ設定バリデーション ===
        if (!validatePositiveInt(_tyreWidth.value, 1, 500))
            errors.add("【タイヤ幅】正の整数で1～500の範囲で入力してください")
        if (!validatePositiveInt(_tyreAspect.value, 1, 500))
            errors.add("【扁平率】正の整数で1～500の範囲で入力してください")
        if (!validatePositiveInt(_wheelDia.value, 1, 500))
            errors.add("【リム径】正の整数で1～500の範囲で入力してください")

        // === 警告設定バリデーション ===
        val waterLow = _waterTempLow.value.toIntOrNull()
        val waterHigh = _waterTempHigh.value.toIntOrNull()
        if (!validateInt(_waterTempLow.value, -40, 200))
            errors.add("【水温低温】-40～200の整数で入力してください")
        if (!validateInt(_waterTempHigh.value, -40, 200))
            errors.add("【水温高温】-40～200の整数で入力してください")
        if (waterLow != null && waterHigh != null && waterLow >= waterHigh)
            errors.add("【水温警告】水温低温 < 水温高温 の関係にしてください")
        if (!validatePositiveInt(_fuelWarn.value, 0, 1000))
            errors.add("【燃料警告】0～1000の整数で入力してください")

        // === ギア比バリデーション ===
        val g1 = _gear1.value.toDoubleOrNull()
        val g2 = _gear2.value.toDoubleOrNull()
        val g3 = _gear3.value.toDoubleOrNull()
        val g4 = _gear4.value.toDoubleOrNull()
        val g5 = _gear5.value.toDoubleOrNull()
        val g6 = _gear6.value.toDoubleOrNull()

        if (!validatePositiveDecimal(_gear1.value)) errors.add("【1速】正の小数で入力してください")
        if (!validatePositiveDecimal(_gear2.value)) errors.add("【2速】正の小数で入力してください")
        if (!validatePositiveDecimal(_gear3.value)) errors.add("【3速】正の小数で入力してください")
        if (!validatePositiveDecimal(_gear4.value)) errors.add("【4速】正の小数で入力してください")
        if (!validatePositiveDecimal(_gear5.value)) errors.add("【5速】正の小数で入力してください")
        // 6速は0許可（5速車の場合）
        if (_gear6.value.isNotBlank() && _gear6.value != "0" && !validatePositiveDecimal(_gear6.value))
            errors.add("【6速】正の小数で入力してください（5速車は0）")
        if (!validatePositiveDecimal(_finalGear.value)) errors.add("【ファイナル】正の小数で入力してください")

        // ギア比の大小関係チェック
        if (g1 != null && g2 != null && g1 <= g2) errors.add("【ギア比】1速 > 2速 の関係にしてください")
        if (g2 != null && g3 != null && g2 <= g3) errors.add("【ギア比】2速 > 3速 の関係にしてください")
        if (g3 != null && g4 != null && g3 <= g4) errors.add("【ギア比】3速 > 4速 の関係にしてください")
        if (g4 != null && g5 != null && g4 <= g5) errors.add("【ギア比】4速 > 5速 の関係にしてください")
        if (g5 != null && g6 != null && g6 > 0 && g5 <= g6) errors.add("【ギア比】5速 > 6速 の関係にしてください")

        // === シフトインジケータバリデーション ===
        val rpm1 = _shiftRpm1.value.toIntOrNull()
        val rpm2 = _shiftRpm2.value.toIntOrNull()
        val rpm3 = _shiftRpm3.value.toIntOrNull()
        val rpm4 = _shiftRpm4.value.toIntOrNull()
        val rpm5 = _shiftRpm5.value.toIntOrNull()

        if (!validatePositiveInt(_shiftRpm1.value, 1, 15000)) errors.add("【青2灯】正の整数で1～15000の範囲で入力してください")
        if (!validatePositiveInt(_shiftRpm2.value, 1, 15000)) errors.add("【青4灯】正の整数で1～15000の範囲で入力してください")
        if (!validatePositiveInt(_shiftRpm3.value, 1, 15000)) errors.add("【緑6灯】正の整数で1～15000の範囲で入力してください")
        if (!validatePositiveInt(_shiftRpm4.value, 1, 15000)) errors.add("【赤8灯】正の整数で1～15000の範囲で入力してください")
        if (!validatePositiveInt(_shiftRpm5.value, 1, 15000)) errors.add("【白点滅】正の整数で1～15000の範囲で入力してください")

        // シフトRPMの大小関係チェック
        if (rpm1 != null && rpm2 != null && rpm1 >= rpm2) errors.add("【シフトインジケータ】青2灯 < 青4灯 の関係にしてください")
        if (rpm2 != null && rpm3 != null && rpm2 >= rpm3) errors.add("【シフトインジケータ】青4灯 < 緑6灯 の関係にしてください")
        if (rpm3 != null && rpm4 != null && rpm3 >= rpm4) errors.add("【シフトインジケータ】緑6灯 < 赤8灯 の関係にしてください")
        if (rpm4 != null && rpm5 != null && rpm4 >= rpm5) errors.add("【シフトインジケータ】赤8灯 < 白点滅 の関係にしてください")

        return errors
    }

    private fun validatePositiveInt(value: String, min: Int, max: Int): Boolean {
        if (value.isBlank()) return false
        val num = value.toIntOrNull() ?: return false
        return num in min..max
    }

    private fun validateInt(value: String, min: Int, max: Int): Boolean {
        if (value.isBlank()) return false
        val num = value.toIntOrNull() ?: return false
        return num in min..max
    }

    private fun validatePositiveDecimal(value: String): Boolean {
        if (value.isBlank()) return false
        val num = value.toDoubleOrNull() ?: return false
        return num > 0
    }

    /**
     * ViewModelファクトリ
     */
    class Factory(private val context: Context) : ViewModelProvider.Factory {
        @Suppress("UNCHECKED_CAST")
        override fun <T : ViewModel> create(modelClass: Class<T>): T {
            if (modelClass.isAssignableFrom(MainViewModel::class.java)) {
                return MainViewModel(context) as T
            }
            throw IllegalArgumentException("Unknown ViewModel class")
        }
    }
}
