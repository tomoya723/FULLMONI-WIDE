package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel
import kotlinx.coroutines.launch

/**
 * メイン画面
 * Navigation DrawerでWindows版と同様のナビゲーション構造を実現
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainScreen(viewModel: MainViewModel) {
    val drawerState = rememberDrawerState(initialValue = DrawerValue.Closed)
    val scope = rememberCoroutineScope()

    var currentScreen by remember { mutableStateOf("home") }

    val isConnected by viewModel.isConnected.collectAsState()
    val firmwareVersion by viewModel.firmwareVersion.collectAsState()
    val availableDrivers by viewModel.availableDrivers.collectAsState()
    val selectedDriver by viewModel.selectedDriver.collectAsState()
    val errorMessage by viewModel.errorMessage.collectAsState()
    val isBootloaderMode by viewModel.isBootloaderMode.collectAsState()

    // エラーダイアログ（FW Update中/成功後のUSBエラーはViewModel側でフィルタリング済み）
    errorMessage?.let { error ->
        AlertDialog(
            onDismissRequest = { viewModel.clearError() },
            title = { Text("Error", color = TextPrimary) },
            text = { Text(error, color = TextSecondary) },
            confirmButton = {
                TextButton(onClick = { viewModel.clearError() }) {
                    Text("OK", color = FullmoniPrimary)
                }
            },
            containerColor = CardBackground
        )
    }

    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            ModalDrawerSheet(
                modifier = Modifier.width(260.dp),
                drawerContainerColor = SidebarBackground
            ) {
                // Drawer Header
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .background(
                            brush = Brush.verticalGradient(
                                colors = listOf(SidebarBackground, SidebarGradientEnd)
                            )
                        )
                        .padding(16.dp)
                ) {
                    Column {
                        Text(
                            text = "Connection",
                            style = MaterialTheme.typography.titleSmall,
                            fontWeight = FontWeight.SemiBold,
                            color = FullmoniAccent
                        )

                        Spacer(modifier = Modifier.height(12.dp))

                        // Device dropdown
                        var expanded by remember { mutableStateOf(false) }
                        ExposedDropdownMenuBox(
                            expanded = expanded && !isConnected,
                            onExpandedChange = { if (!isConnected) expanded = it }
                        ) {
                            OutlinedTextField(
                                value = selectedDriver?.device?.deviceName ?: "Select device",
                                onValueChange = {},
                                readOnly = true,
                                enabled = !isConnected,
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .menuAnchor(),
                                trailingIcon = {
                                    if (!isConnected) ExposedDropdownMenuDefaults.TrailingIcon(expanded = expanded)
                                },
                                colors = OutlinedTextFieldDefaults.colors(
                                    focusedBorderColor = FullmoniPrimary,
                                    unfocusedBorderColor = TextMuted,
                                    disabledBorderColor = TextMuted.copy(alpha = 0.5f)
                                ),
                                textStyle = MaterialTheme.typography.bodySmall.copy(color = TextPrimary)
                            )
                            ExposedDropdownMenu(
                                expanded = expanded,
                                onDismissRequest = { expanded = false }
                            ) {
                                availableDrivers.forEach { driver ->
                                    DropdownMenuItem(
                                        text = { Text(driver.device.deviceName) },
                                        onClick = {
                                            viewModel.selectDriver(driver)
                                            expanded = false
                                        }
                                    )
                                }
                            }
                        }

                        Spacer(modifier = Modifier.height(8.dp))

                        // Connect/Refresh buttons
                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.spacedBy(8.dp)
                        ) {
                            OutlinedButton(
                                onClick = { viewModel.refreshDevices() },
                                enabled = !isConnected,
                                modifier = Modifier.weight(1f),
                                colors = ButtonDefaults.outlinedButtonColors(
                                    contentColor = TextPrimary
                                )
                            ) {
                                Text("Refresh", style = MaterialTheme.typography.labelSmall)
                            }
                            Button(
                                onClick = {
                                    if (isConnected) viewModel.disconnect()
                                    else viewModel.connect()
                                },
                                enabled = selectedDriver != null || isConnected,
                                modifier = Modifier.weight(1f),
                                colors = ButtonDefaults.buttonColors(
                                    containerColor = FullmoniPrimary
                                )
                            ) {
                                Text(
                                    if (isConnected) "Disconnect" else "Connect",
                                    style = MaterialTheme.typography.labelSmall
                                )
                            }
                        }

                        // シミュレーター接続ボタン（未接続時のみ表示）
                        if (!isConnected) {
                            Spacer(modifier = Modifier.height(8.dp))
                            OutlinedButton(
                                onClick = { viewModel.connectSimulator() },
                                modifier = Modifier.fillMaxWidth(),
                                colors = ButtonDefaults.outlinedButtonColors(
                                    contentColor = FullmoniAccent
                                )
                            ) {
                                Icon(
                                    imageVector = Icons.Default.Computer,
                                    contentDescription = null,
                                    modifier = Modifier.size(16.dp)
                                )
                                Spacer(modifier = Modifier.width(4.dp))
                                Text("Simulator", style = MaterialTheme.typography.labelSmall)
                            }
                        }

                        Spacer(modifier = Modifier.height(12.dp))

                        // Connection status
                        Row(verticalAlignment = Alignment.CenterVertically) {
                            Box(
                                modifier = Modifier
                                    .size(10.dp)
                                    .clip(CircleShape)
                                    .background(if (isConnected) StatusConnected else StatusDisconnected)
                            )
                            Spacer(modifier = Modifier.width(8.dp))
                            Text(
                                text = if (isConnected) "Connected" else "Not Connected",
                                style = MaterialTheme.typography.bodySmall,
                                color = TextPrimary
                            )
                        }

                        if (isConnected && firmwareVersion != null) {
                            Text(
                                text = "FW: v$firmwareVersion",
                                style = MaterialTheme.typography.bodySmall,
                                color = TextMuted,
                                modifier = Modifier.padding(start = 18.dp, top = 4.dp)
                            )
                        }
                    }
                }

                HorizontalDivider(color = TextMuted.copy(alpha = 0.2f))

                Spacer(modifier = Modifier.height(8.dp))

                // Navigation Items
                val isFirmwareMode = isConnected && !isBootloaderMode

                NavigationDrawerItem(
                    icon = Icons.Default.Home,
                    label = "Home",
                    selected = currentScreen == "home",
                    onClick = {
                        currentScreen = "home"
                        scope.launch { drawerState.close() }
                    }
                )
                NavigationDrawerItem(
                    icon = Icons.Default.Timer,
                    label = "Status",
                    selected = currentScreen == "status",
                    enabled = isFirmwareMode,
                    onClick = {
                        currentScreen = "status"
                        scope.launch { drawerState.close() }
                    }
                )
                NavigationDrawerItem(
                    icon = Icons.Default.Settings,
                    label = "Settings",
                    selected = currentScreen == "settings",
                    enabled = isFirmwareMode,
                    onClick = {
                        currentScreen = "settings"
                        scope.launch { drawerState.close() }
                    }
                )
                NavigationDrawerItem(
                    icon = Icons.Default.SettingsInputComponent,
                    label = "CAN Config",
                    selected = currentScreen == "can",
                    enabled = isFirmwareMode,
                    onClick = {
                        currentScreen = "can"
                        scope.launch { drawerState.close() }
                    }
                )
                NavigationDrawerItem(
                    icon = Icons.Default.Wallpaper,
                    label = "Boot Screen",
                    selected = currentScreen == "startup",
                    enabled = isFirmwareMode,
                    onClick = {
                        currentScreen = "startup"
                        scope.launch { drawerState.close() }
                    }
                )
                NavigationDrawerItem(
                    icon = Icons.Default.SystemUpdateAlt,
                    label = "FW Update",
                    selected = currentScreen == "firmware",
                    onClick = {
                        currentScreen = "firmware"
                        scope.launch { drawerState.close() }
                    }
                )

                Spacer(modifier = Modifier.weight(1f))

                NavigationDrawerItem(
                    icon = Icons.Default.Info,
                    label = "About",
                    selected = currentScreen == "about",
                    onClick = {
                        currentScreen = "about"
                        scope.launch { drawerState.close() }
                    }
                )

                Spacer(modifier = Modifier.height(16.dp))
            }
        }
    ) {
        Scaffold(
            topBar = {
                TopAppBar(
                    title = {
                        Text(
                            "FULLMONI-WIDE Terminal",
                            color = TextPrimary
                        )
                    },
                    navigationIcon = {
                        IconButton(onClick = { scope.launch { drawerState.open() } }) {
                            Icon(
                                Icons.Default.Menu,
                                contentDescription = "Menu",
                                tint = TextPrimary
                            )
                        }
                    },
                    actions = {
                        // Connection status indicator
                        Box(
                            modifier = Modifier
                                .padding(end = 16.dp)
                                .size(12.dp)
                                .clip(CircleShape)
                                .background(if (isConnected) StatusConnected else StatusDisconnected)
                        )
                    },
                    colors = TopAppBarDefaults.topAppBarColors(
                        containerColor = SidebarBackground,
                        titleContentColor = TextPrimary
                    )
                )
            },
            containerColor = BackgroundDark
        ) { paddingValues ->
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(paddingValues)
                    .background(
                        brush = Brush.linearGradient(
                            colors = listOf(BackgroundDark, BackgroundMedium, BackgroundDark)
                        )
                    )
            ) {
                when (currentScreen) {
                    "home" -> HomeScreen(
                        viewModel = viewModel,
                        onNavigate = { screen -> currentScreen = screen }
                    )
                    "status" -> StatusScreen(viewModel = viewModel)
                    "settings" -> SettingsScreen(viewModel = viewModel)
                    "can" -> CanConfigScreen(viewModel = viewModel)
                    "startup" -> BootScreenScreen(viewModel = viewModel)
                    "firmware" -> FirmwareUpdateScreen(viewModel = viewModel)
                    "about" -> AboutScreen()
                }
            }
        }
    }
}

@Composable
private fun NavigationDrawerItem(
    icon: ImageVector,
    label: String,
    selected: Boolean,
    enabled: Boolean = true,
    onClick: () -> Unit
) {
    val backgroundColor = when {
        selected -> FullmoniPrimary.copy(alpha = 0.2f)
        else -> androidx.compose.ui.graphics.Color.Transparent
    }
    val contentColor = when {
        !enabled -> TextMuted.copy(alpha = 0.5f)
        selected -> FullmoniPrimary
        else -> TextPrimary
    }

    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 12.dp, vertical = 2.dp)
            .clip(RoundedCornerShape(8.dp))
            .then(
                if (enabled) Modifier.clickable(onClick = onClick)
                else Modifier
            ),
        color = backgroundColor
    ) {
        Row(
            modifier = Modifier.padding(horizontal = 12.dp, vertical = 10.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Icon(
                imageVector = icon,
                contentDescription = null,
                tint = contentColor,
                modifier = Modifier.size(24.dp)
            )
            Spacer(modifier = Modifier.width(12.dp))
            Text(
                text = label,
                style = MaterialTheme.typography.bodyMedium,
                color = contentColor
            )
        }
    }
}
