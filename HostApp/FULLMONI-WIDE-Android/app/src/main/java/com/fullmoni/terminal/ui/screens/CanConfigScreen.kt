package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.fullmoni.terminal.ui.components.ExpandableCard
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.CanField
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * CAN設定画面
 * Master Warning、受信チャンネル、データフィールド定義
 */
@Composable
fun CanConfigScreen(viewModel: MainViewModel) {
    val isConnected by viewModel.isConnected.collectAsState()
    val scrollState = rememberScrollState()

    val warningEnabled by viewModel.canWarningEnabled.collectAsState()
    val soundEnabled by viewModel.canSoundEnabled.collectAsState()

    val canChannels by viewModel.canChannels.collectAsState()
    val canFields by viewModel.canFields.collectAsState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp)
    ) {
        Text(
            text = "CAN Communication Settings",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold,
            color = TextPrimary
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Master Warning Settings
        ExpandableCard(
            title = "Master Warning Settings",
            subtitle = "Enable warning display based on CAN values",
            icon = Icons.Default.Warning,
            initialExpanded = true
        ) {
            Row(
                modifier = Modifier.padding(16.dp),
                horizontalArrangement = Arrangement.spacedBy(24.dp)
            ) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Checkbox(
                        checked = warningEnabled,
                        onCheckedChange = { viewModel.updateCanWarningEnabled(it) },
                        enabled = isConnected,
                        colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary)
                    )
                    Spacer(modifier = Modifier.width(4.dp))
                    Text("Enable", color = TextPrimary)
                }
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Checkbox(
                        checked = soundEnabled,
                        onCheckedChange = { viewModel.updateCanSoundEnabled(it) },
                        enabled = isConnected,
                        colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary)
                    )
                    Spacer(modifier = Modifier.width(4.dp))
                    Text("Sound", color = TextPrimary)
                }
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Receive Channel Settings
        ExpandableCard(
            title = "Receive Channel Settings",
            subtitle = "Configure up to 6 CAN IDs",
            icon = Icons.Default.SettingsInputComponent,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                canChannels.chunked(3).forEach { rowChannels ->
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        rowChannels.forEach { channel ->
                            CanChannelInput(
                                modifier = Modifier.weight(1f),
                                channelNumber = channel.number,
                                canId = channel.id,
                                enabled = channel.enabled,
                                isConnected = isConnected,
                                onIdChange = { viewModel.updateCanChannelId(channel.number, it) },
                                onEnabledChange = { viewModel.updateCanChannelEnabled(channel.number, it) },
                                labelColor = if (channel.number <= 3) ShiftBlue else ShiftGreen
                            )
                        }
                    }
                    Spacer(modifier = Modifier.height(8.dp))
                }
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Data Field Definitions
        ExpandableCard(
            title = "Data Field Definitions (Max 16)",
            subtitle = "Define how to parse CAN data",
            icon = Icons.Default.TableChart,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(8.dp)) {
                // Table Header
                CanFieldHeader()
                
                Divider(color = TextMuted.copy(alpha = 0.3f))
                
                // Table Body
                canFields.forEachIndexed { index, field ->
                    CanFieldRow(
                        index = index,
                        field = field,
                        isConnected = isConnected,
                        onFieldChange = { viewModel.updateCanField(index, it) }
                    )
                    if (index < canFields.size - 1) {
                        Divider(color = TextMuted.copy(alpha = 0.1f))
                    }
                }
                
                Spacer(modifier = Modifier.height(8.dp))
                
                Text(
                    text = "Type: U=Unsigned, S=Signed | End: B=Big, L=Little",
                    style = MaterialTheme.typography.bodySmall,
                    color = TextMuted
                )
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        // Action Buttons
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.Center
        ) {
            OutlinedButton(
                onClick = { viewModel.loadCanConfig() },
                enabled = isConnected
            ) {
                Icon(imageVector = Icons.Default.Refresh, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Read")
            }

            Spacer(modifier = Modifier.width(8.dp))

            Button(
                onClick = { viewModel.saveCanConfig() },
                enabled = isConnected,
                colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
            ) {
                Icon(imageVector = Icons.Default.Check, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Save")
            }

            Spacer(modifier = Modifier.width(8.dp))

            OutlinedButton(
                onClick = { viewModel.defaultCanConfig() },
                enabled = isConnected
            ) {
                Icon(imageVector = Icons.Default.Restore, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Default")
            }
        }
    }
}

@Composable
private fun CanChannelInput(
    modifier: Modifier = Modifier,
    channelNumber: Int,
    canId: String,
    enabled: Boolean,
    isConnected: Boolean,
    onIdChange: (String) -> Unit,
    onEnabledChange: (Boolean) -> Unit,
    labelColor: androidx.compose.ui.graphics.Color
) {
    Row(
        modifier = modifier,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(
            text = "CH$channelNumber",
            style = MaterialTheme.typography.bodySmall,
            fontWeight = FontWeight.SemiBold,
            color = labelColor,
            modifier = Modifier.width(32.dp)
        )
        OutlinedTextField(
            value = canId,
            onValueChange = onIdChange,
            modifier = Modifier.weight(1f),
            enabled = isConnected,
            singleLine = true,
            placeholder = { Text("0x000", fontSize = 12.sp) },
            colors = OutlinedTextFieldDefaults.colors(
                focusedBorderColor = FullmoniPrimary,
                unfocusedBorderColor = TextMuted
            ),
            textStyle = MaterialTheme.typography.bodySmall
        )
        Checkbox(
            checked = enabled,
            onCheckedChange = onEnabledChange,
            enabled = isConnected,
            colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary),
            modifier = Modifier.size(32.dp)
        )
    }
}

