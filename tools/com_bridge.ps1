# COM to TCP Bridge for Android Emulator Testing
# Usage: .\com_bridge.ps1 -ComPort COM19 -TcpPort 9999

param(
    [string]$ComPort = "COM19",
    [int]$TcpPort = 9999
)

Write-Host "=== COM to TCP Bridge ===" -ForegroundColor Cyan
Write-Host "COM Port: $ComPort"
Write-Host "TCP Port: $TcpPort"
Write-Host ""

# シリアルポートを開く
$serial = New-Object System.IO.Ports.SerialPort $ComPort, 115200, "None", 8, "One"
$serial.ReadTimeout = 100
$serial.WriteTimeout = 1000

try {
    $serial.Open()
    Write-Host "[OK] Serial port opened" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] Failed to open $ComPort : $_" -ForegroundColor Red
    exit 1
}

# TCPリスナーを開始
$listener = [System.Net.Sockets.TcpListener]::new([System.Net.IPAddress]::Any, $TcpPort)
$listener.Start()
Write-Host "[OK] TCP listener started on port $TcpPort" -ForegroundColor Green
Write-Host ""
Write-Host "Waiting for connection..." -ForegroundColor Yellow
Write-Host "Run this on another terminal: adb reverse tcp:9999 tcp:9999" -ForegroundColor Yellow
Write-Host "Press Ctrl+C to stop" -ForegroundColor Yellow
Write-Host ""

try {
    while ($true) {
        # クライアント接続を待機
        $client = $listener.AcceptTcpClient()
        Write-Host "[CONNECTED] Client connected from $($client.Client.RemoteEndPoint)" -ForegroundColor Green
        
        $stream = $client.GetStream()
        $buffer = New-Object byte[] 1024
        
        try {
            while ($client.Connected) {
                # TCP -> Serial (Androidからのコマンド)
                if ($stream.DataAvailable) {
                    $bytesRead = $stream.Read($buffer, 0, $buffer.Length)
                    if ($bytesRead -gt 0) {
                        $data = [System.Text.Encoding]::ASCII.GetString($buffer, 0, $bytesRead)
                        Write-Host "[TX->DEVICE] $($data.Trim())" -ForegroundColor Cyan
                        $serial.Write($data)
                    }
                }
                
                # Serial -> TCP (デバイスからの応答)
                try {
                    $serialData = $serial.ReadExisting()
                    if ($serialData.Length -gt 0) {
                        Write-Host "[RX<-DEVICE] $($serialData.Trim())" -ForegroundColor Magenta
                        $bytes = [System.Text.Encoding]::ASCII.GetBytes($serialData)
                        $stream.Write($bytes, 0, $bytes.Length)
                    }
                } catch {}
                
                Start-Sleep -Milliseconds 10
            }
        } catch {
            Write-Host "[DISCONNECTED] Client disconnected: $_" -ForegroundColor Yellow
        }
        
        $client.Close()
        Write-Host "Waiting for next connection..." -ForegroundColor Yellow
    }
} finally {
    $listener.Stop()
    $serial.Close()
    Write-Host "Bridge stopped" -ForegroundColor Red
}
