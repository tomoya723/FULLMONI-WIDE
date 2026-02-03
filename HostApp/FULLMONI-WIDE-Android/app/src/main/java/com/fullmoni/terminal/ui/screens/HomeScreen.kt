package com.fullmoni.terminal.ui.screens

import androidx.compose.foundation.BorderStroke
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel

// デバッグビルドかどうかを判定する定数（BuildConfigが生成されない場合のフォールバック）
private val IS_DEBUG_BUILD = true  // リリースビルド時はfalseに変更

/**
 * ホーム画面（ダッシュボード）
 * Windows版のヒーローバナー + クイックアクセスカードを再現
 */
@Composable
fun HomeScreen(
    viewModel: MainViewModel,
    onNavigate: (String) -> Unit
) {
    val isConnected by viewModel.isConnected.collectAsState()
    val firmwareVersion by viewModel.firmwareVersion.collectAsState()
    val connectionPort by viewModel.connectionPort.collectAsState()
    val scrollState = rememberScrollState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp)
    ) {
        // ヒーローバナー
        HeroBanner(
            isConnected = isConnected,
            firmwareVersion = firmwareVersion,
            isSimulationMode = viewModel.isSimulationMode,
            onConnectClick = {
                if (isConnected) {
                    viewModel.disconnect()
                } else {
                    viewModel.connect()
                }
            },
            onSimulatorClick = {
                viewModel.connectSimulator()
            },
            onTcpBridgeClick = {
                viewModel.connectTcpBridge()
            }
        )

        Spacer(modifier = Modifier.height(24.dp))

        // Quick Access タイトル
        Text(
            text = "Quick Access",
            style = MaterialTheme.typography.titleMedium,
            fontWeight = FontWeight.SemiBold,
            color = TextPrimary
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Feature Cards - 上段4つ
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            FeatureCard(
                modifier = Modifier.weight(1f),
                title = "Status",
                subtitle = "ODO/TRIP/Clock",
                icon = Icons.Default.Timer,
                iconColor = CardBlue,
                enabled = isConnected,
                onClick = { onNavigate("status") }
            )
            FeatureCard(
                modifier = Modifier.weight(1f),
                title = "Settings",
                subtitle = "Tire/Gear/Warnings",
                icon = Icons.Default.Settings,
                iconColor = CardGreen,
                enabled = isConnected,
                onClick = { onNavigate("settings") }
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            FeatureCard(
                modifier = Modifier.weight(1f),
                title = "CAN Config",
                subtitle = "Channels/Fields",
                icon = Icons.Default.SettingsInputComponent,
                iconColor = CardYellow,
                enabled = isConnected,
                onClick = { onNavigate("can") }
            )
            FeatureCard(
                modifier = Modifier.weight(1f),
                title = "Boot Screen",
                subtitle = "Custom Image",
                icon = Icons.Default.Wallpaper,
                iconColor = CardPurple,
                enabled = isConnected,
                onClick = { onNavigate("startup") }
            )
        }

        Spacer(modifier = Modifier.height(12.dp))

        // 下段2つ
        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            FeatureCard(
                modifier = Modifier.weight(1f),
                title = "FW Update",
                subtitle = "Update firmware",
                icon = Icons.Default.SystemUpdateAlt,
                iconColor = CardRed,
                enabled = true,
                onClick = { onNavigate("firmware") }
            )
            FeatureCard(
                modifier = Modifier.weight(1f),
                title = "About",
                subtitle = "Version & License",
                icon = Icons.Default.Info,
                iconColor = CardIndigo,
                enabled = true,
                onClick = { onNavigate("about") }
            )
        }

        Spacer(modifier = Modifier.height(24.dp))

        // Footer Info Card
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = CardBackground),
            shape = RoundedCornerShape(8.dp)
        ) {
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Column {
                    Text(
                        text = "Connected Device",
                        style = MaterialTheme.typography.titleSmall,
                        fontWeight = FontWeight.SemiBold,
                        color = TextPrimary
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                    Text(
                        text = "Port: ${connectionPort ?: "Not connected"}  |  FW: ${firmwareVersion ?: "---"}",
                        style = MaterialTheme.typography.bodySmall,
                        color = TextMuted
                    )
                }
                Button(
                    onClick = { onNavigate("status") },
                    enabled = isConnected,
                    colors = ButtonDefaults.buttonColors(
                        containerColor = FullmoniPrimary
                    )
                ) {
                    Text("View Details")
                }
            }
        }
    }
}

