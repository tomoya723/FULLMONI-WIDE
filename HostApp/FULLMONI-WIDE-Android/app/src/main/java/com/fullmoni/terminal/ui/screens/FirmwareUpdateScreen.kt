package com.fullmoni.terminal.ui.screens

import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.List
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import coil.compose.AsyncImage
import coil.request.ImageRequest
import android.util.Log
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.fullmoni.terminal.model.FirmwareVariant
import com.fullmoni.terminal.ui.components.ExpandableCard
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * ファームウェアアップデート画面
 * オンラインカタログからの選択、ファイル選択、進捗表示、ログ表示
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FirmwareUpdateScreen(viewModel: MainViewModel) {
    val context = LocalContext.current
    val isConnected by viewModel.isConnected.collectAsState()
    val scrollState = rememberScrollState()
    
    // TCP Bridge/Simulatorモードではアップデート不可
    val isRealUsbConnection = isConnected && !viewModel.isSimulationMode && !viewModel.isTcpBridgeMode

    val firmwareVersion by viewModel.firmwareVersion.collectAsState()
    val firmwarePath by viewModel.selectedFirmwarePath.collectAsState()
    val firmwareProgress by viewModel.firmwareProgress.collectAsState()
    val firmwareProgressText by viewModel.firmwareProgressText.collectAsState()
    val firmwareLog by viewModel.firmwareLog.collectAsState()
    val isUpdating by viewModel.isFirmwareUpdating.collectAsState()
    
    // Online Update
    val availableReleases by viewModel.onlineReleases.collectAsState()
    val currentManifest by viewModel.onlineManifest.collectAsState()
    val selectedReleaseTag by viewModel.selectedReleaseTag.collectAsState()
    val selectedVariant by viewModel.selectedVariant.collectAsState()
    val downloadProgress by viewModel.onlineDownloadProgress.collectAsState()
    val onlineStatus by viewModel.onlineStatusMessage.collectAsState()
    val isOnlineLoading by viewModel.isOnlineLoading.collectAsState()
    
    // Local file mode vs Online mode
    var useLocalFile by remember { mutableStateOf(false) }
    
    // Dropdown expanded state
    var releaseDropdownExpanded by remember { mutableStateOf(false) }

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
        
        // Mode Selection Tabs
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.Center
        ) {
            FilterChip(
                selected = !useLocalFile,
                onClick = { useLocalFile = false },
                label = { Text("Online Catalog") },
                leadingIcon = { Icon(Icons.Default.Cloud, contentDescription = null, modifier = Modifier.size(18.dp)) }
            )
            Spacer(modifier = Modifier.width(8.dp))
            FilterChip(
                selected = useLocalFile,
                onClick = { useLocalFile = true },
                label = { Text("Local File") },
                leadingIcon = { Icon(Icons.Default.FolderOpen, contentDescription = null, modifier = Modifier.size(18.dp)) }
            )
        }
        
        Spacer(modifier = Modifier.height(12.dp))
        
        if (useLocalFile) {
            // Local File Selection Card
            ExpandableCard(
                title = "Select Firmware File",
                subtitle = "Choose .bin file for update",
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
        } else {
            // Online Catalog Card
            ExpandableCard(
                title = "Online Catalog",
                subtitle = "Select firmware from GitHub Releases",
                icon = Icons.Default.Cloud,
                initialExpanded = true
            ) {
                Column(modifier = Modifier.padding(16.dp)) {
                    // Release Dropdown
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Release:", modifier = Modifier.width(80.dp))
                        
                        ExposedDropdownMenuBox(
                            expanded = releaseDropdownExpanded,
                            onExpandedChange = { releaseDropdownExpanded = it },
                            modifier = Modifier.weight(1f)
                        ) {
                            OutlinedTextField(
                                value = selectedReleaseTag.ifEmpty { "Select release..." },
                                onValueChange = {},
                                readOnly = true,
                                trailingIcon = {
                                    ExposedDropdownMenuDefaults.TrailingIcon(expanded = releaseDropdownExpanded)
                                },
                                modifier = Modifier.menuAnchor(),
                                singleLine = true,
                                colors = OutlinedTextFieldDefaults.colors(
                                    focusedBorderColor = FullmoniPrimary,
                                    unfocusedBorderColor = TextMuted
                                )
                            )
                            
                            ExposedDropdownMenu(
                                expanded = releaseDropdownExpanded,
                                onDismissRequest = { releaseDropdownExpanded = false }
                            ) {
                                availableReleases.forEach { tag ->
                                    DropdownMenuItem(
                                        text = { Text(tag) },
                                        onClick = {
                                            viewModel.selectReleaseTag(tag)
                                            releaseDropdownExpanded = false
                                        }
                                    )
                                }
                            }
                        }
                        
                        Spacer(modifier = Modifier.width(8.dp))
                        
                        Button(
                            onClick = { viewModel.fetchOnlineReleases() },
                            enabled = !isOnlineLoading && !isUpdating,
                            colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
                        ) {
                            if (isOnlineLoading) {
                                CircularProgressIndicator(
                                    modifier = Modifier.size(16.dp),
                                    strokeWidth = 2.dp,
                                    color = Color.White
                                )
                            } else {
                                Icon(imageVector = Icons.Default.Refresh, contentDescription = null)
                            }
                        }
                    }
                    
                    Spacer(modifier = Modifier.height(16.dp))
                    
                    // Status message
                    if (onlineStatus.isNotEmpty()) {
                        Text(
                            text = onlineStatus,
                            style = MaterialTheme.typography.bodySmall,
                            color = TextSecondary
                        )
                        Spacer(modifier = Modifier.height(8.dp))
                    }
                    
                    // Manifest info and variant selection
                    currentManifest?.let { manifest ->
                        // Release info
                        Card(
                            colors = CardDefaults.cardColors(containerColor = SurfaceLight),
                            modifier = Modifier.fillMaxWidth()
                        ) {
                            Column(modifier = Modifier.padding(12.dp)) {
                                Text(
                                    text = "Version: ${manifest.version}",
                                    style = MaterialTheme.typography.titleSmall,
                                    fontWeight = FontWeight.SemiBold
                                )
                                manifest.releaseDate?.let { date ->
                                    Text(
                                        text = "Date: $date",
                                        style = MaterialTheme.typography.bodySmall,
                                        color = TextSecondary
                                    )
                                }
                                manifest.releaseNotes?.let { notes ->
                                    Spacer(modifier = Modifier.height(4.dp))
                                    Text(
                                        text = notes,
                                        style = MaterialTheme.typography.bodySmall,
                                        color = TextSecondary,
                                        maxLines = 3
                                    )
                                }
                            }
                        }
                        
                        Spacer(modifier = Modifier.height(12.dp))
                        
                        // Firmware Variants
                        Text(
                            text = "Firmware Variants",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.SemiBold
                        )
                        Spacer(modifier = Modifier.height(8.dp))
                        
                        manifest.firmware?.variants?.forEach { variant ->
                            FirmwareVariantCard(
                                variant = variant,
                                isSelected = selectedVariant?.id == variant.id,
                                onClick = { viewModel.selectFirmwareVariant(variant) }
                            )
                            Spacer(modifier = Modifier.height(8.dp))
                        }
                    }
                    
                    // Download progress
                    if (downloadProgress in 1..99) {
                        Spacer(modifier = Modifier.height(8.dp))
                        LinearProgressIndicator(
                            progress = { downloadProgress / 100f },
                            modifier = Modifier.fillMaxWidth(),
                            color = FullmoniAccent
                        )
                        Text(
                            text = "Downloading: $downloadProgress%",
                            style = MaterialTheme.typography.bodySmall,
                            color = TextSecondary
                        )
                    }
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
            icon = Icons.AutoMirrored.Filled.List,
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

        // Start/Cancel Update Buttons
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            if (isUpdating) {
                // Cancel button
                Button(
                    onClick = { viewModel.cancelFirmwareUpdate() },
                    colors = ButtonDefaults.buttonColors(containerColor = WarnHigh)
                ) {
                    Icon(imageVector = Icons.Default.Cancel, contentDescription = null)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text("Cancel Update")
                }
            } else {
                // Start button
                val canStartUpdate = if (useLocalFile) {
                    isRealUsbConnection && firmwarePath.isNotEmpty()
                } else {
                    isRealUsbConnection && selectedVariant != null
                }
                
                Button(
                    onClick = { 
                        if (useLocalFile) {
                            viewModel.startFirmwareUpdate(context)
                        } else {
                            viewModel.downloadAndStartFirmwareUpdate(context)
                        }
                    },
                    enabled = canStartUpdate,
                    colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
                ) {
                    Icon(imageVector = Icons.Default.CloudUpload, contentDescription = null)
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(if (useLocalFile) "Start Firmware Update" else "Download & Update")
                }
            }
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
        } else if (viewModel.isSimulationMode || viewModel.isTcpBridgeMode) {
            Spacer(modifier = Modifier.height(8.dp))
            Text(
                text = "⚠ FW Update requires real USB connection\n(Not available via TCP Bridge or Simulator)",
                style = MaterialTheme.typography.bodySmall,
                color = WarnHigh,
                modifier = Modifier.align(Alignment.CenterHorizontally)
            )
        }
        
        // Note about .bin files
        Spacer(modifier = Modifier.height(8.dp))
        Text(
            text = "Note: Use .bin file (not .mot)",
            style = MaterialTheme.typography.bodySmall,
            color = TextMuted,
            modifier = Modifier.align(Alignment.CenterHorizontally)
        )
    }
}

