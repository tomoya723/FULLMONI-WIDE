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
    
    private fun onConnected(driver: UsbSerialDriver) {
        _connectionPort.value = driver.device.deviceName
        viewModelScope.launch {
            delay(500)
            // バージョン取得
            sendCommand("version")
            delay(300)
            parseVersionFromResponse()
            // パラメータ読み込み
            loadParameters()
        }
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
        viewModelScope.launch {
            sendCommand("list")
            delay(500)
            parseParametersFromResponse()
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
        val versionMatch = Regex("v?([0-9]+\\.[0-9]+\\.[0-9]+)").find(data)
        versionMatch?.groupValues?.get(1)?.let {
            _firmwareVersion.value = it
        }
    }
    
    private fun parseParametersFromResponse() {
        val data = receivedData.value
        // Parse key=value pairs
        val lines = data.lines()
        for (line in lines) {
            val parts = line.split("=", ":").map { it.trim() }
            if (parts.size >= 2) {
                when (parts[0].lowercase()) {
                    "tyre_width", "tyrewidth" -> _tyreWidth.value = parts[1]
                    "tyre_aspect", "tyreaspect" -> _tyreAspect.value = parts[1]
                    "tyre_rim", "rim", "wheel_dia" -> _wheelDia.value = parts[1]
                    "gear1" -> _gear1.value = parts[1]
                    "gear2" -> _gear2.value = parts[1]
                    "gear3" -> _gear3.value = parts[1]
                    "gear4" -> _gear4.value = parts[1]
                    "gear5" -> _gear5.value = parts[1]
                    "gear6" -> _gear6.value = parts[1]
                    "final", "finalgear" -> _finalGear.value = parts[1]
                    "water_low" -> _waterTempLow.value = parts[1]
                    "water_high" -> _waterTempHigh.value = parts[1]
                    "fuel_warn" -> _fuelWarn.value = parts[1]
                    "shift_rpm1" -> _shiftRpm1.value = parts[1]
                    "shift_rpm2" -> _shiftRpm2.value = parts[1]
                    "shift_rpm3" -> _shiftRpm3.value = parts[1]
                    "shift_rpm4" -> _shiftRpm4.value = parts[1]
                    "shift_rpm5" -> _shiftRpm5.value = parts[1]
                    "odo" -> _odoValue.value = parts[1]
                    "trip" -> _tripValue.value = parts[1]
                    "rtc", "datetime" -> _rtcValue.value = parts[1]
                }
            }
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
            sendCommand("can_list")
            delay(500)
            // Parse CAN config from response
        }
    }
    
    fun saveCanConfig() {
        viewModelScope.launch {
            // Send CAN config commands
            sendCommand("can_save")
            Toast.makeText(context, "CAN config saved", Toast.LENGTH_SHORT).show()
        }
    }
    
    fun defaultCanConfig() {
        viewModelScope.launch {
            sendCommand("can_default")
            delay(500)
            loadCanConfig()
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
