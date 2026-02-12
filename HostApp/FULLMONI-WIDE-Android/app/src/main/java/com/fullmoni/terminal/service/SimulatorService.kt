package com.fullmoni.terminal.service

import android.util.Log
import kotlinx.coroutines.delay

/**
 * FULLMONI WIDE ファームウェアシミュレーター
 * エミュレーターでのテスト用に、ファームウェア応答をシミュレートする
 */
class SimulatorService {
    
    companion object {
        const val SIMULATOR_DEVICE_NAME = "FULLMONI Simulator"
        private const val TAG = "SimulatorService"
    }
    
    // シミュレートされた内部状態 (FULLMONI-WIDE実機のデフォルト値: param_storage.c PARAM_DEFAULT)
    private var odoValue: Int = 0
    private var tripValue: Int = 0
    private var tyreWidth: Int = 195      // 195/50 R15
    private var tyreAspect: Int = 50
    private var wheelDia: Int = 15
    // ギア比は1000倍の整数で保存（ファームウェアと同じ形式）
    private var gear1: Int = 3760         // dataregister.c table_gear_ratio * 1000
    private var gear2: Int = 2269
    private var gear3: Int = 1645
    private var gear4: Int = 1257
    private var gear5: Int = 1000
    private var gear6: Int = 843
    private var finalGear: Int = 4300     // table_final_gear_ratio * 1000
    private var waterTempLow: Int = 60    // water_temp_low
    private var waterTempHigh: Int = 100  // water_temp_high
    private var fuelWarn: Int = 10        // fuel_warn_level
    private var shiftRpm = listOf(5500, 6000, 6500, 7000, 7500)  // shift_rpm1-5
    private var canWarningEnabled: Boolean = true
    private var canSoundEnabled: Boolean = true
    
    // CAN Channel config: id, enabled
    private val canChannels = mutableListOf(
        Pair("0x7E0", true),
        Pair("0x7E1", false),
        Pair("0x000", false),
        Pair("0x000", false),
        Pair("0x000", false),
        Pair("0x000", false)
    )
    
    // CAN Field config (簡略化)
    private val canFields = mutableListOf(
        CanFieldConfig(true, 1, 0, 2, "U", "B", "VAR_SPEED", 0f, 1f, 10f, "Speed", "km/h", 0, false, 0f, false, 0f),
        CanFieldConfig(true, 1, 2, 2, "U", "B", "VAR_RPM", 0f, 1f, 4f, "RPM", "rpm", 0, false, 0f, true, 7000f),
        CanFieldConfig(true, 1, 4, 1, "U", "B", "VAR_WATER_TEMP", -40f, 1f, 1f, "Water", "C", 0, true, 70f, true, 105f),
        CanFieldConfig(true, 1, 5, 1, "U", "B", "VAR_FUEL", 0f, 100f, 255f, "Fuel", "%", 0, true, 10f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f),
        CanFieldConfig(false, 1, 0, 1, "U", "B", "VAR_NONE", 0f, 1f, 1f, "", "", 0, false, 0f, false, 0f)
    )
    
    data class CanFieldConfig(
        var enabled: Boolean,
        var channel: Int,
        var startByte: Int,
        var byteCount: Int,
        var dataType: String,
        var endian: String,
        var targetVar: String,
        var offset: Float,
        var multiplier: Float,
        var divisor: Float,
        var name: String,
        var unit: String,
        var decimals: Int,
        var warnLowEnabled: Boolean,
        var warnLow: Float,
        var warnHighEnabled: Boolean,
        var warnHigh: Float
    )
    
    /**
     * コマンドを処理して応答を生成
     */
    suspend fun processCommand(command: String): String {
        // 少し遅延を入れてリアルな通信をシミュレート
        delay(50)
        
        val cmd = command.trim()
        Log.d(TAG, "processCommand called with: '$cmd' (length=${cmd.length})")
        
        val response = when {
            cmd == "" || cmd == "\r" -> generateWelcomeResponse()  // 空文字でパラメータモード開始
            cmd == "version" -> generateVersionResponse()
            cmd == "ver" -> "Unknown command: ver\r\nType 'help' for command list.\r\n"
            cmd == "list" -> {
                Log.d(TAG, "Generating list response")
                generateListResponse()
            }
            cmd == "bootloader_ver" -> "ERROR: Unknown command\r\n"
            cmd == "odo" -> "ODO: $odoValue km\r\n"
            cmd == "trip" -> "TRIP: ${tripValue / 10.0} km\r\n"
            cmd == "rtc" -> generateRtcResponse()
            cmd == "can_list" -> generateCanListResponse()
            cmd == "can_save" -> "CAN config saved\r\n"
            cmd.startsWith("set ") -> handleSet(cmd)
            cmd == "save" -> "Parameters saved to EEPROM\r\n"
            cmd == "load" -> "Parameters loaded from EEPROM\r\n"
            cmd == "default" -> "Parameters reset to default\r\n"
            cmd.startsWith("can_warning ") -> handleCanWarning(cmd)
            cmd.startsWith("can_sound ") -> handleCanSound(cmd)
            cmd.startsWith("can_ch ") -> handleCanChannel(cmd)
            cmd.startsWith("can_field ") -> handleCanField(cmd)
            cmd == "exit" -> "Exiting parameter mode...\r\n"
            cmd == "help" -> generateHelpResponse()
            else -> "Unknown command: $cmd\r\nType 'help' for command list.\r\n"
        }
        
        Log.d(TAG, "Response: ${response.take(100)}...")
        return response
    }
    
