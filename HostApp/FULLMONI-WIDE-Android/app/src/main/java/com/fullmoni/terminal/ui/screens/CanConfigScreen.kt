package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
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
    
    // 編集ダイアログ用の状態
    var editingFieldIndex by remember { mutableStateOf<Int?>(null) }
    var editingField by remember { mutableStateOf<CanField?>(null) }
    
    // 確認ダイアログ用の状態
    var showSaveConfirmDialog by remember { mutableStateOf(false) }
    var showDefaultConfirmDialog by remember { mutableStateOf(false) }

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

        // Data Field Definitions - カード形式で表示
        ExpandableCard(
            title = "Data Field Definitions (Max 16)",
            subtitle = "Tap a field to edit",
            icon = Icons.Default.TableChart,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(8.dp)) {
                Text(
                    text = "Type: U=Unsigned, S=Signed | Endian: B=Big, L=Little",
                    style = MaterialTheme.typography.bodySmall,
                    color = TextMuted,
                    modifier = Modifier.padding(bottom = 8.dp)
                )
                
                // フィールドリスト（コンパクトカード形式）
                canFields.forEachIndexed { index, field ->
                    CanFieldCard(
                        index = index,
                        field = field,
                        isConnected = isConnected,
                        onClick = {
                            editingFieldIndex = index
                            editingField = field
                        }
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                }
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
                onClick = { showSaveConfirmDialog = true },
                enabled = isConnected,
                colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
            ) {
                Icon(imageVector = Icons.Default.Check, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Save")
            }

            Spacer(modifier = Modifier.width(8.dp))

            OutlinedButton(
                onClick = { showDefaultConfirmDialog = true },
                enabled = isConnected
            ) {
                Icon(imageVector = Icons.Default.Restore, contentDescription = null)
                Spacer(modifier = Modifier.width(8.dp))
                Text("Default")
            }
        }
    }
    
    // フィールド編集ダイアログ
    if (editingFieldIndex != null && editingField != null) {
        CanFieldEditDialog(
            fieldIndex = editingFieldIndex!!,
            field = editingField!!,
            onDismiss = {
                editingFieldIndex = null
                editingField = null
            },
            onSave = { updatedField ->
                viewModel.updateCanField(editingFieldIndex!!, updatedField)
                editingFieldIndex = null
                editingField = null
            }
        )
    }
    
    // Save確認ダイアログ
    if (showSaveConfirmDialog) {
        AlertDialog(
            onDismissRequest = { showSaveConfirmDialog = false },
            title = { Text("Save CAN Config") },
            text = { Text("Save current CAN configuration to device EEPROM?") },
            confirmButton = {
                Button(
                    onClick = {
                        showSaveConfirmDialog = false
                        viewModel.saveCanConfig()
                    },
                    colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
                ) {
                    Text("Save")
                }
            },
            dismissButton = {
                OutlinedButton(onClick = { showSaveConfirmDialog = false }) {
                    Text("Cancel")
                }
            }
        )
    }
    
    // Default確認ダイアログ
    if (showDefaultConfirmDialog) {
        AlertDialog(
            onDismissRequest = { showDefaultConfirmDialog = false },
            title = { Text("Reset to Default") },
            text = { Text("Reset CAN configuration to factory default (MoTeC M100)?\n\nNote: This will NOT save to EEPROM automatically.") },
            confirmButton = {
                Button(
                    onClick = {
                        showDefaultConfirmDialog = false
                        viewModel.defaultCanConfig()
                    },
                    colors = ButtonDefaults.buttonColors(containerColor = WarnHigh)
                ) {
                    Text("Reset")
                }
            },
            dismissButton = {
                OutlinedButton(onClick = { showDefaultConfirmDialog = false }) {
                    Text("Cancel")
                }
            }
        )
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

/**
 * フィールド情報を表示するコンパクトなカード
 */
@Composable
private fun CanFieldCard(
    index: Int,
    field: CanField,
    isConnected: Boolean,
    onClick: () -> Unit
) {
    val backgroundColor = if (field.enabled) CardBackground else CardBackground.copy(alpha = 0.5f)
    val textColor = if (field.enabled) TextPrimary else TextMuted
    
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(8.dp))
            .clickable(enabled = isConnected, onClick = onClick),
        color = backgroundColor
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(12.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            // インデックスとステータス
            Box(
                modifier = Modifier
                    .size(28.dp)
                    .clip(RoundedCornerShape(4.dp))
                    .background(if (field.enabled) FullmoniPrimary else TextMuted.copy(alpha = 0.3f)),
                contentAlignment = Alignment.Center
            ) {
                Text(
                    text = "${index + 1}",
                    style = MaterialTheme.typography.labelMedium,
                    fontWeight = FontWeight.Bold,
                    color = TextPrimary
                )
            }
            
            Spacer(modifier = Modifier.width(12.dp))
            
            // フィールド情報
            Column(modifier = Modifier.weight(1f)) {
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Text(
                        text = field.name.ifEmpty { "Field ${index + 1}" },
                        style = MaterialTheme.typography.bodyMedium,
                        fontWeight = FontWeight.SemiBold,
                        color = textColor
                    )
                    if (field.unit.isNotEmpty()) {
                        Spacer(modifier = Modifier.width(4.dp))
                        Text(
                            text = "(${field.unit})",
                            style = MaterialTheme.typography.bodySmall,
                            color = TextMuted
                        )
                    }
                }
                Text(
                    text = "CH${field.channel} | Byte ${field.startByte} | ${field.byteCount}B | ${field.dataType}/${field.endian} | ${field.targetVar}",
                    style = MaterialTheme.typography.bodySmall,
                    color = TextMuted
                )
            }
            
            // 編集アイコン
            Icon(
                imageVector = Icons.Default.Edit,
                contentDescription = "Edit",
                tint = TextMuted,
                modifier = Modifier.size(20.dp)
            )
        }
    }
}