@Composable
private fun HeroBanner(
    isConnected: Boolean,
    firmwareVersion: String?,
    isSimulationMode: Boolean,
    onConnectClick: () -> Unit,
    onSimulatorClick: () -> Unit,
    onTcpBridgeClick: () -> Unit
) {
    Card(
        modifier = Modifier.fillMaxWidth(),
        shape = RoundedCornerShape(16.dp),
        colors = CardDefaults.cardColors(containerColor = Color.Transparent)
    ) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .background(
                    brush = Brush.horizontalGradient(
                        colors = listOf(
                            FullmoniPrimary,
                            FullmoniPrimary,
                            FullmoniPrimaryDark,
                            Color(0xFF06475F)
                        )
                    )
                )
                .padding(24.dp)
        ) {
            Column {
                // ステータスバッジ
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.End
                ) {
                    Surface(
                        shape = RoundedCornerShape(8.dp),
                        color = FullmoniPrimaryDark
                    ) {
                        Row(
                            modifier = Modifier.padding(horizontal = 12.dp, vertical = 8.dp),
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Box(
                                modifier = Modifier
                                    .size(10.dp)
                                    .clip(CircleShape)
                                    .background(if (isConnected) StatusConnected else StatusDisconnected)
                            )
                            Spacer(modifier = Modifier.width(8.dp))
                            Text(
                                text = when {
                                    isSimulationMode -> "Simulator"
                                    isConnected -> "Connected"
                                    else -> "Not Connected"
                                },
                                color = TextPrimary,
                                style = MaterialTheme.typography.bodySmall,
                                fontWeight = FontWeight.Medium
                            )
                        }
                    }
                }

                Spacer(modifier = Modifier.height(16.dp))

                Text(
                    text = "RX72N Multimeter",
                    style = MaterialTheme.typography.bodySmall,
                    color = TextSecondary.copy(alpha = 0.9f)
                )
                Spacer(modifier = Modifier.height(4.dp))
                Text(
                    text = "FULLMONI-WIDE",
                    style = MaterialTheme.typography.headlineLarge,
                    fontWeight = FontWeight.Bold,
                    color = TextPrimary
                )
                Spacer(modifier = Modifier.height(8.dp))
                Text(
                    text = "Build your custom automotive display with CAN bus integration.",
                    style = MaterialTheme.typography.bodyMedium,
                    color = TextSecondary.copy(alpha = 0.9f)
                )

                Spacer(modifier = Modifier.height(20.dp))

                // 接続ボタン（実デバイス）
                Button(
                    onClick = onConnectClick,
                    colors = ButtonDefaults.buttonColors(
                        containerColor = Color.White,
                        contentColor = FullmoniPrimaryDark
                    ),
                    contentPadding = PaddingValues(horizontal = 20.dp, vertical = 12.dp)
                ) {
                    Icon(
                        imageVector = Icons.Default.Usb,
                        contentDescription = null,
                        modifier = Modifier.size(20.dp)
                    )
                    Spacer(modifier = Modifier.width(8.dp))
                    Text(
                        text = if (isConnected) "Disconnect" else "Connect Device",
                        fontWeight = FontWeight.Medium
                    )
                }
                
                // シミュレーターボタン（デバッグビルドのみ）
                if (IS_DEBUG_BUILD && !isConnected) {
                    Spacer(modifier = Modifier.height(8.dp))
                    OutlinedButton(
                        onClick = onSimulatorClick,
                        colors = ButtonDefaults.outlinedButtonColors(
                            contentColor = Color.White
                        ),
                        border = BorderStroke(1.dp, Color.White.copy(alpha = 0.5f)),
                        contentPadding = PaddingValues(horizontal = 20.dp, vertical = 12.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Default.Computer,
                            contentDescription = null,
                            modifier = Modifier.size(20.dp)
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Text(
                            text = "Connect Simulator",
                            fontWeight = FontWeight.Medium
                        )
                    }
                    
                    Spacer(modifier = Modifier.height(8.dp))
                    OutlinedButton(
                        onClick = onTcpBridgeClick,
                        colors = ButtonDefaults.outlinedButtonColors(
                            contentColor = Color.White
                        ),
                        border = BorderStroke(1.dp, Color(0xFFFF9800).copy(alpha = 0.7f)),
                        contentPadding = PaddingValues(horizontal = 20.dp, vertical = 12.dp)
                    ) {
                        Icon(
                            imageVector = Icons.Default.SettingsEthernet,
                            contentDescription = null,
                            modifier = Modifier.size(20.dp),
                            tint = Color(0xFFFF9800)
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Text(
                            text = "TCP Bridge (Real)",
                            fontWeight = FontWeight.Medium,
                            color = Color(0xFFFF9800)
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun FeatureCard(
    modifier: Modifier = Modifier,
    title: String,
    subtitle: String,
    icon: ImageVector,
    iconColor: Color,
    enabled: Boolean,
    onClick: () -> Unit
) {
    Card(
        modifier = modifier
            .clickable(enabled = enabled, onClick = onClick),
        colors = CardDefaults.cardColors(
            containerColor = CardBackground.copy(alpha = if (enabled) 1f else 0.5f)
        ),
        shape = RoundedCornerShape(12.dp)
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Box(
                modifier = Modifier
                    .size(48.dp)
                    .clip(RoundedCornerShape(12.dp))
                    .background(iconColor),
                contentAlignment = Alignment.Center
            ) {
                Icon(
                    imageVector = icon,
                    contentDescription = null,
                    tint = Color.White,
                    modifier = Modifier.size(24.dp)
                )
            }
            Spacer(modifier = Modifier.height(12.dp))
            Text(
                text = title,
                style = MaterialTheme.typography.titleSmall,
                fontWeight = FontWeight.SemiBold,
                color = if (enabled) TextPrimary else TextMuted
            )
            Text(
                text = subtitle,
                style = MaterialTheme.typography.bodySmall,
                color = TextMuted
            )
        }
    }
}
