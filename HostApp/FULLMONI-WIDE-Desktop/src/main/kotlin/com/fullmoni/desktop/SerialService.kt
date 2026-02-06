package com.fullmoni.desktop

import com.fazecast.jSerialComm.SerialPort
import com.fazecast.jSerialComm.SerialPortDataListener
import com.fazecast.jSerialComm.SerialPortEvent

/**
 * USB Serial Communication Service
 * 
 * jSerialCommを使用してFULLMONI-WIDEと通信
 * Android版のUsbSerialServiceと同等の機能を提供
 */
class SerialService {
    
    companion object {
        const val BAUD_RATE = 115200
        const val DATA_BITS = 8
        const val STOP_BITS = SerialPort.ONE_STOP_BIT
        const val PARITY = SerialPort.NO_PARITY
        const val READ_TIMEOUT_MS = 100
        const val WRITE_TIMEOUT_MS = 500
    }
    
    private var serialPort: SerialPort? = null
    private val receiveBuffer = StringBuilder()
    
    val isConnected: Boolean
        get() = serialPort?.isOpen == true
    
    /**
     * 利用可能なCOMポートを一覧表示
     */
    fun listPorts(): List<SerialPort> {
        val ports = SerialPort.getCommPorts()
        
        if (ports.isEmpty()) {
            println("No COM ports found.")
            return emptyList()
        }
        
        println()
        println("Available COM Ports:")
        println("--------------------")
        ports.forEachIndexed { index, port ->
            val status = if (port.isOpen) " [OPEN]" else ""
            println("  [$index] ${port.systemPortName} - ${port.descriptivePortName}$status")
        }
        println()
        println("Enter port number to connect (e.g., '0')")
        
        return ports.toList()
    }
    
    /**
     * 指定インデックスのポートに接続
     */
    fun connect(portIndex: Int): Boolean {
        val ports = SerialPort.getCommPorts()
        
        if (portIndex < 0 || portIndex >= ports.size) {
            println("Error: Invalid port index: $portIndex")
            return false
        }
        
        val port = ports[portIndex]
        return connect(port)
    }
    
    /**
     * 指定ポートに接続
     */
    fun connect(port: SerialPort): Boolean {
        // 既存接続を切断
        disconnect()
        
        println("Connecting to ${port.systemPortName}...")
        
        // ポート設定
        port.baudRate = BAUD_RATE
        port.numDataBits = DATA_BITS
        port.numStopBits = STOP_BITS
        port.parity = PARITY
        
        // タイムアウト設定 - 書き込み用にブロッキングモードを使用
        port.setComPortTimeouts(
            SerialPort.TIMEOUT_READ_SEMI_BLOCKING or SerialPort.TIMEOUT_WRITE_BLOCKING,
            READ_TIMEOUT_MS,
            WRITE_TIMEOUT_MS
        )
        
        // フロー制御なし
        port.setFlowControl(SerialPort.FLOW_CONTROL_DISABLED)
        
        // 接続
        if (!port.openPort()) {
            println("Error: Failed to open ${port.systemPortName}")
            return false
        }
        
        println("Port opened successfully")
        
        // データリスナー設定
        port.addDataListener(object : SerialPortDataListener {
            override fun getListeningEvents(): Int = SerialPort.LISTENING_EVENT_DATA_AVAILABLE
            
            override fun serialEvent(event: SerialPortEvent) {
                if (event.eventType != SerialPort.LISTENING_EVENT_DATA_AVAILABLE) return
                
                val bytesAvailable = port.bytesAvailable()
                if (bytesAvailable <= 0) return
                
                val buffer = ByteArray(bytesAvailable)
                port.readBytes(buffer, buffer.size)
                
                val text = String(buffer, Charsets.US_ASCII)
                synchronized(receiveBuffer) {
                    receiveBuffer.append(text)
                }
                
                // リアルタイム表示
                print(text)
            }
        })
        
        serialPort = port
        println("Connected to ${port.systemPortName} at ${BAUD_RATE}bps")
        println()
        
        // 初回接続時にバージョン取得
        Thread.sleep(500)
        sendCommand("ver")
        
        return true
    }
    
    /**
     * 接続を切断
     */
    fun disconnect() {
        serialPort?.let { port ->
            port.removeDataListener()
            if (port.isOpen) {
                port.closePort()
                println("Disconnected from ${port.systemPortName}")
            }
        }
        serialPort = null
        clearBuffer()
    }
    
    /**
     * コマンド送信
     */
    fun sendCommand(command: String): Boolean {
        val port = serialPort
        if (port == null || !port.isOpen) {
            println("Error: Not connected")
            return false
        }
        
        val data = (command + "\r").toByteArray(Charsets.US_ASCII)
        
        // デバッグ: 送信データを表示
        println("[TX] '$command' (${data.size} bytes)")
        
        try {
            // OutputStreamを使用して書き込み
            val outputStream = port.outputStream
            outputStream.write(data)
            outputStream.flush()
            return true
        } catch (e: Exception) {
            println("Error: Failed to send - ${e.message}")
            return false
        }
    }
    
    /**
     * 生データ送信
     */
    fun sendRaw(data: ByteArray): Boolean {
        val port = serialPort
        if (port == null || !port.isOpen) {
            println("Error: Not connected")
            return false
        }
        
        val written = port.writeBytes(data, data.size)
        return written == data.size
    }
    
    /**
     * 受信バッファをクリア
     */
    fun clearBuffer() {
        synchronized(receiveBuffer) {
            receiveBuffer.clear()
        }
        println("Buffer cleared")
    }
    
    /**
     * 受信バッファ内容を表示
     */
    fun showBuffer() {
        synchronized(receiveBuffer) {
            if (receiveBuffer.isEmpty()) {
                println("(buffer empty)")
            } else {
                println("--- Buffer Content ---")
                println(receiveBuffer.toString())
                println("--- End of Buffer ---")
            }
        }
    }
    
    /**
     * 受信バッファ取得
     */
    fun getBuffer(): String {
        synchronized(receiveBuffer) {
            return receiveBuffer.toString()
        }
    }
}
