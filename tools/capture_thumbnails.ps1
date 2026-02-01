<#
.SYNOPSIS
    GUISimulationウィンドウのスクリーンショットを自動取得
.DESCRIPTION
    Firmware/aw*フォルダを自動検出し、各バリアントのSimulationを起動してサムネイル画像をキャプチャ
    GUI_Libが存在しない場合はAppWizardインストール先から自動コピー
    GUISimulation.exeが存在しない場合は自動ビルド
.EXAMPLE
    .\capture_thumbnails.ps1 -Version "1.0.0"
#>

param(
    [string]$OutputDir = "$PSScriptRoot\..\test-release",
    [string]$Version = ""
)

$baseDir = "$PSScriptRoot\..\Firmware"

# Find AppWizard installation
function Find-AppWizardPath {
    $searchPaths = @(
        "C:\Program Files (x86)\SEGGER\AppWizard*",
        "C:\Program Files\SEGGER\AppWizard*"
    )
    foreach ($path in $searchPaths) {
        $found = Get-ChildItem $path -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending | Select-Object -First 1
        if ($found) {
            return $found.FullName
        }
    }
    return $null
}

# Find MSBuild
function Find-MSBuild {
    $searchPaths = @(
        "C:\Program Files\Microsoft Visual Studio\*\*\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\*\*\MSBuild\Current\Bin\MSBuild.exe"
    )
    foreach ($pattern in $searchPaths) {
        $found = Get-ChildItem $pattern -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($found) {
            return $found.FullName
        }
    }
    return $null
}

# Setup GUI_Lib for a variant
function Setup-GUILib {
    param([string]$VariantPath)
    
    $guiLibDest = "$VariantPath\Simulation\GUI_Lib"
    if (Test-Path $guiLibDest) {
        Write-Host "  GUI_Lib already exists" -ForegroundColor Green
        return $true
    }
    
    $appWizardPath = Find-AppWizardPath
    if (-not $appWizardPath) {
        Write-Host "  ERROR: AppWizard not found. Please install SEGGER AppWizard." -ForegroundColor Red
        return $false
    }
    
    # Look for GUI_Lib in AppWizard sample projects
    $sampleGuiLib = Get-ChildItem "$appWizardPath" -Recurse -Directory -Filter "GUI_Lib" -ErrorAction SilentlyContinue | Select-Object -First 1
    if (-not $sampleGuiLib) {
        # Alternative: Copy from desktop backup if exists
        $backupPath = "$env:USERPROFILE\Desktop\FULLMONI-WIDE_aw003\Firmware\aw001\Simulation\GUI_Lib"
        if (Test-Path $backupPath) {
            Write-Host "  Copying GUI_Lib from backup..." -ForegroundColor Yellow
            Copy-Item $backupPath $guiLibDest -Recurse -Force
            return $true
        }
        Write-Host "  ERROR: GUI_Lib source not found" -ForegroundColor Red
        return $false
    }
    
    Write-Host "  Copying GUI_Lib from AppWizard..." -ForegroundColor Yellow
    Copy-Item $sampleGuiLib.FullName $guiLibDest -Recurse -Force
    return $true
}

# Build GUISimulation.exe
function Build-Simulation {
    param([string]$VcxprojPath)
    
    $msbuild = Find-MSBuild
    if (-not $msbuild) {
        Write-Host "  ERROR: MSBuild not found. Please install Visual Studio." -ForegroundColor Red
        return $false
    }
    
    Write-Host "  Building with MSBuild..." -ForegroundColor Yellow
    # Override WindowsTargetPlatformVersion to use installed SDK (10.0), and upgrade PlatformToolset
    $result = & $msbuild $VcxprojPath /p:Configuration=Release /p:Platform=Win32 /p:WindowsTargetPlatformVersion=10.0 /p:PlatformToolset=v145 /t:Build /v:minimal 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  ERROR: Build failed" -ForegroundColor Red
        Write-Host $result
        return $false
    }
    Write-Host "  Build successful" -ForegroundColor Green
    return $true
}

Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