@Composable
private fun CanFieldHeader() {
    val horizontalScroll = rememberScrollState()
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .horizontalScroll(horizontalScroll)
            .background(BackgroundMedium)
            .padding(vertical = 8.dp, horizontal = 4.dp)
    ) {
        listOf("#", "EN", "CH", "Byte", "Size", "Type", "End", "Var", "Off", "Mul", "Div", "Dec", "Name", "Unit", "WLo", "Lo", "WHi", "Hi")
            .forEach { header ->
                Text(
                    text = header,
                    style = MaterialTheme.typography.labelSmall,
                    fontWeight = FontWeight.SemiBold,
                    color = TextMuted,
                    modifier = Modifier.width(
                        when (header) {
                            "#" -> 24.dp
                            "EN", "WLo", "WHi" -> 32.dp
                            "CH", "End", "Dec" -> 32.dp
                            "Byte", "Size", "Type" -> 36.dp
                            "Name" -> 56.dp
                            else -> 44.dp
                        }
                    )
                )
            }
    }
}

@Composable
private fun CanFieldRow(
    index: Int,
    field: CanField,
    isConnected: Boolean,
    onFieldChange: (CanField) -> Unit
) {
    val horizontalScroll = rememberScrollState()
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .horizontalScroll(horizontalScroll)
            .padding(vertical = 4.dp, horizontal = 4.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        // Index
        Text(
            text = "${index + 1}",
            style = MaterialTheme.typography.bodySmall,
            color = TextMuted,
            modifier = Modifier.width(24.dp)
        )
        
        // Enabled checkbox
        Checkbox(
            checked = field.enabled,
            onCheckedChange = { onFieldChange(field.copy(enabled = it)) },
            enabled = isConnected,
            modifier = Modifier.width(32.dp).height(24.dp),
            colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary)
        )
        
        // Other fields as mini text fields
        MiniTextField(field.channel.toString(), { onFieldChange(field.copy(channel = it.toIntOrNull() ?: 0)) }, 32.dp, isConnected)
        MiniTextField(field.startByte.toString(), { onFieldChange(field.copy(startByte = it.toIntOrNull() ?: 0)) }, 36.dp, isConnected)
        MiniTextField(field.byteCount.toString(), { onFieldChange(field.copy(byteCount = it.toIntOrNull() ?: 1)) }, 36.dp, isConnected)
        MiniTextField(field.dataType, { onFieldChange(field.copy(dataType = it)) }, 36.dp, isConnected)
        MiniTextField(field.endian, { onFieldChange(field.copy(endian = it)) }, 32.dp, isConnected)
        MiniTextField(field.targetVar, { onFieldChange(field.copy(targetVar = it)) }, 44.dp, isConnected)
        MiniTextField(field.offset.toString(), { onFieldChange(field.copy(offset = it.toFloatOrNull() ?: 0f)) }, 44.dp, isConnected)
        MiniTextField(field.multiplier.toString(), { onFieldChange(field.copy(multiplier = it.toFloatOrNull() ?: 1f)) }, 44.dp, isConnected)
        MiniTextField(field.divisor.toString(), { onFieldChange(field.copy(divisor = it.toFloatOrNull() ?: 1f)) }, 44.dp, isConnected)
        MiniTextField(field.decimals.toString(), { onFieldChange(field.copy(decimals = it.toIntOrNull() ?: 0)) }, 32.dp, isConnected)
        MiniTextField(field.name, { onFieldChange(field.copy(name = it)) }, 56.dp, isConnected)
        MiniTextField(field.unit, { onFieldChange(field.copy(unit = it)) }, 44.dp, isConnected)
        
        // Warn Low checkbox
        Checkbox(
            checked = field.warnLowEnabled,
            onCheckedChange = { onFieldChange(field.copy(warnLowEnabled = it)) },
            enabled = isConnected,
            modifier = Modifier.width(32.dp).height(24.dp),
            colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary)
        )
        MiniTextField(field.warnLow.toString(), { onFieldChange(field.copy(warnLow = it.toFloatOrNull() ?: 0f)) }, 44.dp, isConnected)
        
        // Warn High checkbox
        Checkbox(
            checked = field.warnHighEnabled,
            onCheckedChange = { onFieldChange(field.copy(warnHighEnabled = it)) },
            enabled = isConnected,
            modifier = Modifier.width(32.dp).height(24.dp),
            colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary)
        )
        MiniTextField(field.warnHigh.toString(), { onFieldChange(field.copy(warnHigh = it.toFloatOrNull() ?: 0f)) }, 44.dp, isConnected)
    }
}

@Composable
private fun MiniTextField(
    value: String,
    onValueChange: (String) -> Unit,
    width: androidx.compose.ui.unit.Dp,
    enabled: Boolean
) {
    OutlinedTextField(
        value = value,
        onValueChange = onValueChange,
        modifier = Modifier
            .width(width)
            .height(40.dp)
            .padding(horizontal = 2.dp),
        enabled = enabled,
        singleLine = true,
        textStyle = MaterialTheme.typography.bodySmall.copy(fontSize = 10.sp),
        colors = OutlinedTextFieldDefaults.colors(
            focusedBorderColor = FullmoniPrimary,
            unfocusedBorderColor = TextMuted.copy(alpha = 0.3f)
        )
    )
}
