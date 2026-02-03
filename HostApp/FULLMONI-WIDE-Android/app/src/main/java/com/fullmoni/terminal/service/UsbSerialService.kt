package com.fullmoni.terminal.service

import android.content.Context
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.util.Log
import com.hoho.android.usbserial.driver.UsbSerialDriver
import com.hoho.android.usbserial.driver.UsbSerialPort
import com.hoho.android.usbserial.driver.UsbSerialProber
import com.hoho.android.usbserial.util.SerialInputOutputManager
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import java.io.IOException
import java.net.Socket
import java.util.concurrent.Executors

/**
 * USBシリアル通信サービス
 * FULLMONI-WIDE ファームウェアとのUSB-UART通信を管理
 * シミュレーションモード対応（エミュレーターでのテスト用）
 * TCPブリッジモード対応（エミュレーターから実機テスト用）
 */
class UsbSerialService(private val context: Context) {
    
    companion object {
        private const val TAG = "UsbSerialService"
        const val BAUD_RATE = 115200
        const val DATA_BITS = 8
        const val STOP_BITS = UsbSerialPort.STOPBITS_1
        const val PARITY = UsbSerialPort.PARITY_NONE
        const val READ_WAIT_MILLIS = 200
        const val WRITE_TIMEOUT_MILLIS = 500
        
        // TCPブリッジ設定
        // adb reverse tcp:9999 tcp:9999 を実行すれば 127.0.0.1 でPCにアクセス可能
        const val TCP_BRIDGE_HOST = "127.0.0.1"
        const val TCP_BRIDGE_PORT = 9999
    }
    
    // シミュレーションモード
    private val simulatorService = SimulatorService()
    private var _isSimulationMode = false
    val isSimulationMode: Boolean get() = _isSimulationMode
    
    // TCPブリッジモード（エミュレーターから実機に接続）
    private var _isTcpBridgeMode = false
    val isTcpBridgeMode: Boolean get() = _isTcpBridgeMode
    private var tcpSocket: Socket? = null
    
    private val coroutineScope = CoroutineScope(Dispatchers.IO)
    
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
     * シミュレーションモードで接続（エミュレーター用）
     */
    fun connectSimulator(): Boolean {
        Log.d(TAG, "connectSimulator: Entering simulation mode")
        _isSimulationMode = true
        _isTcpBridgeMode = false
        _isConnected.value = true
        _errorMessage.value = null
        return true
    }
    
    /**
     * TCPブリッジモードで実機に接続（エミュレーターから実機テスト用）
     * 事前に以下を実行：
     * 1. PC側: .\tools\com_bridge.ps1 -ComPort COM19
     * 2. adb reverse tcp:9999 tcp:9999
     */
    suspend fun connectTcpBridge(): Boolean {
        Log.d(TAG, "connectTcpBridge: Starting TCP bridge connection to $TCP_BRIDGE_HOST:$TCP_BRIDGE_PORT")
        
        return try {
            // 接続をIOディスパッチャーで実行
            kotlinx.coroutines.withContext(Dispatchers.IO) {
                Log.d(TAG, "connectTcpBridge: Creating socket...")
                val socket = Socket()
                socket.connect(java.net.InetSocketAddress(TCP_BRIDGE_HOST, TCP_BRIDGE_PORT), 5000)
                socket.soTimeout = 2000
                tcpSocket = socket
                _isTcpBridgeMode = true
                _isSimulationMode = false
                _isConnected.value = true
                _errorMessage.value = null
                
                Log.d(TAG, "connectTcpBridge: Connected successfully!")
                true
            }.also {
                // TCP受信ループを別のコルーチンで開始（withContextの外で）
                if (it) {
                    startTcpReceiveLoop()
                }
            }
        } catch (e: Exception) {
            Log.e(TAG, "connectTcpBridge failed: ${e.message}")
            _errorMessage.value = "TCP connection failed: ${e.message}"
            _isTcpBridgeMode = false
            _isConnected.value = false
            false
        }
    }
    
    /**
     * TCP受信ループを開始
     */
    private fun startTcpReceiveLoop() {
        coroutineScope.launch(Dispatchers.IO) {
            val buffer = ByteArray(1024)
            val socket = tcpSocket ?: return@launch
            try {
                Log.d(TAG, "TCP receive loop started")
                while (_isTcpBridgeMode && !socket.isClosed) {
                    try {
                        val bytesRead = socket.getInputStream()?.read(buffer) ?: -1
                        if (bytesRead > 0) {
                            val text = String(buffer, 0, bytesRead, Charsets.US_ASCII)
                            Log.d(TAG, "TCP RX: $text")
                            terminalBuffer.append(text)
                            _receivedData.value = terminalBuffer.toString()
                        } else if (bytesRead == -1) {
                            Log.d(TAG, "TCP connection closed by server")
                            break
                        }
                    } catch (e: java.net.SocketTimeoutException) {
                        // タイムアウトは正常、ループ継続
                    }
                }
            } catch (e: Exception) {
                Log.e(TAG, "TCP receive loop error: ${e.message}")
            } finally {
                Log.d(TAG, "TCP receive loop ended")
            }
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
        _isSimulationMode = false
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
        if (_isSimulationMode) {
            _isSimulationMode = false
            _isConnected.value = false
            return
        }
        
        if (_isTcpBridgeMode) {
            _isTcpBridgeMode = false
            try {
                tcpSocket?.close()
            } catch (e: Exception) {
                // 切断時のエラーは無視
            }
            tcpSocket = null
            _isConnected.value = false
            return
        }
        
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
        Log.d(TAG, "sendCommand: $command (simMode=$_isSimulationMode, tcpMode=$_isTcpBridgeMode, connected=${_isConnected.value})")
        
        if (!_isConnected.value) {
            Log.w(TAG, "sendCommand: Not connected")
            _errorMessage.value = "Not connected"
            return false
        }
        
        // シミュレーションモードの場合
        if (_isSimulationMode) {
            Log.d(TAG, "sendCommand: Using simulator for command '$command'")
            coroutineScope.launch {
                Log.d(TAG, "sendCommand: Calling simulatorService.processCommand")
                val response = simulatorService.processCommand(command)
                Log.d(TAG, "sendCommand: Got response (${response.length} chars): ${response.take(100)}...")
                terminalBuffer.append("> $command\r\n")
                terminalBuffer.append(response)
                _receivedData.value = terminalBuffer.toString()
                Log.d(TAG, "sendCommand: Updated receivedData, total buffer size=${terminalBuffer.length}")
            }
            return true
        }
        
        // TCPブリッジモードの場合
        if (_isTcpBridgeMode) {
            val socket = tcpSocket
            if (socket == null || !socket.isConnected) {
                _errorMessage.value = "TCP not connected"
                return false
            }
            // TCP送信をバックグラウンドで実行
            coroutineScope.launch {
                try {
                    val data = (command + "\r").toByteArray(Charsets.US_ASCII)
                    Log.d(TAG, "TCP TX: $command")
                    socket.getOutputStream().write(data)
                    socket.getOutputStream().flush()
                } catch (e: Exception) {
                    Log.e(TAG, "TCP send error: ${e.message}")
                    _errorMessage.value = "TCP send error: ${e.message}"
                }
            }
            return true
        }
        
        val port = usbSerialPort
        if (port == null) {
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
        if (!_isConnected.value) {
            _errorMessage.value = "Not connected"
            return false
        }
        
        // シミュレーションモードの場合は単に成功を返す
        if (_isSimulationMode) {
            return true
        }
        
        val port = usbSerialPort
        if (port == null) {
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
