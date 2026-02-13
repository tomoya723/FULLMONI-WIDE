<#
.SYNOPSIS
    AppWizardバリアント（aw001, aw002）のビルドを自動化

.DESCRIPTION
    Junction + 動的subdir.mk生成でバリアントを切り替えてビルドします。

.EXAMPLE
    .\build_variants.ps1
    .\build_variants.ps1 -Variants aw001
    .\build_variants.ps1 -Variants aw002,aw001

.NOTES
    このスクリプトは以下を自動化します：
    1. Junctionでソースフォルダーを切り替え
    2. subdir.mkを動的生成
    3. ビルド実行
    4. objcopyでバイナリ生成
    5. 結果をtest-releaseにコピー
    6. マニフェストのSHA256を更新
#>

param(
    [string[]]$Variants = @("aw001", "aw002"),
    [string]$OutputDir = "",
    [string]$Version = ""
)

$ErrorActionPreference = "Stop"

# パスの設定
$ScriptDir = $PSScriptRoot
$RootDir = Resolve-Path "$ScriptDir\.."
$FirmwareDir = "$RootDir\Firmware"
$BuildDir = "$FirmwareDir\HardwareDebug"
$MakeExe = "C:\Renesas\e2_studio\eclipse\plugins\com.renesas.ide.exttools.gnumake.win32.x86_64_4.3.1.v20240909-0854\mk\make.exe"

if ([string]::IsNullOrEmpty($OutputDir)) {
    $OutputDir = "$RootDir\test-release"
} else {
    # 相対パスの場合は絶対パスに変換
    if (-not [System.IO.Path]::IsPathRooted($OutputDir)) {
        $OutputDir = (Resolve-Path $OutputDir -ErrorAction SilentlyContinue).Path
        if (-not $OutputDir) {
            # パスが存在しない場合は現在のディレクトリから相対パスを解決
            $OutputDir = Join-Path (Get-Location).Path $OutputDir
        }
    }
}

$SourceJunction = "$FirmwareDir\aw"
$BuildAwDir = "$BuildDir\aw"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "FULLMONI-WIDE Variant Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# 出力ディレクトリ作成
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# インクルードパス（rx-elf-gccがPATHにあることを前提）
$IncludePaths = @(
    "-I../src",
    "-I../aw",
    "-I../aw/Resource",
    "-I../aw/Source",
    "-I../aw/Source/Generated",
    "-I../aw/Source/CustomWidgets",
    "-I../aw/Source/CustomCode",
    "-I../src/smc_gen",
    "-I../src/smc_gen/general",
    "-I../src/smc_gen/r_emwin_rx",
    "-I../src/smc_gen/r_emwin_rx/lib/Config",
    "-I../src/smc_gen/r_emwin_rx/lib/GUI",
    "-I../src/smc_gen/r_bsp",
    "-I../src/smc_gen/r_config"
)
$IncFlags = $IncludePaths -join " "

# GCCオプション（rx72tを使用）
$GccFlags = "-ffunction-sections -fdata-sections -fno-strict-aliasing -Wpointer-arith -g -mcpu=rx72t -misa=v3 -mlittle-endian-data -std=gnu99 -fmax-errors=5 -O2 -g3"

