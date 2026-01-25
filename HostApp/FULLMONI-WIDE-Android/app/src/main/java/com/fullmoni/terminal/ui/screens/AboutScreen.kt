package com.fullmoni.terminal.ui.screens

import android.content.Intent
import android.net.Uri
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.fullmoni.terminal.ui.theme.*

/**
 * About画面
 * アプリバージョン、ハードウェア情報、ライセンス情報、GitHubリンク
 */
@Composable
fun AboutScreen() {
    val context = LocalContext.current
    val scrollState = rememberScrollState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Spacer(modifier = Modifier.height(32.dp))

        // Logo placeholder (アイコン代わり)
        Card(
            modifier = Modifier.size(120.dp),
            shape = RoundedCornerShape(24.dp),
            colors = CardDefaults.cardColors(containerColor = FullmoniPrimary)
        ) {
            Box(
                modifier = Modifier.fillMaxSize(),
                contentAlignment = Alignment.Center
            ) {
                Text(
                    text = "FMW",
                    style = MaterialTheme.typography.headlineLarge,
                    fontWeight = FontWeight.Bold,
                    color = TextPrimary
                )
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        // Title
        Text(
            text = "FULLMONI-WIDE Terminal",
            style = MaterialTheme.typography.headlineMedium,
            fontWeight = FontWeight.Bold,
            color = TextPrimary,
            textAlign = TextAlign.Center
        )

        Spacer(modifier = Modifier.height(8.dp))

        Text(
            text = "Version 1.0.0",
            style = MaterialTheme.typography.bodyMedium,
            color = TextMuted
        )

        Spacer(modifier = Modifier.height(32.dp))

        // Description Card
        Card(
            modifier = Modifier.fillMaxWidth(),
            colors = CardDefaults.cardColors(containerColor = CardBackground),
            shape = RoundedCornerShape(12.dp)
        ) {
            Column(modifier = Modifier.padding(20.dp)) {
                Text(
                    text = "About This Tool",
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.SemiBold,
                    color = TextPrimary
                )

                Spacer(modifier = Modifier.height(12.dp))

                Text(
                    text = "Configuration tool for FULLMONI-WIDE digital meter.\nConnect via USB to configure parameters and update firmware.",
                    style = MaterialTheme.typography.bodyMedium,
                    color = TextSecondary,
                    lineHeight = MaterialTheme.typography.bodyMedium.lineHeight * 1.4f
                )

                Spacer(modifier = Modifier.height(20.dp))

                Text(
                    text = "Features",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold,
                    color = TextPrimary
                )

                Spacer(modifier = Modifier.height(8.dp))

                Column(modifier = Modifier.padding(start = 8.dp)) {
                    FeatureItem("Real-time status monitoring (RPM, Speed, Temp, etc.)")
                    FeatureItem("Parameter settings (Tire size, Gear ratio, Warnings)")
                    FeatureItem("CAN configuration (ECU presets, Custom IDs)")
                    FeatureItem("Boot screen customization (765×256 BMP)")
                    FeatureItem("Firmware update via USB (~268KB/s)")
                }

                Spacer(modifier = Modifier.height(20.dp))

                Text(
                    text = "Hardware",
                    style = MaterialTheme.typography.titleSmall,
                    fontWeight = FontWeight.SemiBold,
                    color = TextPrimary
                )

                Spacer(modifier = Modifier.height(8.dp))

                Text(
                    text = "Renesas RX72N MCU • 800×256 TFT LCD • CAN Bus • Neopixel LEDs",
                    style = MaterialTheme.typography.bodySmall,
                    color = TextMuted
                )
            }
        }

        Spacer(modifier = Modifier.height(24.dp))

        // Copyright
        Text(
            text = "© 2024-2026 FULLMONI-WIDE Project",
            style = MaterialTheme.typography.bodySmall,
            color = TextMuted
        )

        Spacer(modifier = Modifier.height(4.dp))

        Text(
            text = "Open Source Hardware & Software (MIT License)",
            style = MaterialTheme.typography.bodySmall,
            color = TextMuted.copy(alpha = 0.7f)
        )

        Spacer(modifier = Modifier.height(16.dp))

        // GitHub Link
        OutlinedButton(
            onClick = {
                val intent = Intent(Intent.ACTION_VIEW, Uri.parse("https://github.com/tomoya723/FULLMONI-WIDE"))
                context.startActivity(intent)
            },
            colors = ButtonDefaults.outlinedButtonColors(
                contentColor = FullmoniPrimary
            )
        ) {
            Text("🔗 GitHub")
        }

        Spacer(modifier = Modifier.height(32.dp))
    }
}

@Composable
private fun FeatureItem(text: String) {
    Row(modifier = Modifier.padding(vertical = 2.dp)) {
        Text(
            text = "•",
            style = MaterialTheme.typography.bodySmall,
            color = TextMuted,
            modifier = Modifier.padding(end = 8.dp)
        )
        Text(
            text = text,
            style = MaterialTheme.typography.bodySmall,
            color = TextMuted.copy(alpha = 0.85f)
        )
    }
}
