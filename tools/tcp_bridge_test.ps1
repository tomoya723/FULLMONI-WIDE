#!/usr/bin/env pwsh
# TCP Bridge Test Script for Android Emulator + FULLMONI-WIDE Real Device
param(
    [string]$ComPort = "COM19"
)

$ErrorActionPreference = "Stop"
$ADB = "C:\Users\tomoy\AppData\Local\Android\Sdk\platform-tools\adb.exe"
$EMULATOR = "C:\Users\tomoy\AppData\Local\Android\Sdk\emulator\emulator.exe"
$AVD = "Medium_Phone_API_36.1"

Write-Host "=== TCP Bridge Test for Android Emulator ===" -ForegroundColor Cyan
Write-Host "COM Port: $ComPort"
Write-Host ""

# Step 1: Check if emulator is running
Write-Host "[1/5] Checking emulator status..." -ForegroundColor Yellow
$devices = & $ADB devices 2>&1 | Out-String
Write-Host $devices

$emulatorRunning = $devices -match "emulator-\d+.*device"
if (-not $emulatorRunning) {
    Write-Host "Starting emulator..." -ForegroundColor Yellow
    Start-Process -FilePath $EMULATOR -ArgumentList "-avd",$AVD,"-no-snapshot-load" -WindowStyle Minimized
    
    Write-Host "Waiting for emulator to boot (60s)..." -ForegroundColor Yellow
    for ($i = 0; $i -lt 60; $i++) {
        Start-Sleep -Seconds 1
        $devices = & $ADB devices 2>&1 | Out-String
        if ($devices -match "emulator-\d+.*device") {
            Write-Host "Emulator ready!" -ForegroundColor Green
            break
        }
        if ($i % 10 -eq 9) {
            Write-Host "  Still waiting... ($($i+1)s)"
        }
    }
}

# Step 2: Setup adb reverse
Write-Host ""
Write-Host "[2/5] Setting up adb reverse tcp:9999 tcp:9999..." -ForegroundColor Yellow
& $ADB reverse tcp:9999 tcp:9999
$reverseList = & $ADB reverse --list 2>&1 | Out-String
Write-Host "Reverse list: $reverseList"

# Step 3: Install and launch app
Write-Host ""
Write-Host "[3/5] Installing and launching app..." -ForegroundColor Yellow
$apkPath = "C:\Users\tomoy\git\FULLMONI-WIDE\HostApp\FULLMONI-WIDE-Android\app\build\outputs\apk\debug\app-debug.apk"
if (Test-Path $apkPath) {
    & $ADB install -r $apkPath 2>&1 | Out-Null
    Write-Host "APK installed"
}
& $ADB shell am start -n com.example.fullmoni_wide_android/.MainActivity 2>&1 | Out-Null
Write-Host "App launched"
Start-Sleep -Seconds 3

# Step 4: Start TCP Bridge in background
Write-Host ""
Write-Host "[4/5] Starting TCP Bridge on $ComPort..." -ForegroundColor Yellow
$bridgeScript = Join-Path $PSScriptRoot "com_bridge.ps1"
$bridgeJob = Start-Job -ScriptBlock {
    param($script, $port)
    powershell -ExecutionPolicy Bypass -File $script -ComPort $port
} -ArgumentList $bridgeScript, $ComPort

Start-Sleep -Seconds 2
Write-Host "Bridge job started (ID: $($bridgeJob.Id))"

# Step 5: Tap TCP Bridge button in app
Write-Host ""
Write-Host "[5/5] Tapping TCP Bridge (Real) button..." -ForegroundColor Yellow
# Get button location from UI dump
& $ADB shell uiautomator dump /sdcard/ui.xml 2>&1 | Out-Null
$uiXml = & $ADB shell cat /sdcard/ui.xml 2>&1 | Out-String

# Look for TCP Bridge button
if ($uiXml -match 'text="TCP Bridge \(Real\)"[^>]*bounds="\[(\d+),(\d+)\]\[(\d+),(\d+)\]"') {
    $x = [int](([int]$Matches[1] + [int]$Matches[3]) / 2)
    $y = [int](([int]$Matches[2] + [int]$Matches[4]) / 2)
    Write-Host "Found button at ($x, $y)"
    & $ADB shell input tap $x $y
} else {
    Write-Host "TCP Bridge button not found, trying default location..." -ForegroundColor Yellow
    & $ADB shell input tap 377 1157
}

Start-Sleep -Seconds 3

# Check results
Write-Host ""
Write-Host "=== Results ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "Logcat (last 20 lines):" -ForegroundColor Yellow
& $ADB logcat -d -t 20 -s "FULLMONI" 2>&1 | Out-String | Write-Host

Write-Host ""
Write-Host "Bridge job status:" -ForegroundColor Yellow
$jobState = Receive-Job -Job $bridgeJob -Keep 2>&1 | Out-String
Write-Host $jobState

Write-Host ""
Write-Host "Press Ctrl+C to stop..." -ForegroundColor Yellow
try {
    Wait-Job $bridgeJob
} finally {
    Stop-Job $bridgeJob -ErrorAction SilentlyContinue
    Remove-Job $bridgeJob -ErrorAction SilentlyContinue
}