/**
 * ファームウェアバリアント選択カード
 */
@Composable
fun FirmwareVariantCard(
    variant: FirmwareVariant,
    isSelected: Boolean,
    onClick: () -> Unit
) {
    val borderColor = if (isSelected) FullmoniPrimary else TextMuted.copy(alpha = 0.3f)
    val backgroundColor = if (isSelected) FullmoniPrimary.copy(alpha = 0.1f) else SurfaceLight
    
    Card(
        colors = CardDefaults.cardColors(containerColor = backgroundColor),
        modifier = Modifier
            .fillMaxWidth()
            .border(
                width = if (isSelected) 2.dp else 1.dp,
                color = borderColor,
                shape = RoundedCornerShape(8.dp)
            )
            .clip(RoundedCornerShape(8.dp))
            .clickable(onClick = onClick)
    ) {
        Column(
            modifier = Modifier.fillMaxWidth()
        ) {
            // Thumbnail image - constrained width
            if (!variant.thumbnailUrl.isNullOrEmpty()) {
                AsyncImage(
                    model = ImageRequest.Builder(LocalContext.current)
                        .data(variant.thumbnailUrl)
                        .crossfade(true)
                        .build(),
                    contentDescription = variant.name,
                    modifier = Modifier
                        .fillMaxWidth()
                        .height(120.dp)
                        .clip(RoundedCornerShape(topStart = 8.dp, topEnd = 8.dp))
                        .background(CardBackground),
                    contentScale = ContentScale.FillWidth
                )
            }
            
            // Content row with radio button and text
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(12.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                // Radio button indicator
                RadioButton(
                    selected = isSelected,
                    onClick = onClick,
                    colors = RadioButtonDefaults.colors(selectedColor = FullmoniPrimary)
                )
                
                Spacer(modifier = Modifier.width(8.dp))
                
                Column(modifier = Modifier.weight(1f)) {
                    Text(
                        text = variant.name,
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.SemiBold
                    )
                    variant.description?.let { desc ->
                        Text(
                            text = desc,
                            style = MaterialTheme.typography.bodySmall,
                            color = TextSecondary
                        )
                    }
                    Text(
                        text = "File: ${variant.file}",
                        style = MaterialTheme.typography.bodySmall,
                        color = TextMuted
                    )
                    Text(
                        text = "Size: ${variant.sizeFormatted ?: "${variant.size / 1024} KB"}",
                        style = MaterialTheme.typography.bodySmall,
                        color = TextMuted
                    )
                }
            }
        }
    }
}
