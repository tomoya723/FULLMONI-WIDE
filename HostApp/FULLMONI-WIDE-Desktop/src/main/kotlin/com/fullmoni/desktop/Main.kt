package com.fullmoni.desktop

import com.fazecast.jSerialComm.SerialPort
import kotlinx.coroutines.*
import java.util.Scanner

/**
 * FULLMONI-WIDE Desktop Test Application
 * 
 * PCからUSB CDC経由でFULLMONI-WIDE実機と通信するテストツール
 * Androidアプリのロジックをデバッグするために使用
 */
fun main(args: Array<String>) = runBlocking {
    println("===========================================")
    println("  FULLMONI-WIDE Desktop Test Tool")
    println("  USB CDC Serial Communication Tester")
    println("===========================================")
    println()
    
    val serialService = SerialService()
    val scanner = Scanner(System.`in`)
    
    // 引数でポート指定があれば自動接続
    if (args.isNotEmpty()) {
        val portName = args[0]
        val ports = SerialPort.getCommPorts()
        val port = ports.find { it.systemPortName.equals(portName, ignoreCase = true) }
        if (port != null) {
            serialService.connect(port)
        } else {
            println("Port not found: $portName")
            println("Available ports:")
            serialService.listPorts()
        }
    }
    
    // メインループ
    while (true) {
        if (!serialService.isConnected) {
            showDisconnectedMenu()
        } else {
            showConnectedPrompt()
        }
        
        print("> ")
        System.out.flush()
        
        if (!scanner.hasNextLine()) {
            delay(100)
            continue
        }
        
        val input = scanner.nextLine().trim()
        if (input.isEmpty()) continue
        
        if (!serialService.isConnected) {
            when (input.lowercase()) {
                "l", "list" -> serialService.listPorts()
                "q", "quit", "exit" -> {
                    println("Goodbye!")
                    break
                }
                else -> {
                    // ポート番号またはポート名で接続
                    val portIndex = input.toIntOrNull()
                    if (portIndex != null) {
                        serialService.connect(portIndex)
                    } else {
                        // ポート名で検索
                        val ports = SerialPort.getCommPorts()
                        val port = ports.find { 
                            it.systemPortName.equals(input, ignoreCase = true) ||
                            it.systemPortName.equals("COM$input", ignoreCase = true)
                        }
                        if (port != null) {
                            serialService.connect(port)
                        } else {
                            println("Unknown command or port: $input")
                        }
                    }
                }
            }
        } else {
            when (input.lowercase()) {
                "d", "disconnect" -> serialService.disconnect()
                "q", "quit", "exit" -> {
                    serialService.disconnect()
                    println("Goodbye!")
                    break
                }
                "clear" -> serialService.clearBuffer()
                "buf", "buffer" -> serialService.showBuffer()
                "help" -> showHelp()
                else -> {
                    // コマンド送信
                    serialService.sendCommand(input)
                    // 応答待ち
                    delay(500)
                }
            }
        }
    }
}

fun showDisconnectedMenu() {
    println()
    println("--- Not Connected ---")
    println("Commands: l=list ports, <number>=connect, q=quit")
}

fun showConnectedPrompt() {
    // 何も表示しない（プロンプトのみ）
}

fun showHelp() {
    println("""
        |
        |=== FULLMONI-WIDE Commands ===
        |
        | Device Info:
        |   ver           - Show firmware version
        |   help          - Show device help
        |
        | Status:
        |   odo           - Read odometer
        |   trip          - Read trip meter
        |   rtc           - Read real-time clock
        |
        | Settings:
        |   tyre          - Read tyre settings
        |   gear          - Read gear ratios
        |   warn          - Read warning thresholds
        |   shift_rpm     - Read shift RPM settings
        |
        | CAN Config:
        |   can_list      - Show all CAN configuration
        |   can_warning 0/1  - Enable/disable master warning
        |   can_sound 0/1    - Enable/disable warning sound
        |
        | Boot Screen:
        |   boot_read     - Read boot screen data
        |
        | Connection:
        |   d, disconnect - Disconnect from device
        |   clear         - Clear receive buffer
        |   buf, buffer   - Show receive buffer
        |   q, quit       - Exit application
        |
    """.trimMargin())
}