    private fun generateWelcomeResponse(): String {
        return """
            |[FULLMONI-WIDE] USB CDC Ready. Send 'PARAM_ENTER' to enter parameter mode.
            |
            |=========================================
            |  FULLMONI-WIDE Parameter Console
            |  Type 'help' for command list.
            |  Type 'exit' to return to normal mode.
            |=========================================
            |
        """.trimMargin()
    }
    
    private fun generateVersionResponse(): String {
        return "VERSION 2.0.0\r\n"
    }
    
    private fun generateListResponse(): String {
        return """
            |
            |=== Current Parameters ===
            |Tyre: $tyreWidth/$tyreAspect R$wheelDia
            |Gear1: ${"%.3f".format(gear1 / 1000.0)}  Gear2: ${"%.3f".format(gear2 / 1000.0)}  Gear3: ${"%.3f".format(gear3 / 1000.0)}
            |Gear4: ${"%.3f".format(gear4 / 1000.0)}  Gear5: ${"%.3f".format(gear5 / 1000.0)}  Gear6: ${"%.3f".format(gear6 / 1000.0)}
            |Final: ${"%.3f".format(finalGear / 1000.0)}
            |Water Temp Warning: $waterTempLow - $waterTempHigh C
            |Fuel Warning: $fuelWarn %
            |Shift RPM: ${shiftRpm.joinToString(" / ")}
            |ODO: $odoValue km  TRIP: ${tripValue / 10.0} km
            |
        """.trimMargin()
    }
    
    private fun generateRtcResponse(): String {
        val now = java.text.SimpleDateFormat("yyyy/MM/dd HH:mm:ss", java.util.Locale.US)
            .format(java.util.Date())
        return "RTC: $now\r\n"
    }
    
    private fun generateCanListResponse(): String {
        val sb = StringBuilder()
        sb.append("\r\n=== CAN Configuration ===\r\n")
        sb.append("Version: 1, Preset: 0\r\n")
        sb.append("Warning: ${if (canWarningEnabled) "ON" else "OFF"}, Sound: ${if (canSoundEnabled) "ON" else "OFF"}\r\n")
        sb.append("\r\n-- Channels --\r\n")
        
        canChannels.forEachIndexed { index, (id, enabled) ->
            sb.append("CH${index + 1}: ID=$id, ${if (enabled) "ON" else "OFF"}\r\n")
        }
        
        sb.append("\r\n-- Fields (16) --\r\n")
        sb.append("No CH Byte Len Type End Var    Off  Mul   Div  Dsh Name    Unit WLo  Lo WHi    Hi\r\n")
        
        canFields.forEachIndexed { index, field ->
            val ch = if (field.enabled) field.channel else 0
            val varName = if (field.enabled) field.targetVar.take(6).padEnd(6) else "---   "
            val name = if (field.enabled) field.name.take(7).padEnd(7) else "-      "
            val unit = if (field.enabled) field.unit.take(4).padEnd(4) else "-   "
            val wLoEn = if (field.warnLowEnabled) "Y" else "N"
            val wHiEn = if (field.warnHighEnabled) "Y" else "N"
            val wLo = if (field.enabled) "${"%.2f".format(field.warnLow)}" else "---"
            val wHi = if (field.enabled) "${"%.2f".format(field.warnHigh)}" else "---"
            
            sb.append("%2d  %d   %d   %d   %s    %s   %s  %4.0f  %4.0f  %4.0f  %d  %s %s %s %6s %s %6s\r\n".format(
                index, ch, field.startByte, field.byteCount, field.dataType, field.endian,
                varName, field.offset, field.multiplier, field.divisor, field.decimals,
                name, unit, wLoEn, wLo, wHiEn, wHi
            ))
        }
        
        return sb.toString()
    }
    
