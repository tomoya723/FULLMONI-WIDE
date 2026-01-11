package com.fullmoni.terminal.service

import android.content.Context
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import com.hoho.android.usbserial.driver.UsbSerialDriver
import com.hoho.android.usbserial.driver.UsbSerialPort
import com.hoho.android.usbserial.driver.UsbSerialProber
import com.hoho.android.usbserial.util.SerialInputOutputManager
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import java.io.IOException
import java.util.concurrent.Executors

/**
 * USBシリアル通信サービス
 * FULLMONI-WIDE ファームウェアとのUSB-UART通信を管理
 */
class UsbSerialService(private val context: Context) {
    
    companion object {
        const val BAUD_RATE = 115200
        const val DATA_BITS = 8
        const val STOP_BITS = UsbSerialPort.STOPBITS_1
        const val PARITY = UsbSerialPort.PARITY_NONE
        const val READ_WAIT_MILLIS = 200
        const val WRITE_TIMEOUT_MILLIS = 500
    }
    
    private var usbSerialPort: UsbSerialPort? = null
    private var serialInputOutputManager: SerialInputOutputManager? = null
    
    private val _isConnected = MutableStateFlow(false)
    val isConnected: StateFlow<Boolean> = _isConnected.asStateFlow()
    
    private val _receivedData = MutableStateFlow("")
    val receivedData: StateFlow<String> = _receivedData.asStateFlow()
    
    private val _errorMessage = MutableStateFlow<String?>(null)
    val errorMessage: StateFlow<String?> = _errorMessage.asStateFlow()
    
    private val _availableDevices = MutableStateFlow<List<UsbDevice>>(emptyList())
    val availableDevices: StateFlow<List<UsbDevice>> = _availableDevices.asStateFlow()
    
    private val terminalBuffer = StringBuilder()
    
    private val listener = object : SerialInputOutputManager.Listener {
        override fun onNewData(data: ByteArray) {
            val text = String(data, Charsets.US_ASCII)
            terminalBuffer.append(text)
            _receivedData.value = terminalBuffer.toString()
        }
        
        override fun onRunError(e: Exception) {
            _errorMessage.value = "Read error: ${e.message}"
            disconnect()
        }
    }
    
    /**
     * 利用可能なUSBシリアルデバイスを検索
     */
    fun refreshDevices(): List<UsbSerialDriver> {
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        val drivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager)
        _availableDevices.value = drivers.map { it.device }
        return drivers
    }
    
    /**
     * USBシリアルデバイスに接続
     */
    fun connect(driver: UsbSerialDriver): Boolean {
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        
        // USB許可チェック
        if (!usbManager.hasPermission(driver.device)) {
            _errorMessage.value = "USB permission not granted"
            return false
        }
        
        val connection = usbManager.openDevice(driver.device)
        if (connection == null) {
            _errorMessage.value = "Failed to open device"
            return false
        }
        
        try {
            val port = driver.ports[0]
            port.open(connection)
            port.setParameters(BAUD_RATE, DATA_BITS, STOP_BITS, PARITY)
            
            // XON/XOFFフロー制御の設定（サポートされている場合）
            try {
                port.setDTR(true)
                port.setRTS(true)
            } catch (e: Exception) {
                // 一部のドライバはDTR/RTSをサポートしていない
            }
            
            usbSerialPort = port
            
            // 受信マネージャを開始
            serialInputOutputManager = SerialInputOutputManager(port, listener).also {
                Executors.newSingleThreadExecutor().submit(it)
            }
            
            _isConnected.value = true
            _errorMessage.value = null
            return true
            
        } catch (e: IOException) {
            _errorMessage.value = "Connection failed: ${e.message}"
            return false
        }
    }
    
    /**
     * 接続を切断
     */
    fun disconnect() {
        serialInputOutputManager?.listener = null
        serialInputOutputManager?.stop()
        serialInputOutputManager = null
        
        try {
            usbSerialPort?.close()
        } catch (e: IOException) {
            // 切断時のエラーは無視
        }
        usbSerialPort = null
        
        _isConnected.value = false
    }
    
    /**
     * コマンドを送信
     */
    fun sendCommand(command: String): Boolean {
        val port = usbSerialPort
        if (port == null || !_isConnected.value) {
            _errorMessage.value = "Not connected"
            return false
        }
        
        return try {
            // コマンドに改行を付加して送信（ファームウェア仕様に合わせて \r）
            val data = (command + "\r").toByteArray(Charsets.US_ASCII)
            port.write(data, WRITE_TIMEOUT_MILLIS)
            true
        } catch (e: IOException) {
            _errorMessage.value = "Send error: ${e.message}"
            false
        }
    }
    
    /**
     * 生データを送信
     */
    fun sendRaw(data: ByteArray): Boolean {
        val port = usbSerialPort
        if (port == null || !_isConnected.value) {
            _errorMessage.value = "Not connected"
            return false
        }
        
        return try {
            port.write(data, WRITE_TIMEOUT_MILLIS)
            true
        } catch (e: IOException) {
            _errorMessage.value = "Send error: ${e.message}"
            false
        }
    }
    
    /**
     * ターミナルバッファをクリア
     */
    fun clearBuffer() {
        terminalBuffer.clear()
        _receivedData.value = ""
    }
    
    /**
     * エラーメッセージをクリア
     */
    fun clearError() {
        _errorMessage.value = null
    }
}