#----------------------------------------------------------------------
# subdir.mkを生成する関数
#----------------------------------------------------------------------
function Generate-SubdirMk {
    param(
        [string]$RelativeDir,    # aw配下の相対パス（例: "Source/Generated"）
        [string]$BaseSrcPath     # Junctionが指すソースパス
    )

    $srcDir = Join-Path $BaseSrcPath $RelativeDir
    $outDir = Join-Path $BuildAwDir $RelativeDir.Replace('/', '\')

    if (-not (Test-Path $srcDir)) { return }

    $cFiles = Get-ChildItem $srcDir -Filter "*.c" -File -ErrorAction SilentlyContinue
    if ($cFiles.Count -eq 0) { return }

    if (-not (Test-Path $outDir)) {
        New-Item -Path $outDir -ItemType Directory -Force | Out-Null
    }

    $relPath = "aw/$RelativeDir"
    $srcRelPath = "../aw/$RelativeDir"

    $lines = @()
    $lines += "################################################################################"
    $lines += "# Auto-generated for variant build"
    $lines += "################################################################################"
    $lines += ""
    $lines += "C_SRCS += \"
    foreach ($f in $cFiles) {
        $lines += "$srcRelPath/$($f.Name) \"
    }
    $lines[-1] = $lines[-1].TrimEnd(" \")
    $lines += ""
    $lines += "OBJS += \"
    foreach ($f in $cFiles) {
        $base = [System.IO.Path]::GetFileNameWithoutExtension($f.Name)
        $lines += "$relPath/$base.o \"
    }
    $lines[-1] = $lines[-1].TrimEnd(" \")
    $lines += ""
    $lines += "C_DEPS += \"
    foreach ($f in $cFiles) {
        $base = [System.IO.Path]::GetFileNameWithoutExtension($f.Name)
        $lines += "$relPath/$base.d \"
    }
    $lines[-1] = $lines[-1].TrimEnd(" \")
    $lines += ""

    foreach ($f in $cFiles) {
        $base = [System.IO.Path]::GetFileNameWithoutExtension($f.Name)
        $lines += "$relPath/$base.o: $srcRelPath/$($f.Name)"
        $lines += "`t@echo 'Building: `$<'"
        $lines += "`t@rx-elf-gcc -c $IncFlags $GccFlags -MMD -MP -MF`"`$(@:%.o=%.d)`" -MT`"`$@`" -o `"`$@`" `"`$<`""
        $lines += ""
    }

    $mkPath = Join-Path $outDir "subdir.mk"
    $content = $lines -join "`n"
    [System.IO.File]::WriteAllText($mkPath, $content, [System.Text.Encoding]::UTF8)

    return $cFiles.Count
}

#----------------------------------------------------------------------
# variant_id.h生成
#----------------------------------------------------------------------
function Generate-VariantIdHeader {
    param([string]$VariantId)
    
    $variantIdFile = "$FirmwareDir\src\variant_id.h"
    $content = @"
/*
 * variant_id.h - Auto-generated by build_variants.ps1
 * DO NOT EDIT - This file is regenerated on each build
 */
#ifndef VARIANT_ID_H_
#define VARIANT_ID_H_

#define BUILD_VARIANT_ID "$VariantId"
#define BUILD_VARIANT_STRING "VARIANT:$VariantId"

#endif /* VARIANT_ID_H_ */
"@
    [System.IO.File]::WriteAllText($variantIdFile, $content, [System.Text.Encoding]::UTF8)
    return $variantIdFile
}

#----------------------------------------------------------------------
# バリアント検証（バイナリ内の埋め込み文字列をチェック）
#----------------------------------------------------------------------
function Verify-VariantId {
    param(
        [string]$BinaryPath,
        [string]$ExpectedVariantId
    )
    
    $bytes = [System.IO.File]::ReadAllBytes($BinaryPath)
    $content = [System.Text.Encoding]::ASCII.GetString($bytes)
    
    $expectedString = "VARIANT:$ExpectedVariantId"
    if ($content.Contains($expectedString)) {
        return $true
    }
    return $false
}

#----------------------------------------------------------------------
# Junction切り替え
#----------------------------------------------------------------------
function Set-Junction {
    param([string]$JunctionPath, [string]$TargetPath)

    if (Test-Path $JunctionPath) {
        cmd /c rmdir "$JunctionPath" 2>$null
    }

    $result = cmd /c mklink /j "$JunctionPath" "$TargetPath" 2>&1
    return $?
}

#----------------------------------------------------------------------
# メイン処理
#----------------------------------------------------------------------
$Results = @{}

foreach ($variant in $Variants) {
    Write-Host "`n----------------------------------------" -ForegroundColor Yellow
    Write-Host "Building variant: $variant" -ForegroundColor Yellow
    Write-Host "----------------------------------------" -ForegroundColor Yellow

    $SourceAwDir = "$FirmwareDir\$variant"

    if (-not (Test-Path $SourceAwDir)) {
        Write-Host "Source directory not found: $SourceAwDir" -ForegroundColor Red
        continue
    }

    # Step 1: Junction切り替え
    Write-Host "Setting up junction: aw -> $variant"
    if (-not (Set-Junction -JunctionPath $SourceJunction -TargetPath $SourceAwDir)) {
        Write-Host "Failed to create junction" -ForegroundColor Red
        continue
    }

    # Step 1.5: variant_id.h生成
    Write-Host "Generating variant_id.h for $variant..."
    $variantIdFile = Generate-VariantIdHeader -VariantId $variant
    Write-Host "  Created: $variantIdFile"

    # Step 2: awビルドディレクトリをクリア
    if (Test-Path $BuildAwDir) {
        Remove-Item $BuildAwDir -Recurse -Force
    }

    # Step 2.5: make clean を実行（srcのオブジェクトファイルもクリア）
    Push-Location $BuildDir
    Write-Host "Cleaning previous build..."
    cmd /c "$MakeExe clean 2>&1" | Out-Null
    Pop-Location

    # Step 3: subdir.mk生成（Simulation除外）
    Write-Host "Generating subdir.mk files..."
    $totalFiles = 0
    Get-ChildItem $SourceAwDir -Recurse -Directory |
        Where-Object { $_.FullName -notlike "*Simulation*" } |
        ForEach-Object {
            $rel = $_.FullName.Replace($SourceAwDir, "").TrimStart('\').Replace('\', '/')
            $count = Generate-SubdirMk -RelativeDir $rel -BaseSrcPath $SourceAwDir
            if ($count) { $totalFiles += $count }
        }
    Write-Host "  Generated subdir.mk for $totalFiles source files"

    # Step 3.5: makefile検証（aw/ subdir.mkインクルード行が存在するか確認）
    $makefilePath = Join-Path $BuildDir "makefile"
    if (Test-Path $makefilePath) {
        $makefileContent = Get-Content $makefilePath -Raw
        if ($makefileContent -notmatch 'aw/Source/Generated/subdir\.mk') {
            Write-Host ""
            Write-Host "ERROR: makefile に aw/ subdir.mk のインクルード行がありません。" -ForegroundColor Red
            Write-Host "  e2 studio のプロジェクト設定 > パスおよびシンボル > ソース・ロケーション に" -ForegroundColor Red
            Write-Host "  /Firmware/aw/Resource と /Firmware/aw/Source が含まれているか確認してください。" -ForegroundColor Red
            Write-Host "  設定後、e2 studio で makefile を再生成してください。" -ForegroundColor Red
            throw "makefile validation failed: aw/ source locations missing"
        }
    }

    # Step 4: ビルド
    Push-Location $BuildDir
    try {
        # 既存の成果物を削除
        Remove-Item "Firmware.elf", "Firmware.bin", "Firmware.mot" -Force -ErrorAction SilentlyContinue

        Write-Host "Building..."
        # stderrをstdoutにマージし、PowerShellのエラー検出を回避
        $buildOutput = cmd /c "$MakeExe -j8 all 2>&1"
        
        # エラーチェック（リンカエラーのみ検出）
        $hasError = $buildOutput | Where-Object { $_ -match "^.+: error:" -or $_ -match "collect2.*error" }

        if (-not (Test-Path "Firmware.elf")) {
            Write-Host "Build failed for $variant" -ForegroundColor Red
            if ($hasError) {
                $hasError | Select-Object -Last 5 | ForEach-Object { Write-Host "  $_" -ForegroundColor Red }
            }
            continue
        }

        # Step 5: objcopyでFirmware.binを生成（正規Makefileと同じオプション）
        # PowerShellのワイルドカード展開を避けるためcmd.exeで実行
        Write-Host "Generating Firmware.bin..."
        cmd /c 'rx-elf-objcopy -O binary --gap-fill=0xFF -j .firmware_header -j .text -j .rvectors -j .rodata* -j .fvectors -j .data Firmware.elf Firmware.bin' 2>&1 | Out-Null

        if (-not (Test-Path "Firmware.bin")) {
            Write-Host "Failed to generate Firmware.bin" -ForegroundColor Red
            continue
        }

        # Step 5.5: バリアントID検証
        Write-Host "Verifying variant ID in binary..."
        $binFullPath = Join-Path $BuildDir "Firmware.bin"
        if (-not (Verify-VariantId -BinaryPath $binFullPath -ExpectedVariantId $variant)) {
            Write-Host "ERROR: Variant ID mismatch! Binary does not contain 'VARIANT:$variant'" -ForegroundColor Red
            Write-Host "       This is a critical build error. Aborting." -ForegroundColor Red
            exit 1
        }
        Write-Host "  Variant ID verified: VARIANT:$variant" -ForegroundColor Green

        Write-Host "Build successful!" -ForegroundColor Green

        # Step 6: 出力ファイルをコピー（バージョン付きファイル名）
        if ([string]::IsNullOrEmpty($Version)) {
            $outputBin = "Firmware_$variant.bin"
        } else {
            $outputBin = "Firmware_${Version}_$variant.bin"
        }
        $destPath = Join-Path $OutputDir $outputBin

        Copy-Item "Firmware.bin" $destPath -Force

        # SHA256計算
        $hash = (Get-FileHash $destPath -Algorithm SHA256).Hash.ToLower()
        $size = (Get-Item $destPath).Length

        $Results[$variant] = @{
            File = $outputBin
            Size = $size
            SHA256 = $hash
        }

        Write-Host "  $outputBin : $size bytes" -ForegroundColor Green
        Write-Host "  SHA256: $hash"

    } finally {
        Pop-Location
    }
}

# Step 7: Junctionクリーンアップ
if (Test-Path $SourceJunction) {
    cmd /c rmdir "$SourceJunction" 2>$null
}

# Step 8: マニフェスト更新
$manifestPath = Join-Path $OutputDir "release-manifest.json"
if ((Test-Path $manifestPath) -and ($Results.Count -gt 0)) {
    Write-Host "`nUpdating manifest..." -ForegroundColor Cyan

    $manifest = Get-Content $manifestPath -Encoding UTF8 -Raw | ConvertFrom-Json

    foreach ($variantObj in $manifest.firmware.variants) {
        if ($Results.ContainsKey($variantObj.id)) {
            $result = $Results[$variantObj.id]
            $variantObj.file = $result.File
            $variantObj.size = $result.Size
            $variantObj.sha256 = $result.SHA256
            Write-Host "  Updated: $($variantObj.id)" -ForegroundColor Green
        }
    }

    $manifest | ConvertTo-Json -Depth 10 | Set-Content $manifestPath -Encoding UTF8
}

# 結果サマリー
Write-Host "`n========================================" -ForegroundColor Green
Write-Host "Build Summary" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

if ($Results.Count -eq 0) {
    Write-Host "No successful builds" -ForegroundColor Red
    exit 1
} else {
    foreach ($key in $Results.Keys | Sort-Object) {
        $r = $Results[$key]
        Write-Host "$key : $($r.File) ($($r.Size) bytes)"
        Write-Host "       SHA256: $($r.SHA256.Substring(0, 16))..."
    }
}

Write-Host "`nDone!" -ForegroundColor Cyan
