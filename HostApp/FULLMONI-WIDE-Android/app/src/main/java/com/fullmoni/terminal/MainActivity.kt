package com.fullmoni.terminal

import android.content.Intent
import android.hardware.usb.UsbManager
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import com.fullmoni.terminal.ui.screens.MainScreen
import com.fullmoni.terminal.ui.theme.FullmoniTheme
import com.fullmoni.terminal.viewmodel.MainViewModel

class MainActivity : ComponentActivity() {
    
    companion object {
        private const val TAG = "MainActivity"
    }
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        
        // USBデバイス接続によるIntentをログ
        handleUsbIntent(intent)
        
        setContent {
            FullmoniTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    val viewModel: MainViewModel = viewModel(
                        factory = MainViewModel.Factory(applicationContext)
                    )
                    MainScreen(viewModel = viewModel)
                }
            }
        }
    }
    
    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        // singleTopモードでUSBデバイス接続時に呼ばれる
        // Activityは再起動されず、このメソッドだけが呼ばれる
        handleUsbIntent(intent)
    }
    
    private fun handleUsbIntent(intent: Intent?) {
        if (intent?.action == UsbManager.ACTION_USB_DEVICE_ATTACHED) {
            Log.d(TAG, "USB device attached via intent - Activity not restarted due to singleTop mode")
            // FW Update中なら、自動的に再接続処理が行われる
            // (実際の接続処理はUsbSerialServiceがBroadcastReceiverで行う)
        }
    }
}
