package com.fullmoni.terminal.ui.screens

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
import com.fullmoni.terminal.ui.components.ExpandableCard
import com.fullmoni.terminal.ui.theme.*
import com.fullmoni.terminal.viewmodel.MainViewModel

/**
 * ステータス画面
 * ODO/TRIP/DateTime表示、TRIP Reset、RTC Sync機能
 */
@Composable
fun StatusScreen(viewModel: MainViewModel) {
    val isConnected by viewModel.isConnected.collectAsState()
    val odoValue by viewModel.odoValue.collectAsState()
    val tripValue by viewModel.tripValue.collectAsState()
    val rtcValue by viewModel.rtcValue.collectAsState()
    val scrollState = rememberScrollState()

    Column(
        modifier = Modifier
            .fillMaxSize()
            .verticalScroll(scrollState)
            .padding(16.dp)
    ) {
        Text(
            text = "Status",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold,
            color = TextPrimary
        )

        Spacer(modifier = Modifier.height(16.dp))

        // Meter Info Card
        ExpandableCard(
            title = "Meter Info",
            subtitle = "View and configure ODO/TRIP/DateTime",
            icon = Icons.Default.Timer,
            initialExpanded = true
        ) {
            Column(modifier = Modifier.padding(16.dp)) {
                // ODO
                StatusRow(
                    label = "ODO",
                    value = "$odoValue km",
                    showButton = false,
                    onButtonClick = {}
                )
                
                Spacer(modifier = Modifier.height(16.dp))
                
                // TRIP
                StatusRow(
                    label = "TRIP",
                    value = "$tripValue km",
                    showButton = true,
                    buttonText = "RESET",
                    onButtonClick = { viewModel.resetTrip() }
                )
                
                Spacer(modifier = Modifier.height(16.dp))
                
                // DateTime
                StatusRow(
                    label = "DateTime",
                    value = rtcValue,
                    showButton = true,
                    buttonText = "Sync with Phone",
                    onButtonClick = { viewModel.syncRtc() }
                )
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Reload Button
        Button(
            onClick = { viewModel.loadParameters() },
            modifier = Modifier.align(Alignment.CenterHorizontally),
            enabled = isConnected,
            colors = ButtonDefaults.buttonColors(containerColor = FullmoniPrimary)
        ) {
            Icon(Icons.Default.Refresh, contentDescription = null)
            Spacer(modifier = Modifier.width(8.dp))
            Text("Reload")
        }
    }
}

@Composable
private fun StatusRow(
    label: String,
    value: String,
    showButton: Boolean,
    buttonText: String = "",
    onButtonClick: () -> Unit
) {
    Row(
        modifier = Modifier.fillMaxWidth(),
        horizontalArrangement = Arrangement.SpaceBetween,
        verticalAlignment = Alignment.CenterVertically
    ) {
        Column(modifier = Modifier.weight(1f)) {
            Text(
                text = label,
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.SemiBold,
                color = TextPrimary
            )
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = value,
                style = MaterialTheme.typography.bodyLarge,
                fontWeight = FontWeight.Medium,
                color = TextSecondary
            )
        }
        if (showButton) {
            OutlinedButton(
                onClick = onButtonClick,
                colors = ButtonDefaults.outlinedButtonColors(
                    contentColor = FullmoniPrimary
                )
            ) {
                Text(buttonText)
            }
        }
    }
}
