package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import com.fullmoni.terminal.ui.components.ExpandableCard
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * 設定画面
 * タイヤサイズ、ギア比、警告設定、シフトインジケータ設定
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SettingsScreen(viewModel: MainViewModel) {
    val isConnected by viewModel.isConnected.collectAsState()
    val scrollState = rememberScrollState()

    // パラメータの状態
    val tyreWidth by viewModel.tyreWidth.collectAsState()
    val tyreAspect by viewModel.tyreAspect.collectAsState()
    val wheelDia by viewModel.wheelDia.collectAsState()

    val gear1 by viewModel.gear1.collectAsState()
    val gear2 by viewModel.gear2.collectAsState()
    val gear3 by viewModel.gear3.collectAsState()
    val gear4 by viewModel.gear4.collectAsState()
    val gear5 by viewModel.gear5.collectAsState()
    val gear6 by viewModel.gear6.collectAsState()
    val finalGear by viewModel.finalGear.collectAsState()

    val waterTempLow by viewModel.waterTempLow.collectAsState()
    val waterTempHigh by viewModel.waterTempHigh.collectAsState()
    val fuelWarn by viewModel.fuelWarn.collectAsState()

    val shiftRpm1 by viewModel.shiftRpm1.collectAsState()
    val shiftRpm2 by viewModel.shiftRpm2.collectAsState()
    val shiftRpm3 by viewModel.shiftRpm3.collectAsState()
    val shiftRpm4 by viewModel.shiftRpm4.collectAsState()
    val shiftRpm5 by viewModel.shiftRpm5.collectAsState()

    val selectedGearPreset by viewModel.selectedGearPreset.collectAsState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp)
    ) {
        Text(
            text = "Vehicle Settings",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold,
            color = TextPrimary
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Tire Size Card
        ExpandableCard(
            title = "Tire Size",
            subtitle = "Tire diameter for speed calculation",
            icon = Icons.Default.Settings,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Width",
                        value = tyreWidth,
                        onValueChange = { viewModel.updateTyreWidth(it) },
                        suffix = "mm",
                        enabled = isConnected
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Aspect",
                        value = tyreAspect,
                        onValueChange = { viewModel.updateTyreAspect(it) },
                        suffix = "%",
                        enabled = isConnected
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Diameter",
                        value = wheelDia,
                        onValueChange = { viewModel.updateWheelDia(it) },
                        suffix = "inch",
                        enabled = isConnected
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Gear Ratio Card
        ExpandableCard(
            title = "Gear Ratio",
            subtitle = "Used for gear position calculation",
            icon = Icons.Default.Settings,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                // Preset Dropdown
                var presetExpanded by remember { mutableStateOf(false) }
                val presets = listOf("-- Select --", "NA/NB 5-speed", "NB 6-speed")
                
                ExposedDropdownMenuBox(
                    expanded = presetExpanded,
                    onExpandedChange = { presetExpanded = it }
                ) {
                    OutlinedTextField(
                        value = selectedGearPreset,
                        onValueChange = {},
                        readOnly = true,
                        label = { Text("Preset") },
                        trailingIcon = { ExposedDropdownMenuDefaults.TrailingIcon(expanded = presetExpanded) },
                        modifier = Modifier
                            .fillMaxWidth()
                            .menuAnchor(),
                        colors = OutlinedTextFieldDefaults.colors(
                            focusedBorderColor = FullmoniPrimary,
                            unfocusedBorderColor = TextMuted
                        )
                    )
                    ExposedDropdownMenu(
                        expanded = presetExpanded,
                        onDismissRequest = { presetExpanded = false }
                    ) {
                        presets.forEach { preset ->
                            DropdownMenuItem(
                                text = { Text(preset) },
                                onClick = {
                                    viewModel.selectGearPreset(preset)
                                    presetExpanded = false
                                }
                            )
                        }
                    }
                }

                Spacer(modifier = Modifier.height(12.dp))

                // Gear ratios
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "1st",
                        value = gear1,
                        onValueChange = { viewModel.updateGear1(it) },
                        isDecimal = true,
                        enabled = isConnected
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "2nd",
                        value = gear2,
                        onValueChange = { viewModel.updateGear2(it) },
                        isDecimal = true,
                        enabled = isConnected
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "3rd",
                        value = gear3,
                        onValueChange = { viewModel.updateGear3(it) },
                        isDecimal = true,
                        enabled = isConnected
                    )
                }

                Spacer(modifier = Modifier.height(8.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "4th",
                        value = gear4,
                        onValueChange = { viewModel.updateGear4(it) },
                        isDecimal = true,
                        enabled = isConnected
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "5th",
                        value = gear5,
                        onValueChange = { viewModel.updateGear5(it) },
                        isDecimal = true,
                        enabled = isConnected
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "6th",
                        value = gear6,
                        onValueChange = { viewModel.updateGear6(it) },
                        isDecimal = true,
                        enabled = isConnected
                    )
                }

                Spacer(modifier = Modifier.height(8.dp))

                SettingTextField(
                    modifier = Modifier.fillMaxWidth(),
                    label = "Final",
                    value = finalGear,
                    onValueChange = { viewModel.updateFinalGear(it) },
                    isDecimal = true,
                    enabled = isConnected,
                    labelColor = WarnFuel
                )
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Warning Settings Card
        ExpandableCard(
            title = "Warning Settings",
            subtitle = "Water temp & fuel warning thresholds",
            icon = Icons.Default.Warning,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Water Low",
                        value = waterTempLow,
                        onValueChange = { viewModel.updateWaterTempLow(it) },
                        suffix = "°C",
                        enabled = isConnected,
                        labelColor = WarnLow
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Water High",
                        value = waterTempHigh,
                        onValueChange = { viewModel.updateWaterTempHigh(it) },
                        suffix = "°C",
                        enabled = isConnected,
                        labelColor = WarnHigh
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Fuel Warn",
                        value = fuelWarn,
                        onValueChange = { viewModel.updateFuelWarn(it) },
                        suffix = "L",
                        enabled = isConnected,
                        labelColor = WarnFuel
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Shift Indicator Card
        ExpandableCard(
            title = "Shift Indicator",
            subtitle = "Shift lamp activation RPM",
            icon = Icons.Default.Timer,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Stage 1",
                        value = shiftRpm1,
                        onValueChange = { viewModel.updateShiftRpm1(it) },
                        suffix = "rpm",
                        enabled = isConnected,
                        labelColor = ShiftBlue
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Stage 2",
                        value = shiftRpm2,
                        onValueChange = { viewModel.updateShiftRpm2(it) },
                        suffix = "rpm",
                        enabled = isConnected,
                        labelColor = ShiftBlue
                    )
                }

                Spacer(modifier = Modifier.height(8.dp))

                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Stage 3",
                        value = shiftRpm3,
                        onValueChange = { viewModel.updateShiftRpm3(it) },
                        suffix = "rpm",
                        enabled = isConnected,
                        labelColor = ShiftGreen
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Stage 4",
                        value = shiftRpm4,
                        onValueChange = { viewModel.updateShiftRpm4(it) },
                        suffix = "rpm",
                        enabled = isConnected,
                        labelColor = ShiftRed
                    )
                    SettingTextField(
                        modifier = Modifier.weight(1f),
                        label = "Flash",
                        value = shiftRpm5,
                        onValueChange = { viewModel.updateShiftRpm5(it) },
                        suffix = "rpm",
                        enabled = isConnected,
                        labelColor = TextPrimary
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        // Action Buttons
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            OutlinedButton(
                onClick = { viewModel.loadParameters() },
                enabled = isConnected
            ) {
                Icon(Icons.Default.Refresh, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Reload")
            }

            Spacer(modifier = Modifier.width(8.dp))

            Button(
                onClick = { viewModel.saveParameters() },
                enabled = isConnected,
                colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
            ) {
                Icon(imageVector = Icons.Default.Check, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Save")
            }

            Spacer(modifier = Modifier.width(8.dp))

            OutlinedButton(
                onClick = { viewModel.defaultParameters() },
                enabled = isConnected
            ) {
                Icon(imageVector = Icons.Default.Refresh, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Default")
            }
        }
    }
}

@Composable
private fun SettingTextField(
    modifier: Modifier = Modifier,
    label: String,
    value: String,
    onValueChange: (String) -> Unit,
    suffix: String = "",
    isDecimal: Boolean = false,
    enabled: Boolean = true,
    labelColor: androidx.compose.ui.graphics.Color = TextPrimary
) {
    Column(modifier = modifier) {
        Text(
            text = label,
            style = MaterialTheme.typography.bodySmall,
            fontWeight = FontWeight.SemiBold,
            color = labelColor
        )
        Spacer(modifier = Modifier.height(4.dp))
        OutlinedTextField(
            value = value,
            onValueChange = onValueChange,
            modifier = Modifier.fillMaxWidth(),
            enabled = enabled,
            singleLine = true,
            keyboardOptions = KeyboardOptions(
                keyboardType = if (isDecimal) KeyboardType.Decimal else KeyboardType.Number
            ),
            suffix = if (suffix.isNotEmpty()) { { Text(suffix, color = TextMuted) } } else null,
            colors = OutlinedTextFieldDefaults.colors(
                focusedBorderColor = FullmoniPrimary,
                unfocusedBorderColor = TextMuted,
                disabledBorderColor = TextMuted.copy(alpha = 0.3f)
            )
        )
    }
}