# Win32 API for window manipulation (with ForceSetForegroundWindow using AttachThreadInput trick)
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class Win32 {
    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);

    [DllImport("user32.dll")]
    public static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

    [DllImport("user32.dll")]
    public static extern IntPtr GetForegroundWindow();

    [DllImport("user32.dll")]
    public static extern uint GetWindowThreadProcessId(IntPtr hWnd, IntPtr lpdwProcessId);

    [DllImport("kernel32.dll")]
    public static extern uint GetCurrentThreadId();

    [DllImport("user32.dll")]
    public static extern bool AttachThreadInput(uint idAttach, uint idAttachTo, bool fAttach);

    [DllImport("user32.dll")]
    public static extern bool BringWindowToTop(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern void keybd_event(byte bVk, byte bScan, uint dwFlags, int dwExtraInfo);

    public const byte VK_MENU = 0x12;
    public const uint KEYEVENTF_KEYUP = 0x0002;

    [StructLayout(LayoutKind.Sequential)]
    public struct RECT {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    // Force window to foreground using Alt key trick and AttachThreadInput
    public static void ForceSetForegroundWindow(IntPtr hWnd) {
        // Press and release Alt key to bypass Windows foreground restrictions
        keybd_event(VK_MENU, 0, 0, 0);
        keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

        IntPtr foregroundWnd = GetForegroundWindow();
        uint foregroundThread = GetWindowThreadProcessId(foregroundWnd, IntPtr.Zero);
        uint currentThread = GetCurrentThreadId();

        if (foregroundThread != currentThread) {
            AttachThreadInput(currentThread, foregroundThread, true);
            SetForegroundWindow(hWnd);
            BringWindowToTop(hWnd);
            AttachThreadInput(currentThread, foregroundThread, false);
        } else {
            SetForegroundWindow(hWnd);
            BringWindowToTop(hWnd);
        }
    }
}
"@

function Capture-Window {
    param(
        [string]$ExePath,
        [string]$OutputPath,
        [int]$WaitSeconds = 5
    )

    Write-Host "Starting: $ExePath" -ForegroundColor Cyan

    # Start the process
    $process = Start-Process -FilePath $ExePath -PassThru

    # Wait for window to initialize
    Write-Host "  Waiting ${WaitSeconds}s for initialization..."
    Start-Sleep -Seconds $WaitSeconds

    # Refresh process to get main window handle
    $process.Refresh()
    $hwnd = $process.MainWindowHandle

    if ($hwnd -eq [IntPtr]::Zero) {
        Write-Host "  ERROR: Could not get window handle" -ForegroundColor Red
        $process | Stop-Process -Force
        return $false
    }

    # Bring window to front using force method
    [Win32]::ShowWindow($hwnd, 9) | Out-Null  # SW_RESTORE
    Start-Sleep -Milliseconds 200
    [Win32]::ShowWindow($hwnd, 5) | Out-Null  # SW_SHOW
    Start-Sleep -Milliseconds 200
    [Win32]::ForceSetForegroundWindow($hwnd)  # Use force method
    Start-Sleep -Milliseconds 500
    
    # Click on the window to ensure it's focused (simulate mouse)
    Add-Type -AssemblyName System.Windows.Forms
    $rect = New-Object Win32+RECT
    [Win32]::GetWindowRect($hwnd, [ref]$rect) | Out-Null
    $centerX = $rect.Left + ($rect.Right - $rect.Left) / 2
    $centerY = $rect.Top + ($rect.Bottom - $rect.Top) / 2
    [System.Windows.Forms.Cursor]::Position = New-Object System.Drawing.Point([int]$centerX, [int]$centerY)
    Start-Sleep -Milliseconds 100
    
    # Final foreground attempt using force method
    [Win32]::ForceSetForegroundWindow($hwnd)
    Start-Sleep -Milliseconds 1000

    # Get window rectangle
    [Win32]::GetWindowRect($hwnd, [ref]$rect) | Out-Null

    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top

    Write-Host "  Window size: ${width}x${height}"

    # Calculate LCD area (800x256) - centered in window with title bar offset
    $lcdWidth = 800
    $lcdHeight = 256
    $offsetX = 35  # Fixed offset for window border
    $offsetY = 75  # Title bar + top margin

    Write-Host "  Capturing LCD area at offset ($offsetX, $offsetY)"

    # Capture screenshot of LCD area only
    $bitmap = New-Object System.Drawing.Bitmap($lcdWidth, $lcdHeight)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.CopyFromScreen($rect.Left + $offsetX, $rect.Top + $offsetY, 0, 0, $bitmap.Size)
    $graphics.Dispose()

    # Save as PNG
    $bitmap.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $bitmap.Dispose()

    Write-Host "  Saved: $OutputPath" -ForegroundColor Green

    # Close the simulation
    $process | Stop-Process -Force
    Start-Sleep -Milliseconds 500

    return $true
}

# Main
Write-Host "======================================" -ForegroundColor Yellow
Write-Host "  Thumbnail Capture Tool" -ForegroundColor Yellow
Write-Host "======================================" -ForegroundColor Yellow

# Ensure output directory exists
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# 自動的にaw*フォルダを検出してバリアントリストを生成（awはリンクフォルダなので除外）
$variants = @()
$awFolders = Get-ChildItem -Path $baseDir -Directory | Where-Object { $_.Name -match '^aw\d+$' } | Sort-Object Name
foreach ($awFolder in $awFolders) {
    $vcxproj = "$($awFolder.FullName)\Simulation\Simulation_VS2015_2017_Lib.vcxproj"
    if (Test-Path $vcxproj) {
        $name = $awFolder.Name
        if ([string]::IsNullOrEmpty($Version)) {
            $thumbName = "thumbnail_${name}.png"
        } else {
            $thumbName = "thumbnail_v${Version}_${name}.png"
        }
        $variants += @{
            Name = $name
            Path = $awFolder.FullName
            Vcxproj = $vcxproj
            Exe = "$($awFolder.FullName)\Simulation\Exe\GUISimulation.exe"
            Output = "$OutputDir\$thumbName"
        }
    }
}

if ($variants.Count -eq 0) {
    Write-Host "ERROR: No aw* variants found with Simulation projects" -ForegroundColor Red
    exit 1
}

Write-Host "Found $($variants.Count) variant(s): $($variants.Name -join ', ')" -ForegroundColor Cyan

# First, close any existing GUISimulation processes
Get-Process -Name "GUISimulation" -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Seconds 1

$results = @()

foreach ($variant in $variants) {
    Write-Host ""
    Write-Host "Processing $($variant.Name)..." -ForegroundColor Yellow

    # Step 1: Setup GUI_Lib if needed
    if (-not (Setup-GUILib -VariantPath $variant.Path)) {
        Write-Host "  Skipping $($variant.Name) due to missing GUI_Lib" -ForegroundColor Red
        continue
    }

    # Step 2: Build if exe doesn't exist
    if (-not (Test-Path $variant.Exe)) {
        Write-Host "  GUISimulation.exe not found, building..." -ForegroundColor Yellow
        if (-not (Build-Simulation -VcxprojPath $variant.Vcxproj)) {
            Write-Host "  Skipping $($variant.Name) due to build failure" -ForegroundColor Red
            continue
        }
    }

    # Step 3: Verify exe exists
    if (-not (Test-Path $variant.Exe)) {
        Write-Host "  ERROR: Executable still not found after build: $($variant.Exe)" -ForegroundColor Red
        continue
    }

    # Step 4: Capture (wait longer for resource loading)
    $success = Capture-Window -ExePath $variant.Exe -OutputPath $variant.Output -WaitSeconds 5

    if ($success) {
        $fileInfo = Get-Item $variant.Output
        $results += @{
            Name = $variant.Name
            Path = $variant.Output
            Size = $fileInfo.Length
        }
    }
}

Write-Host ""
Write-Host "======================================" -ForegroundColor Yellow
Write-Host "  Results" -ForegroundColor Yellow
Write-Host "======================================" -ForegroundColor Yellow

foreach ($r in $results) {
    Write-Host "  $($r.Name): $($r.Path) ($($r.Size) bytes)" -ForegroundColor Green
}

Write-Host ""
Write-Host "Done!" -ForegroundColor Green
