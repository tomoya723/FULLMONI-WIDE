package com.fullmoni.terminal.ui.screens

import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.fullmoni.terminal.ui.components.ExpandableCard
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * ファームウェアアップデート画面
 * ファイル選択、進捗表示、ログ表示
 */
@Composable
fun FirmwareUpdateScreen(viewModel: MainViewModel) {
    val context = LocalContext.current
    val isConnected by viewModel.isConnected.collectAsState()
    val scrollState = rememberScrollState()

    val firmwareVersion by viewModel.firmwareVersion.collectAsState()
    val firmwarePath by viewModel.selectedFirmwarePath.collectAsState()
    val firmwareProgress by viewModel.firmwareProgress.collectAsState()
    val firmwareProgressText by viewModel.firmwareProgressText.collectAsState()
    val firmwareLog by viewModel.firmwareLog.collectAsState()
    val isUpdating by viewModel.isFirmwareUpdating.collectAsState()

    // ファイル選択
    val filePickerLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.GetContent()
    ) { uri: Uri? ->
        uri?.let { viewModel.selectFirmwareFile(context, it) }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp)
    ) {
        // Header
        Row(verticalAlignment = Alignment.CenterVertically) {
            Box(
                modifier = Modifier
                    .width(4.dp)
                    .height(32.dp)
                    .background(FullmoniAccent, RoundedCornerShape(2.dp))
            )
            Spacer(modifier = Modifier.width(12.dp))
            Text(
                text = "Firmware Update",
                style = MaterialTheme.typography.headlineSmall,
                fontWeight = FontWeight.SemiBold,
                color = TextPrimary
            )
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Current Version Card
        ExpandableCard(
            title = "Current Version",
            subtitle = "Connected device firmware version",
            icon = Icons.Default.Info,
            initialExpanded = true
        ) {
            Text(
                text = "Firmware: v${firmwareVersion ?: "Not connected"}",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.SemiBold,
                color = StatusConnected,
                modifier = Modifier.padding(16.dp)
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        // File Selection Card
        ExpandableCard(
            title = "Select Firmware File",
            subtitle = "Choose .mot file for update",
            icon = Icons.Default.FolderOpen,
            initialExpanded = true
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                OutlinedTextField(
                    value = firmwarePath,
                    onValueChange = {},
                    modifier = Modifier.weight(1f),
                    readOnly = true,
                    placeholder = { Text("No file selected") },
                    colors = OutlinedTextFieldDefaults.colors(
                        focusedBorderColor = FullmoniPrimary,
                        unfocusedBorderColor = TextMuted
                    ),
                    singleLine = true
                )
                Spacer(modifier = Modifier.width(8.dp))
                Button(
                    onClick = { filePickerLauncher.launch("*/*") },
                    enabled = !isUpdating,
                    colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
                ) {
                    Icon(imageVector = Icons.Default.Search, contentDescription = null)
                    Spacer(modifier = Modifier.width(4.dp))
                    Text("Browse...")
                }
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Transfer Progress Card
        ExpandableCard(
            title = "Transfer Progress",
            subtitle = "Upload status",
            icon = Icons.Default.CloudUpload,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                LinearProgressIndicator(
                    progress = { firmwareProgress / 100f },
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(24.dp),
                    color = FullmoniPrimary,
                    trackColor = TextMuted.copy(alpha = 0.2f)
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = firmwareProgressText,
                    style = MaterialTheme.typography.bodyMedium,
                    color = TextSecondary
                )
            }
        }

        Spacer(modifier = Modifier.height(12.dp))

        // Communication Log Card
        ExpandableCard(
            title = "Communication Log",
            subtitle = "Transfer details",
            icon = Icons.Default.List,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    OutlinedButton(
                        onClick = { viewModel.clearFirmwareLog() },
                        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 4.dp)
                    ) {
                        Icon(imageVector = Icons.Default.Delete, contentDescription = null, modifier = Modifier.size(16.dp))
                        Spacer(modifier = Modifier.width(4.dp))
                        Text("Clear", fontSize = 12.sp)
                    }
                    OutlinedButton(
                        onClick = { viewModel.copyFirmwareLog(context) },
                        contentPadding = PaddingValues(horizontal = 12.dp, vertical = 4.dp)
                    ) {
                        Icon(imageVector = Icons.Default.ContentCopy, contentDescription = null, modifier = Modifier.size(16.dp))
                        Spacer(modifier = Modifier.width(4.dp))
                        Text("Copy", fontSize = 12.sp)
                    }
                }

                Spacer(modifier = Modifier.height(8.dp))

                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(180.dp)
                        .background(TerminalBackground, RoundedCornerShape(4.dp))
                        .padding(8.dp)
                ) {
                    val logScrollState = rememberScrollState()
                    
                    // Auto-scroll to bottom when log changes
                    LaunchedEffect(firmwareLog) {
                        logScrollState.animateScrollTo(logScrollState.maxValue)
                    }
                    
                    Text(
                        text = firmwareLog.ifEmpty { "> Ready for firmware update" },
                        style = MaterialTheme.typography.bodySmall.copy(
                            fontFamily = FontFamily.Monospace,
                            fontSize = 11.sp
                        ),
                        color = TerminalText,
                        modifier = Modifier.verticalScroll(logScrollState)
                    )
                }
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        // Start Update Button
        Button(
            onClick = { viewModel.startFirmwareUpdate() },
            modifier = Modifier.align(Alignment.CenterHorizontally),
            enabled = isConnected && firmwarePath.isNotEmpty() && !isUpdating,
            colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
        ) {
            Icon(imageVector = Icons.Default.CloudUpload, contentDescription = null)
            Spacer(modifier = Modifier.width(8.dp))
            Text("Start Firmware Update")
        }

        // Warning text
        if (!isConnected) {
            Spacer(modifier = Modifier.height(8.dp))
            Text(
                text = "⚠ Connect to device first",
                style = MaterialTheme.typography.bodySmall,
                color = WarnFuel,
                modifier = Modifier.align(Alignment.CenterHorizontally)
            )
        }
    }
}
