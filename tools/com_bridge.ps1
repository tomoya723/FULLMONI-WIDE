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
$serial.ReadBufferSize = 65536
$serial.WriteBufferSize = 65536

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
                # TCP -> Serial (Androidからのデータ) - バイナリ対応
                if ($stream.DataAvailable) {
                    $bytesRead = $stream.Read($buffer, 0, $buffer.Length)
                    if ($bytesRead -gt 0) {
                        # バイナリデータをそのまま送信
                        $serial.BaseStream.Write($buffer, 0, $bytesRead)
                        $serial.BaseStream.Flush()

                        # ログ表示（印字可能文字のみ表示、それ以外は省略）
                        $printable = [System.Text.Encoding]::ASCII.GetString($buffer, 0, [Math]::Min($bytesRead, 80))
                        $printable = $printable -replace '[^\x20-\x7E]', '.'
                        if ($bytesRead -le 80) {
                            Write-Host "[TX->DEVICE] $bytesRead bytes: $printable" -ForegroundColor Cyan
                        } else {
                            Write-Host "[TX->DEVICE] $bytesRead bytes: $($printable.Substring(0, 40))..." -ForegroundColor Cyan
                        }
                    }
                }

                # Serial -> TCP (デバイスからの応答) - バイナリ対応
                $bytesToRead = $serial.BytesToRead
                if ($bytesToRead -gt 0) {
                    $rxBuffer = New-Object byte[] $bytesToRead
                    $actualRead = $serial.BaseStream.Read($rxBuffer, 0, $bytesToRead)
                    if ($actualRead -gt 0) {
                        $stream.Write($rxBuffer, 0, $actualRead)
                        $stream.Flush()

                        # ログ表示
                        $printable = [System.Text.Encoding]::ASCII.GetString($rxBuffer, 0, [Math]::Min($actualRead, 200))
                        $printable = $printable -replace '[^\x20-\x7E\r\n]', '.'
                        Write-Host "[RX<-DEVICE] $actualRead bytes: $printable" -ForegroundColor Magenta
                    }
                }

                Start-Sleep -Milliseconds 5
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
