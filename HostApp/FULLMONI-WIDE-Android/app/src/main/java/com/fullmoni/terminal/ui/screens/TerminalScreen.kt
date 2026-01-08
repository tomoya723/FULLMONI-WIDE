package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import com.fullmoni.terminal.R
import com.fullmoni.terminal.ui.theme.TerminalBackground
import com.fullmoni.terminal.ui.theme.TerminalText
import com.fullmoni.terminal.ui.theme.TerminalTypography
import com.fullmoni.terminal.ui.theme.StatusConnected
import com.fullmoni.terminal.ui.theme.StatusDisconnected
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * ターミナル画面
 */
@Composable
fun TerminalScreen(viewModel: MainViewModel) {
    val isConnected by viewModel.isConnected.collectAsState()
    val receivedData by viewModel.receivedData.collectAsState()
    val availableDrivers by viewModel.availableDrivers.collectAsState()
    val selectedDriver by viewModel.selectedDriver.collectAsState()
    
    var commandText by remember { mutableStateOf("") }
    var expanded by remember { mutableStateOf(false) }
    
    val scrollState = rememberScrollState()
    
    // 新しいデータが来たら自動スクロール
    LaunchedEffect(receivedData) {
        scrollState.animateScrollTo(scrollState.maxValue)
    }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp)
    ) {
        // 接続ステータスとデバイス選択
        Card(
            modifier = Modifier.fillMaxWidth()
        ) {
            Column(
                modifier = Modifier.padding(16.dp)
            ) {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Box(
                            modifier = Modifier
                                .size(12.dp)
                                .background(
                                    color = if (isConnected) StatusConnected else StatusDisconnected,
                                    shape = MaterialTheme.shapes.small
                                )
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Text(
                            text = if (isConnected) stringResource(R.string.connected) 
                                   else stringResource(R.string.not_connected),
                            style = MaterialTheme.typography.bodyMedium
                        )
                    }
                    
                    Button(
                        onClick = {
                            if (isConnected) {
                                viewModel.disconnect()
                            } else {
                                viewModel.connect()
                            }
                        },
                        enabled = selectedDriver != null || isConnected
                    ) {
                        Text(
                            text = if (isConnected) stringResource(R.string.disconnect)
                                   else stringResource(R.string.connect)
                        )
                    }
                }
                
                if (!isConnected && availableDrivers.isNotEmpty()) {
                    Spacer(modifier = Modifier.height(8.dp))
                    
                    ExposedDropdownMenuBox(
                        expanded = expanded,
                        onExpandedChange = { expanded = it }
                    ) {
                        OutlinedTextField(
                            value = selectedDriver?.device?.deviceName ?: "デバイスを選択",
                            onValueChange = {},
                            readOnly = true,
                            trailingIcon = { ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded) },
                            modifier = Modifier
                                .menuAnchor()
                                .fillMaxWidth()
                        )
                        
                        ExposedDropdownMenu(
                            expanded = expanded,
                            onDismissRequest = { expanded = false }
                        ) {
                            availableDrivers.forEach { driver ->
                                DropdownMenuItem(
                                    text = { 
                                        Text("${driver.device.deviceName} (${driver.javaClass.simpleName})")
                                    },
                                    onClick = {
                                        viewModel.selectDriver(driver)
                                        expanded = false
                                    }
                                )
                            }
                        }
                    }
                }
                
                if (availableDrivers.isEmpty()) {
                    Spacer(modifier = Modifier.height(8.dp))
                    Text(
                        text = stringResource(R.string.no_device),
                        style = MaterialTheme.typography.bodySmall,
                        color = MaterialTheme.colorScheme.error
                    )
                }
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // クイックコマンドボタン
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            QuickCommandButton(stringResource(R.string.cmd_help)) { viewModel.sendCommand("help") }
            QuickCommandButton(stringResource(R.string.cmd_version)) { viewModel.sendCommand("version") }
            QuickCommandButton(stringResource(R.string.cmd_list)) { viewModel.sendCommand("list") }
            QuickCommandButton(stringResource(R.string.cmd_save)) { viewModel.sendCommand("save") }
            QuickCommandButton(stringResource(R.string.cmd_load)) { viewModel.sendCommand("load") }
            QuickCommandButton(stringResource(R.string.cmd_default)) { viewModel.sendCommand("default") }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // ターミナル表示エリア
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1f)
        ) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .background(TerminalBackground)
                    .padding(8.dp)
            ) {
                Text(
                    text = receivedData.ifEmpty { ">>> 接続してコマンドを送信してください" },
                    style = TerminalTypography,
                    color = TerminalText,
                    modifier = Modifier
                        .fillMaxSize()
                        .verticalScroll(scrollState)
                )
            }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // コマンド入力エリア
        Row(
            modifier = Modifier.fillMaxWidth(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            OutlinedTextField(
                value = commandText,
                onValueChange = { commandText = it },
                modifier = Modifier.weight(1f),
                placeholder = { Text(stringResource(R.string.command_hint)) },
                singleLine = true,
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Send),
                keyboardActions = KeyboardActions(
                    onSend = {
                        viewModel.sendCommand(commandText)
                        commandText = ""
                    }
                ),
                enabled = isConnected
            )
            
            Spacer(modifier = Modifier.width(8.dp))
            
            Button(
                onClick = {
                    viewModel.sendCommand(commandText)
                    commandText = ""
                },
                enabled = isConnected && commandText.isNotBlank()
            ) {
                Text(stringResource(R.string.send))
            }
            
            Spacer(modifier = Modifier.width(8.dp))
            
            OutlinedButton(
                onClick = { viewModel.clearTerminal() }
            ) {
                Text(stringResource(R.string.clear))
            }
        }
    }
}

@Composable
private fun QuickCommandButton(
    text: String,
    onClick: () -> Unit
) {
    FilledTonalButton(
        onClick = onClick,
        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 8.dp)
    ) {
        Text(text)
    }
}
