# RAM Usage Checker for FULLMONI-WIDE Firmware
# mapファイルから正確なRAM使用量を計算して表示
# rx-elf-sizeのbss値は不正確な場合があるため、このスクリプトを使用

param(
    [string]$MapFile = "Firmware.map"
)

if (-not (Test-Path $MapFile)) {
    Write-Host "Error: $MapFile not found" -ForegroundColor Red
    exit 1
}

$map = Get-Content $MapFile

# === メモリマップ定義 (qe_emwin_config.h より) ===
# フレームバッファサイズ: 800 x 256 x 2bytes = 409,600 bytes (0x64000)
$FB_SIZE = 0x64000  # 409,600 bytes

# 低位RAM (0x00000000-0x0007FFFF, 512KB)
$LOWRAM_START = 0x00000000
$LOWRAM_END   = 0x00080000
$FB2_START    = 0x00000040  # EMWIN_GUI_FRAME_BUFFER2
$FB2_END      = $FB2_START + $FB_SIZE  # 0x00064040

# 高位RAM (0x00800000-0x0087FFFF, 512KB)  
$HIGHRAM_START = 0x00800000
$HIGHRAM_END   = 0x00880000
$FB1_START     = 0x00800000  # EMWIN_GUI_FRAME_BUFFER1
$FB1_END       = $FB1_START + $FB_SIZE  # 0x00864000

# BSS領域 (リンカスクリプトより)
$BSS_REGION_START = 0x00064040
$BSS_REGION_END   = 0x00080000
$BSS_CAPACITY     = $BSS_REGION_END - $BSS_REGION_START  # 0x1BFC0 = 114,624 bytes

# BSS開始・終了アドレスを抽出
$bssLine = $map | Select-String "_bss = \." | Select-Object -First 1
$ebssLine = $map | Select-String "_ebss = \." | Select-Object -First 1

if (-not $bssLine -or -not $ebssLine) {
    Write-Host "Error: Could not find _bss or _ebss in map file" -ForegroundColor Red
    exit 1
}

# アドレスを抽出
$bssAddr = [regex]::Match($bssLine.Line, '0x([0-9a-fA-F]+)').Groups[1].Value
$ebssAddr = [regex]::Match($ebssLine.Line, '0x([0-9a-fA-F]+)').Groups[1].Value

$bss = [Convert]::ToInt64($bssAddr, 16)
$ebss = [Convert]::ToInt64($ebssAddr, 16)
$used = $ebss - $bss
$free = $BSS_CAPACITY - $used
$usagePercent = $used / $BSS_CAPACITY

# === メモリマップ表示 ===
Write-Host ""
Write-Host "=== RX72N Internal RAM Memory Map ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "Low RAM (0x00000000 - 0x0007FFFF, 512KB):" -ForegroundColor White
Write-Host ("  0x{0:X8} - 0x{1:X8}: Frame Buffer 2 ({2:N0} KB)" -f $FB2_START, ($FB2_END - 1), ($FB_SIZE / 1024)) -ForegroundColor DarkCyan
Write-Host ("  0x{0:X8} - 0x{1:X8}: BSS Region    ({2:N0} KB)" -f $BSS_REGION_START, ($BSS_REGION_END - 1), ($BSS_CAPACITY / 1024)) -ForegroundColor Yellow
Write-Host ""
Write-Host "High RAM (0x00800000 - 0x0087FFFF, 512KB):" -ForegroundColor White
Write-Host ("  0x{0:X8} - 0x{1:X8}: Frame Buffer 1 ({2:N0} KB)" -f $FB1_START, ($FB1_END - 1), ($FB_SIZE / 1024)) -ForegroundColor DarkCyan
Write-Host ("  0x{0:X8} - 0x{1:X8}: Unused        ({2:N0} KB)" -f $FB1_END, ($HIGHRAM_END - 1), (($HIGHRAM_END - $FB1_END) / 1024)) -ForegroundColor DarkGreen

# === BSS使用量詳細 ===
Write-Host ""
Write-Host "=== BSS Region Usage ===" -ForegroundColor Cyan
Write-Host ("BSS Start:    0x{0:X8}" -f $bss)
Write-Host ("BSS End:      0x{0:X8}" -f $ebss)
Write-Host ""
Write-Host ("Used:         {0,10:N0} bytes ({1,5:P1})" -f $used, $usagePercent)
Write-Host ("Capacity:     {0,10:N0} bytes" -f $BSS_CAPACITY)

$freeColor = if ($free -lt 256) { 'Red' } elseif ($free -lt 1024) { 'Yellow' } else { 'Green' }
Write-Host ("Free:         {0,10:N0} bytes" -f $free) -ForegroundColor $freeColor

# === 侵害チェック ===
Write-Host ""
Write-Host "=== Memory Boundary Check ===" -ForegroundColor Cyan

$hasError = $false

# BSS開始がフレームバッファ2の終端より前か
if ($bss -lt $FB2_END) {
    Write-Host "[ERROR] BSS start (0x$($bssAddr)) overlaps Frame Buffer 2!" -ForegroundColor Red
    $hasError = $true
} else {
    Write-Host "[OK] BSS start is after Frame Buffer 2 end" -ForegroundColor Green
}

# BSS終端がRAM領域を超えていないか
if ($ebss -gt $BSS_REGION_END) {
    Write-Host "[ERROR] BSS end (0x$($ebssAddr)) exceeds RAM region (0x$($BSS_REGION_END.ToString('X8')))!" -ForegroundColor Red
    Write-Host "        Overflow: $($ebss - $BSS_REGION_END) bytes" -ForegroundColor Red
    $hasError = $true
} else {
    Write-Host "[OK] BSS end is within RAM region" -ForegroundColor Green
}

# フレームバッファ1,2の重複チェック
if ($FB2_END -gt $FB1_START -and $FB2_START -lt $FB1_END) {
    Write-Host "[ERROR] Frame Buffer 1 and 2 overlap!" -ForegroundColor Red
    $hasError = $true
} else {
    Write-Host "[OK] Frame Buffers 1 and 2 are separate" -ForegroundColor Green
}

# === 警告・推奨 ===
Write-Host ""
if ($hasError) {
    Write-Host "!!! MEMORY CORRUPTION RISK DETECTED !!!" -ForegroundColor Red
    exit 1
} elseif ($free -lt 256) {
    Write-Host "WARNING: RAM is critically low! Less than 256 bytes free." -ForegroundColor Red
    Write-Host "Consider using High RAM unused region (0x00864000-0x0087FFFF, ~112KB)" -ForegroundColor Yellow
} elseif ($free -lt 1024) {
    Write-Host "CAUTION: RAM is running low. Less than 1KB free." -ForegroundColor Yellow
} else {
    Write-Host "RAM status: OK" -ForegroundColor Green
}

# 補足情報
Write-Host ""
Write-Host "Note: rx-elf-size 'bss' column shows total .bss symbol sizes," -ForegroundColor DarkGray
Write-Host "      which may differ from actual linker placement." -ForegroundColor DarkGray
Write-Host "      This script shows the ACTUAL RAM usage from map file." -ForegroundColor DarkGray
