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
import com.fullmoni.terminal.service.UsbSerialService
import com.hoho.android.usbserial.driver.UsbSerialDriver
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
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
    
    // Connection
    val isConnected: StateFlow<Boolean> = usbSerialService.isConnected
    val receivedData: StateFlow<String> = usbSerialService.receivedData
    val errorMessage: StateFlow<String?> = usbSerialService.errorMessage
    
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
    
    private val _bootLog = MutableStateFlow("")
    val bootLog: StateFlow<String> = _bootLog.asStateFlow()
    
    private val _isBootUploading = MutableStateFlow(false)
    val isBootUploading: StateFlow<Boolean> = _isBootUploading.asStateFlow()
    
    private val _bootUploadProgress = MutableStateFlow(0f)
    val bootUploadProgress: StateFlow<Float> = _bootUploadProgress.asStateFlow()
    
    // Firmware Update
    private val _selectedFirmwarePath = MutableStateFlow("")
    val selectedFirmwarePath: StateFlow<String> = _selectedFirmwarePath.asStateFlow()
    
    private val _firmwareProgress = MutableStateFlow(0f)
    val firmwareProgress: StateFlow<Float> = _firmwareProgress.asStateFlow()
    
    private val _firmwareProgressText = MutableStateFlow("Ready")
    val firmwareProgressText: StateFlow<String> = _firmwareProgressText.asStateFlow()
    
    private val _firmwareLog = MutableStateFlow("")
    val firmwareLog: StateFlow<String> = _firmwareLog.asStateFlow()
    
    private val _isFirmwareUpdating = MutableStateFlow(false)
    val isFirmwareUpdating: StateFlow<Boolean> = _isFirmwareUpdating.asStateFlow()
    
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
            val sdf = SimpleDateFormat("yyyy,MM,dd,HH,mm,ss", Locale.US)
            val dateStr = sdf.format(Date())
            sendCommand("set rtc $dateStr")
            delay(300)
            loadParameters()
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
    
    fun saveParameters() {
        viewModelScope.launch {
            _tyreWidth.value.takeIf { it.isNotBlank() }?.let { sendCommand("set tyre_width $it"); delay(100) }
            _tyreAspect.value.takeIf { it.isNotBlank() }?.let { sendCommand("set tyre_aspect $it"); delay(100) }
            _wheelDia.value.takeIf { it.isNotBlank() }?.let { sendCommand("set tyre_rim $it"); delay(100) }
            _gear1.value.takeIf { it.isNotBlank() }?.let { sendCommand("set gear1 $it"); delay(100) }
            _gear2.value.takeIf { it.isNotBlank() }?.let { sendCommand("set gear2 $it"); delay(100) }
            _gear3.value.takeIf { it.isNotBlank() }?.let { sendCommand("set gear3 $it"); delay(100) }
            _gear4.value.takeIf { it.isNotBlank() }?.let { sendCommand("set gear4 $it"); delay(100) }
            _gear5.value.takeIf { it.isNotBlank() }?.let { sendCommand("set gear5 $it"); delay(100) }
            _gear6.value.takeIf { it.isNotBlank() }?.let { sendCommand("set gear6 $it"); delay(100) }
            _finalGear.value.takeIf { it.isNotBlank() }?.let { sendCommand("set final $it"); delay(100) }
            _waterTempLow.value.takeIf { it.isNotBlank() }?.let { sendCommand("set water_low $it"); delay(100) }
            _waterTempHigh.value.takeIf { it.isNotBlank() }?.let { sendCommand("set water_high $it"); delay(100) }
            _fuelWarn.value.takeIf { it.isNotBlank() }?.let { sendCommand("set fuel_warn $it"); delay(100) }
            _shiftRpm1.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm1 $it"); delay(100) }
            _shiftRpm2.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm2 $it"); delay(100) }
            _shiftRpm3.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm3 $it"); delay(100) }
            _shiftRpm4.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm4 $it"); delay(100) }
            _shiftRpm5.value.takeIf { it.isNotBlank() }?.let { sendCommand("set shift_rpm5 $it"); delay(100) }
            sendCommand("save")
            Toast.makeText(context, "Parameters saved", Toast.LENGTH_SHORT).show()
        }
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
                    _bootImagePath.value = uri.lastPathSegment ?: "image.bmp"
                    appendBootLog("Image loaded: ${bitmap.width}x${bitmap.height}")
                    
                    if (bitmap.width != 765 || bitmap.height != 256) {
                        appendBootLog("Warning: Image should be 765x256 pixels")
                    }
                }
            } catch (e: Exception) {
                appendBootLog("Error loading image: ${e.message}")
            }
        }
    }
    
    fun readBootImageFromDevice() {
        viewModelScope.launch {
            appendBootLog("Reading boot image from device...")
            sendCommand("boot_read")
            // TODO: Implement binary read
        }
    }
    
    fun saveBootImageToFile(context: Context) {
        // TODO: Implement save to file
        Toast.makeText(context, "Save function not implemented", Toast.LENGTH_SHORT).show()
    }
    
    fun uploadBootImage() {
        viewModelScope.launch {
            _isBootUploading.value = true
            _bootUploadProgress.value = 0f
            appendBootLog("Starting upload...")
            
            // TODO: Implement actual upload
            for (i in 1..100) {
                delay(50)
                _bootUploadProgress.value = i / 100f
            }
            
            appendBootLog("Upload complete")
            _isBootUploading.value = false
        }
    }
    
    fun clearBootLog() { _bootLog.value = "" }
    
    fun copyBootLog(context: Context) {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        clipboard.setPrimaryClip(ClipData.newPlainText("Boot Log", _bootLog.value))
        Toast.makeText(context, "Log copied", Toast.LENGTH_SHORT).show()
    }
    
    private fun appendBootLog(message: String) {
        _bootLog.value += "> $message\n"
    }
    
    // ========== Firmware Update ==========
    
    fun selectFirmwareFile(context: Context, uri: Uri) {
        _selectedFirmwarePath.value = uri.lastPathSegment ?: "firmware.mot"
        appendFirmwareLog("Selected: ${_selectedFirmwarePath.value}")
    }
    
    fun startFirmwareUpdate() {
        viewModelScope.launch {
            _isFirmwareUpdating.value = true
            _firmwareProgress.value = 0f
            _firmwareProgressText.value = "Starting update..."
            appendFirmwareLog("Starting firmware update...")
            
            // TODO: Implement actual firmware update
            for (i in 1..100) {
                delay(100)
                _firmwareProgress.value = i.toFloat()
                _firmwareProgressText.value = "Uploading... $i%"
            }
            
            _firmwareProgressText.value = "Update complete"
            appendFirmwareLog("Firmware update complete")
            _isFirmwareUpdating.value = false
        }
    }
    
    fun clearFirmwareLog() { _firmwareLog.value = "" }
    
    fun copyFirmwareLog(context: Context) {
        val clipboard = context.getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        clipboard.setPrimaryClip(ClipData.newPlainText("Firmware Log", _firmwareLog.value))
        Toast.makeText(context, "Log copied", Toast.LENGTH_SHORT).show()
    }
    
    private fun appendFirmwareLog(message: String) {
        val timestamp = SimpleDateFormat("HH:mm:ss", Locale.US).format(Date())
        _firmwareLog.value += "[$timestamp] $message\n"
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
