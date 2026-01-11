package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * パラメータ設定画面
 */
@Composable
fun ParameterScreen(viewModel: MainViewModel) {
    val isConnected by viewModel.isConnected.collectAsState()
    val scrollState = rememberScrollState()
    
    // パラメータの状態
    var tyreWidth by remember { mutableStateOf("") }
    var tyreAspect by remember { mutableStateOf("") }
    var tyreRim by remember { mutableStateOf("") }
    
    var gear1 by remember { mutableStateOf("") }
    var gear2 by remember { mutableStateOf("") }
    var gear3 by remember { mutableStateOf("") }
    var gear4 by remember { mutableStateOf("") }
    var gear5 by remember { mutableStateOf("") }
    var gear6 by remember { mutableStateOf("") }
    var finalGear by remember { mutableStateOf("") }
    
    var waterLow by remember { mutableStateOf("") }
    var waterHigh by remember { mutableStateOf("") }
    var fuelWarn by remember { mutableStateOf("") }
    
    var shiftRpm1 by remember { mutableStateOf("") }
    var shiftRpm2 by remember { mutableStateOf("") }
    var shiftRpm3 by remember { mutableStateOf("") }
    var shiftRpm4 by remember { mutableStateOf("") }
    var shiftRpm5 by remember { mutableStateOf("") }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
            .verticalScroll(scrollState)
    ) {
        // タイヤサイズ設定
        SectionCard(title = "タイヤサイズ") {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                ParameterTextField(
                    value = tyreWidth,
                    onValueChange = { tyreWidth = it },
                    label = "幅 (mm)",
                    modifier = Modifier.weight(1f)
                )
                ParameterTextField(
                    value = tyreAspect,
                    onValueChange = { tyreAspect = it },
                    label = "扁平率 (%)",
                    modifier = Modifier.weight(1f)
                )
                ParameterTextField(
                    value = tyreRim,
                    onValueChange = { tyreRim = it },
                    label = "リム径 (inch)",
                    modifier = Modifier.weight(1f)
                )
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // ギア比設定
        SectionCard(title = "ギア比") {
            Column(
                verticalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    ParameterTextField(
                        value = gear1,
                        onValueChange = { gear1 = it },
                        label = "1速",
                        modifier = Modifier.weight(1f),
                        isDecimal = true
                    )
                    ParameterTextField(
                        value = gear2,
                        onValueChange = { gear2 = it },
                        label = "2速",
                        modifier = Modifier.weight(1f),
                        isDecimal = true
                    )
                    ParameterTextField(
                        value = gear3,
                        onValueChange = { gear3 = it },
                        label = "3速",
                        modifier = Modifier.weight(1f),
                        isDecimal = true
                    )
                }
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    ParameterTextField(
                        value = gear4,
                        onValueChange = { gear4 = it },
                        label = "4速",
                        modifier = Modifier.weight(1f),
                        isDecimal = true
                    )
                    ParameterTextField(
                        value = gear5,
                        onValueChange = { gear5 = it },
                        label = "5速",
                        modifier = Modifier.weight(1f),
                        isDecimal = true
                    )
                    ParameterTextField(
                        value = gear6,
                        onValueChange = { gear6 = it },
                        label = "6速",
                        modifier = Modifier.weight(1f),
                        isDecimal = true
                    )
                }
                ParameterTextField(
                    value = finalGear,
                    onValueChange = { finalGear = it },
                    label = "ファイナル",
                    modifier = Modifier.fillMaxWidth(),
                    isDecimal = true
                )
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // 警告設定
        SectionCard(title = "警告設定") {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                ParameterTextField(
                    value = waterLow,
                    onValueChange = { waterLow = it },
                    label = "水温低温 (℃)",
                    modifier = Modifier.weight(1f)
                )
                ParameterTextField(
                    value = waterHigh,
                    onValueChange = { waterHigh = it },
                    label = "水温高温 (℃)",
                    modifier = Modifier.weight(1f)
                )
                ParameterTextField(
                    value = fuelWarn,
                    onValueChange = { fuelWarn = it },
                    label = "燃料警告",
                    modifier = Modifier.weight(1f)
                )
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // シフトインジケータ設定
        SectionCard(title = "シフトインジケータ (RPM)") {
            Column(
                verticalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    ParameterTextField(
                        value = shiftRpm1,
                        onValueChange = { shiftRpm1 = it },
                        label = "青2灯",
                        modifier = Modifier.weight(1f)
                    )
                    ParameterTextField(
                        value = shiftRpm2,
                        onValueChange = { shiftRpm2 = it },
                        label = "青4灯",
                        modifier = Modifier.weight(1f)
                    )
                }
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    ParameterTextField(
                        value = shiftRpm3,
                        onValueChange = { shiftRpm3 = it },
                        label = "緑6灯",
                        modifier = Modifier.weight(1f)
                    )
                    ParameterTextField(
                        value = shiftRpm4,
                        onValueChange = { shiftRpm4 = it },
                        label = "赤8灯",
                        modifier = Modifier.weight(1f)
                    )
                    ParameterTextField(
                        value = shiftRpm5,
                        onValueChange = { shiftRpm5 = it },
                        label = "白点滅",
                        modifier = Modifier.weight(1f)
                    )
                }
            }
        }
        
        Spacer(modifier = Modifier.height(24.dp))
        
        // 操作ボタン
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            OutlinedButton(
                onClick = {
                    // ファームウェアから読み込み
                    viewModel.sendCommand("list")
                },
                modifier = Modifier.weight(1f),
                enabled = isConnected
            ) {
                Text("読み込み")
            }
            
            Button(
                onClick = {
                    // パラメータを送信
                    if (tyreWidth.isNotBlank()) viewModel.sendCommand("set tyre_width $tyreWidth")
                    if (tyreAspect.isNotBlank()) viewModel.sendCommand("set tyre_aspect $tyreAspect")
                    if (tyreRim.isNotBlank()) viewModel.sendCommand("set tyre_rim $tyreRim")
                    if (gear1.isNotBlank()) viewModel.sendCommand("set gear1 $gear1")
                    if (gear2.isNotBlank()) viewModel.sendCommand("set gear2 $gear2")
                    if (gear3.isNotBlank()) viewModel.sendCommand("set gear3 $gear3")
                    if (gear4.isNotBlank()) viewModel.sendCommand("set gear4 $gear4")
                    if (gear5.isNotBlank()) viewModel.sendCommand("set gear5 $gear5")
                    if (gear6.isNotBlank()) viewModel.sendCommand("set gear6 $gear6")
                    if (finalGear.isNotBlank()) viewModel.sendCommand("set final $finalGear")
                    if (waterLow.isNotBlank()) viewModel.sendCommand("set water_low $waterLow")
                    if (waterHigh.isNotBlank()) viewModel.sendCommand("set water_high $waterHigh")
                    if (fuelWarn.isNotBlank()) viewModel.sendCommand("set fuel_warn $fuelWarn")
                    if (shiftRpm1.isNotBlank()) viewModel.sendCommand("set shift_rpm1 $shiftRpm1")
                    if (shiftRpm2.isNotBlank()) viewModel.sendCommand("set shift_rpm2 $shiftRpm2")
                    if (shiftRpm3.isNotBlank()) viewModel.sendCommand("set shift_rpm3 $shiftRpm3")
                    if (shiftRpm4.isNotBlank()) viewModel.sendCommand("set shift_rpm4 $shiftRpm4")
                    if (shiftRpm5.isNotBlank()) viewModel.sendCommand("set shift_rpm5 $shiftRpm5")
                    viewModel.sendCommand("save")
                },
                modifier = Modifier.weight(1f),
                enabled = isConnected
            ) {
                Text("保存")
            }
        }
    }
}

@Composable
private fun SectionCard(
    title: String,
    content: @Composable ColumnScope.() -> Unit
) {
    Card(
        modifier = Modifier.fillMaxWidth()
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = title,
                style = MaterialTheme.typography.titleMedium
            )
            Spacer(modifier = Modifier.height(12.dp))
            content()
        }
    }
}

@Composable
private fun ParameterTextField(
    value: String,
    onValueChange: (String) -> Unit,
    label: String,
    modifier: Modifier = Modifier,
    isDecimal: Boolean = false
) {
    OutlinedTextField(
        value = value,
        onValueChange = onValueChange,
        label = { Text(label) },
        modifier = modifier,
        singleLine = true,
        keyboardOptions = KeyboardOptions(
            keyboardType = if (isDecimal) KeyboardType.Decimal else KeyboardType.Number
        )
    )
}