    private fun handleSet(cmd: String): String {
        val parts = cmd.split(" ")
        if (parts.size < 3) return "Error: set <param> <value>\r\n"
        
        val param = parts[1]
        val value = parts.drop(2).joinToString(" ")
        
        when (param) {
            "tyre_width" -> value.toIntOrNull()?.let { tyreWidth = it }
            "tyre_aspect" -> value.toIntOrNull()?.let { tyreAspect = it }
            "tyre_rim" -> value.toIntOrNull()?.let { wheelDia = it }
            // ギア比は1000倍の整数値で受け取る（ファームウェアと同じ形式）
            "gear1" -> value.toIntOrNull()?.let { gear1 = it }
            "gear2" -> value.toIntOrNull()?.let { gear2 = it }
            "gear3" -> value.toIntOrNull()?.let { gear3 = it }
            "gear4" -> value.toIntOrNull()?.let { gear4 = it }
            "gear5" -> value.toIntOrNull()?.let { gear5 = it }
            "gear6" -> value.toIntOrNull()?.let { gear6 = it }
            "final" -> value.toIntOrNull()?.let { finalGear = it }
            "water_low" -> value.toIntOrNull()?.let { waterTempLow = it }
            "water_high" -> value.toIntOrNull()?.let { waterTempHigh = it }
            "fuel_warn" -> value.toIntOrNull()?.let { fuelWarn = it }
            "shift_rpm1" -> value.toIntOrNull()?.let { shiftRpm = shiftRpm.toMutableList().also { l -> l[0] = it } }
            "shift_rpm2" -> value.toIntOrNull()?.let { shiftRpm = shiftRpm.toMutableList().also { l -> l[1] = it } }
            "shift_rpm3" -> value.toIntOrNull()?.let { shiftRpm = shiftRpm.toMutableList().also { l -> l[2] = it } }
            "shift_rpm4" -> value.toIntOrNull()?.let { shiftRpm = shiftRpm.toMutableList().also { l -> l[3] = it } }
            "shift_rpm5" -> value.toIntOrNull()?.let { shiftRpm = shiftRpm.toMutableList().also { l -> l[4] = it } }
            else -> return "Unknown parameter: $param\r\n"
        }
        return "OK\r\n"
    }
    
    private fun generateHelpResponse(): String {
        return """
            |
            |=== FULLMONI-WIDE Parameter Console ===
            |Commands:
            |  help              - Show this help
            |  version           - Show firmware version
            |  list              - Show all parameters
            |  set <id> <value>  - Set parameter value
            |  save              - Save to EEPROM
            |  load              - Load from EEPROM
            |  default           - Reset to default values
            |  rtc               - Show current RTC time
            |  odo               - Show ODO value
            |  trip              - Show TRIP value
            |  exit              - Exit parameter mode
            |
            |CAN Configuration:
            |  can_list          - Show CAN configuration
            |  can_warning <0|1> - Enable/disable master warning
            |  can_sound <0|1>   - Enable/disable warning sound
            |  can_ch <n> <id> <en> - Set CAN channel (n=1-6)
            |  can_field <n> ... - Set CAN field
            |  can_save          - Save CAN config to EEPROM
            |
            |Parameter IDs:
            |  tyre_width, tyre_aspect, tyre_rim
            |  gear1-gear6, final
            |  water_low, water_high, fuel_warn
            |  shift_rpm1-shift_rpm5
            |
        """.trimMargin()
    }
    
    private fun handleCanWarning(cmd: String): String {
        val parts = cmd.split(" ")
        if (parts.size >= 2) {
            canWarningEnabled = parts[1] == "1"
        }
        return "OK\r\n"
    }
    
    private fun handleCanSound(cmd: String): String {
        val parts = cmd.split(" ")
        if (parts.size >= 2) {
            canSoundEnabled = parts[1] == "1"
        }
        return "OK\r\n"
    }
    
    private fun handleCanChannel(cmd: String): String {
        // can_ch <1-6> <hex_id> <0|1>
        val parts = cmd.split(" ")
        if (parts.size >= 4) {
            val chNum = parts[1].toIntOrNull()
            val id = parts[2]
            val enabled = parts[3] == "1"
            if (chNum != null && chNum in 1..6) {
                canChannels[chNum - 1] = Pair(id, enabled)
            }
        }
        return "OK\r\n"
    }
    
    private fun handleCanField(cmd: String): String {
        // can_field <idx> <ch> <byte> <len> <type> <endian> <var> <off> <mul> <div> <name> <unit> <dec> <wlo_en> <wlo> <whi_en> <whi>
        val parts = cmd.split(" ")
        if (parts.size >= 17) {
            val idx = parts[1].toIntOrNull()
            if (idx != null && idx in 1..16) {
                val enabled = parts[1].toIntOrNull()?.let { it > 0 } ?: false  // 実際はenabledは別途設定
                canFields[idx - 1] = CanFieldConfig(
                    enabled = true,  // フィールド設定時は有効とみなす
                    channel = parts[2].toIntOrNull() ?: 1,
                    startByte = parts[3].toIntOrNull() ?: 0,
                    byteCount = parts[4].toIntOrNull() ?: 1,
                    dataType = parts[5],
                    endian = parts[6],
                    targetVar = parts[7],
                    offset = parts[8].toFloatOrNull() ?: 0f,
                    multiplier = parts[9].toFloatOrNull() ?: 1f,
                    divisor = parts[10].toFloatOrNull() ?: 1f,
                    name = parts[11].removeSurrounding("\""),
                    unit = parts[12].removeSurrounding("\""),
                    decimals = parts[13].toIntOrNull() ?: 0,
                    warnLowEnabled = parts[14] == "1",
                    warnLow = parts[15].toFloatOrNull() ?: 0f,
                    warnHighEnabled = parts[16] == "1",
                    warnHigh = parts.getOrNull(17)?.toFloatOrNull() ?: 0f
                )
            }
        }
        return "OK\r\n"
    }
}
