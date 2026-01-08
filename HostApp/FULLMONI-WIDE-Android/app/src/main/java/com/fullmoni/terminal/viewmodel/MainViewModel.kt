package com.fullmoni.terminal.viewmodel

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Build
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import com.fullmoni.terminal.service.UsbSerialService
import com.hoho.android.usbserial.driver.UsbSerialDriver
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

/**
 * メイン画面のViewModel
 */
class MainViewModel(private val context: Context) : ViewModel() {
    
    companion object {
        private const val ACTION_USB_PERMISSION = "com.fullmoni.terminal.USB_PERMISSION"
    }
    
    private val usbSerialService = UsbSerialService(context)
    
    val isConnected: StateFlow<Boolean> = usbSerialService.isConnected
    val receivedData: StateFlow<String> = usbSerialService.receivedData
    val errorMessage: StateFlow<String?> = usbSerialService.errorMessage
    
    private val _availableDrivers = MutableStateFlow<List<UsbSerialDriver>>(emptyList())
    val availableDrivers: StateFlow<List<UsbSerialDriver>> = _availableDrivers.asStateFlow()
    
    private val _selectedDriver = MutableStateFlow<UsbSerialDriver?>(null)
    val selectedDriver: StateFlow<UsbSerialDriver?> = _selectedDriver.asStateFlow()
    
    private val _commandHistory = MutableStateFlow<List<String>>(emptyList())
    val commandHistory: StateFlow<List<String>> = _commandHistory.asStateFlow()
    
    private val _currentTab = MutableStateFlow(0)
    val currentTab: StateFlow<Int> = _currentTab.asStateFlow()
    
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
                            }
                        }
                    }
                    pendingDriver = null
                }
            }
        }
    }
    
    init {
        // USB許可レシーバーを登録
        val filter = IntentFilter(ACTION_USB_PERMISSION)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            context.registerReceiver(usbPermissionReceiver, filter, Context.RECEIVER_NOT_EXPORTED)
        } else {
            context.registerReceiver(usbPermissionReceiver, filter)
        }
        
        // 初期デバイス検索
        refreshDevices()
    }
    
    override fun onCleared() {
        super.onCleared()
        try {
            context.unregisterReceiver(usbPermissionReceiver)
        } catch (e: Exception) {
            // 未登録の場合は無視
        }
        usbSerialService.disconnect()
    }
    
    /**
     * デバイスリストを更新
     */
    fun refreshDevices() {
        viewModelScope.launch {
            val drivers = usbSerialService.refreshDevices()
            _availableDrivers.value = drivers
            
            // 1つだけの場合は自動選択
            if (drivers.size == 1) {
                _selectedDriver.value = drivers[0]
            }
        }
    }
    
    /**
     * デバイスを選択
     */
    fun selectDriver(driver: UsbSerialDriver) {
        _selectedDriver.value = driver
    }
    
    /**
     * 接続を開始
     */
    fun connect() {
        val driver = _selectedDriver.value ?: return
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        
        if (usbManager.hasPermission(driver.device)) {
            usbSerialService.connect(driver)
        } else {
            // 許可をリクエスト
            pendingDriver = driver
            val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                PendingIntent.FLAG_MUTABLE
            } else {
                0
            }
            val permissionIntent = PendingIntent.getBroadcast(
                context,
                0,
                Intent(ACTION_USB_PERMISSION),
                flags
            )
            usbManager.requestPermission(driver.device, permissionIntent)
        }
    }
    
    /**
     * 切断
     */
    fun disconnect() {
        usbSerialService.disconnect()
    }
    
    /**
     * コマンドを送信
     */
    fun sendCommand(command: String) {
        if (command.isBlank()) return
        
        // 履歴に追加
        val history = _commandHistory.value.toMutableList()
        history.remove(command) // 重複を削除
        history.add(0, command) // 先頭に追加
        if (history.size > 50) {
            history.removeAt(history.lastIndex)
        }
        _commandHistory.value = history
        
        usbSerialService.sendCommand(command)
    }
    
    /**
     * ターミナルをクリア
     */
    fun clearTerminal() {
        usbSerialService.clearBuffer()
    }
    
    /**
     * エラーをクリア
     */
    fun clearError() {
        usbSerialService.clearError()
    }
    
    /**
     * タブを切り替え
     */
    fun setCurrentTab(tab: Int) {
        _currentTab.value = tab
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
