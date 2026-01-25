package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * ターミナル画面 (デバッグ/高度な操作用に残す)
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TerminalScreen(viewModel: MainViewModel) {
    val isConnected by viewModel.isConnected.collectAsState()
    val receivedData by viewModel.receivedData.collectAsState()
    
    var commandText by remember { mutableStateOf("") }
    
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
        // クイックコマンドボタン
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .horizontalScroll(rememberScrollState()),
            horizontalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            QuickCommandButton("help") { viewModel.sendCommand("help") }
            QuickCommandButton("version") { viewModel.sendCommand("version") }
            QuickCommandButton("list") { viewModel.sendCommand("list") }
            QuickCommandButton("save") { viewModel.sendCommand("save") }
            QuickCommandButton("load") { viewModel.sendCommand("load") }
            QuickCommandButton("default") { viewModel.sendCommand("default") }
        }
        
        Spacer(modifier = Modifier.height(16.dp))
        
        // ターミナル表示エリア
        Card(
            modifier = Modifier
                .fillMaxWidth()
                .weight(1f),
            colors = CardDefaults.cardColors(containerColor = CardBackground),
            shape = RoundedCornerShape(8.dp)
        ) {
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .background(TerminalBackground)
                    .padding(8.dp)
            ) {
                Text(
                    text = receivedData.ifEmpty { ">>> Connect and send commands" },
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
                placeholder = { Text("Enter command...", color = TextMuted) },
                singleLine = true,
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Send),
                keyboardActions = KeyboardActions(
                    onSend = {
                        viewModel.sendCommand(commandText)
                        commandText = ""
                    }
                ),
                enabled = isConnected,
                colors = OutlinedTextFieldDefaults.colors(
                    focusedBorderColor = FullmoniPrimary,
                    unfocusedBorderColor = TextMuted,
                    focusedTextColor = TextPrimary,
                    unfocusedTextColor = TextPrimary
                )
            )
            
            Spacer(modifier = Modifier.width(8.dp))
            
            Button(
                onClick = {
                    viewModel.sendCommand(commandText)
                    commandText = ""
                },
                enabled = isConnected && commandText.isNotBlank(),
                colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
            ) {
                Text("Send")
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
        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 8.dp),
        colors = ButtonDefaults.filledTonalButtonColors(
            containerColor = CardBackground,
            contentColor = TextPrimary
        )
    ) {
        Text(text)
    }
}