/**
 * フィールド編集ダイアログ
 */
@Composable
private fun CanFieldEditDialog(
    fieldIndex: Int,
    field: CanField,
    onDismiss: () -> Unit,
    onSave: (CanField) -> Unit
) {
    var editField by remember { mutableStateOf(field) }
    val scrollState = rememberScrollState()
    
    Dialog(onDismissRequest = onDismiss) {
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            shape = RoundedCornerShape(16.dp),
            colors = CardDefaults.cardColors(containerColor = SidebarBackground)
        ) {
            Column(
                modifier = Modifier
                    .verticalScroll(scrollState)
                    .padding(20.dp)
            ) {
                // タイトル
                Text(
                    text = "Edit Field ${fieldIndex + 1}",
                    style = MaterialTheme.typography.titleLarge,
                    fontWeight = FontWeight.Bold,
                    color = TextPrimary
                )
                
                Spacer(modifier = Modifier.height(16.dp))
                
                // Enabled
                Row(verticalAlignment = Alignment.CenterVertically) {
                    Checkbox(
                        checked = editField.enabled,
                        onCheckedChange = { editField = editField.copy(enabled = it) },
                        colors = CheckboxDefaults.colors(checkedColor = FullmoniPrimary)
                    )
                    Text("Enabled", color = TextPrimary)
                }
                
                Spacer(modifier = Modifier.height(12.dp))
                
                // 基本設定（2列）
                Row(modifier = Modifier.fillMaxWidth()) {
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Channel",
                        value = editField.channel.toString(),
                        onValueChange = { editField = editField.copy(channel = it.toIntOrNull() ?: 0) },
                        keyboardType = KeyboardType.Number
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Start Byte",
                        value = editField.startByte.toString(),
                        onValueChange = { editField = editField.copy(startByte = it.toIntOrNull() ?: 0) },
                        keyboardType = KeyboardType.Number
                    )
                }
                
                Spacer(modifier = Modifier.height(8.dp))
                
                Row(modifier = Modifier.fillMaxWidth()) {
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Byte Count",
                        value = editField.byteCount.toString(),
                        onValueChange = { editField = editField.copy(byteCount = it.toIntOrNull() ?: 1) },
                        keyboardType = KeyboardType.Number
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Type (U/S)",
                        value = editField.dataType,
                        onValueChange = { editField = editField.copy(dataType = it.uppercase().take(1)) }
                    )
                }
                
                Spacer(modifier = Modifier.height(8.dp))
                
                Row(modifier = Modifier.fillMaxWidth()) {
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Endian (B/L)",
                        value = editField.endian,
                        onValueChange = { editField = editField.copy(endian = it.uppercase().take(1)) }
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Target Var",
                        value = editField.targetVar,
                        onValueChange = { editField = editField.copy(targetVar = it.uppercase()) }
                    )
                }
                
                Spacer(modifier = Modifier.height(12.dp))
                
                // 変換設定
                Text(
                    text = "Conversion",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold,
                    color = TextSecondary
                )
                
                Spacer(modifier = Modifier.height(8.dp))
                
                Row(modifier = Modifier.fillMaxWidth()) {
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Offset",
                        value = editField.offset.toString(),
                        onValueChange = { editField = editField.copy(offset = it.toFloatOrNull() ?: 0f) },
                        keyboardType = KeyboardType.Decimal
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Multiplier",
                        value = editField.multiplier.toString(),
                        onValueChange = { editField = editField.copy(multiplier = it.toFloatOrNull() ?: 1f) },
                        keyboardType = KeyboardType.Decimal
                    )
                }
                
                Spacer(modifier = Modifier.height(8.dp))
                
                Row(modifier = Modifier.fillMaxWidth()) {
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Divisor",
                        value = editField.divisor.toString(),
                        onValueChange = { editField = editField.copy(divisor = it.toFloatOrNull() ?: 1f) },
                        keyboardType = KeyboardType.Decimal
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Decimals",
                        value = editField.decimals.toString(),
                        onValueChange = { editField = editField.copy(decimals = it.toIntOrNull() ?: 0) },
                        keyboardType = KeyboardType.Number
                    )
                }
                
                Spacer(modifier = Modifier.height(12.dp))
                
                // 表示設定
                Text(
                    text = "Display",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold,
                    color = TextSecondary
                )
                
                Spacer(modifier = Modifier.height(8.dp))
                
                Row(modifier = Modifier.fillMaxWidth()) {
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Name",
                        value = editField.name,
                        onValueChange = { editField = editField.copy(name = it) }
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Unit",
                        value = editField.unit,
                        onValueChange = { editField = editField.copy(unit = it) }
                    )
                }
                
                Spacer(modifier = Modifier.height(12.dp))
                
                // 警告設定
                Text(
                    text = "Warnings",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold,
                    color = TextSecondary
                )
                
                Spacer(modifier = Modifier.height(8.dp))
                
                // Warning Low
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Checkbox(
                        checked = editField.warnLowEnabled,
                        onCheckedChange = { editField = editField.copy(warnLowEnabled = it) },
                        colors = CheckboxDefaults.colors(checkedColor = WarnLow)
                    )
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "Low Threshold",
                        value = editField.warnLow.toString(),
                        onValueChange = { editField = editField.copy(warnLow = it.toFloatOrNull() ?: 0f) },
                        keyboardType = KeyboardType.Decimal
                    )
                }
                
                // Warning High
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Checkbox(
                        checked = editField.warnHighEnabled,
                        onCheckedChange = { editField = editField.copy(warnHighEnabled = it) },
                        colors = CheckboxDefaults.colors(checkedColor = WarnHigh)
                    )
                    EditTextField(
                        modifier = Modifier.weight(1f),
                        label = "High Threshold",
                        value = editField.warnHigh.toString(),
                        onValueChange = { editField = editField.copy(warnHigh = it.toFloatOrNull() ?: 0f) },
                        keyboardType = KeyboardType.Decimal
                    )
                }
                
                Spacer(modifier = Modifier.height(20.dp))
                
                // ボタン
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.End
                ) {
                    TextButton(onClick = onDismiss) {
                        Text("Cancel", color = TextMuted)
                    }
                    Spacer(modifier = Modifier.width(8.dp))
                    Button(
                        onClick = { onSave(editField) },
                        colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
                    ) {
                        Text("Save")
                    }
                }
            }
        }
    }
}

@Composable
private fun EditTextField(
    modifier: Modifier = Modifier,
    label: String,
    value: String,
    onValueChange: (String) -> Unit,
    keyboardType: KeyboardType = KeyboardType.Text
) {
    OutlinedTextField(
        value = value,
        onValueChange = onValueChange,
        modifier = modifier.fillMaxWidth(),
        label = { Text(label) },
        singleLine = true,
        keyboardOptions = KeyboardOptions(keyboardType = keyboardType),
        colors = OutlinedTextFieldDefaults.colors(
            focusedBorderColor = FullmoniPrimary,
            unfocusedBorderColor = TextMuted,
            focusedLabelColor = FullmoniPrimary,
            unfocusedLabelColor = TextMuted
        ),
        textStyle = MaterialTheme.typography.bodyMedium.copy(color = TextPrimary)
    )
}
