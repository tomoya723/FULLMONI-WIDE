<#
.SYNOPSIS
    FULLMONI-WIDE リリース自動化スクリプト

.DESCRIPTION
    以下を自動で実行します：
    1. test-release/ をクリーンアップ
    2. ファームウェア全バリアントをビルド
    3. サムネイル画像を生成
    4. ホストアプリをビルド・パッケージ
    5. release-manifest.json を生成
    6. 全ファイルの整合性を検証
    7. (オプション) GitHub Release を作成・アップロード

    スキップしたコンポーネントは既存のrelease-manifest.jsonから
    情報を引き継ぎます（ハイブリッドバージョン管理）。

.EXAMPLE
    .\release.ps1 -Version "1.0.1"
    # → 全コンポーネントをビルド

    .\release.ps1 -Version "1.0.1" -SkipHostApp
    # → FWのみビルド、HostAppは既存バージョンを維持

    .\release.ps1 -Version "1.0.1" -SkipFirmware -SkipThumbnails
    # → HostAppのみビルド、FWは既存バージョンを維持

    .\release.ps1 -Version "1.0.1" -Upload
    # → 全ビルド後、GitHubにアップロード

.PARAMETER Version
    リリースバージョン（例: "1.0.1"）

.PARAMETER Upload
    GitHub Release を作成してアップロードする

.PARAMETER SkipFirmware
    ファームウェアビルドをスキップ（既存バージョンを維持）

.PARAMETER SkipThumbnails
    サムネイル生成をスキップ

.PARAMETER SkipHostApp
    Windowsホストアプリビルドをスキップ（既存バージョンを維持）

.PARAMETER SkipAndroidApp
    Androidアプリビルドをスキップ（既存バージョンを維持）
#>

param(
    [Parameter(Mandatory = $true)]
    [string]$Version,

    [switch]$Upload,
    [switch]$SkipFirmware,
    [switch]$SkipThumbnails,
    [switch]$SkipHostApp,
    [switch]$SkipAndroidApp
)

$ErrorActionPreference = "Stop"

# パス設定
$ScriptDir = $PSScriptRoot
$RootDir = Resolve-Path "$ScriptDir\.."
$OutputDir = "$RootDir\test-release"
$FirmwareDir = "$RootDir\Firmware"
$HostAppDir = "$RootDir\HostApp\FULLMONI-WIDE-Terminal"
$AndroidAppDir = "$RootDir\HostApp\FULLMONI-WIDE-Android"

# 色付きメッセージ
function Write-Step { param($msg) Write-Host "`n=== $msg ===" -ForegroundColor Cyan }
function Write-Success { param($msg) Write-Host "✓ $msg" -ForegroundColor Green }
function Write-Warn { param($msg) Write-Host "⚠ $msg" -ForegroundColor Yellow }
function Write-Fail { param($msg) Write-Host "✗ $msg" -ForegroundColor Red }

# バージョン形式チェック
if ($Version -notmatch '^\d+\.\d+\.\d+$') {
    Write-Fail "バージョン形式が不正です: $Version (例: 1.0.1)"
    exit 1
}

Write-Host "========================================"  -ForegroundColor Magenta
Write-Host "  FULLMONI-WIDE Release v$Version"        -ForegroundColor Magenta
Write-Host "========================================"  -ForegroundColor Magenta

#----------------------------------------------------------------------
# Step 0: 既存マニフェストの読み込み & クリーンアップ
#----------------------------------------------------------------------
Write-Step "Step 0: 準備"

# 既存マニフェストを読み込み（スキップ時の引き継ぎ用）
$existingManifestPath = "$RootDir\release-manifest.json"
$existingManifest = $null
if (Test-Path $existingManifestPath) {
    $existingManifest = Get-Content $existingManifestPath -Raw | ConvertFrom-Json
    Write-Success "既存マニフェストを読み込みました (v$($existingManifest.version))"
}

if (Test-Path $OutputDir) {
    # スキップするコンポーネント以外を削除
    $keepPatterns = @("Bootloader*")
    if ($SkipHostApp) { $keepPatterns += "FULLMONI-WIDE-Terminal*" }
    if ($SkipFirmware) { $keepPatterns += "Firmware_*"; $keepPatterns += "thumbnail_*" }
    
    Get-ChildItem $OutputDir -File | Where-Object {
        $file = $_
        -not ($keepPatterns | Where-Object { $file.Name -like $_ })
    } | Remove-Item -Force
    Write-Success "古いリリースファイルを削除しました"
} else {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
    Write-Success "test-release/ を作成しました"
}

#----------------------------------------------------------------------
# Step 1: ファームウェアバージョン更新
#----------------------------------------------------------------------
Write-Step "Step 1: ファームウェアバージョン更新"

$versionParts = $Version.Split('.')
$major = $versionParts[0]
$minor = $versionParts[1]
$patch = $versionParts[2]

$versionFile = "$FirmwareDir\src\firmware_version.h"
$versionContent = Get-Content $versionFile -Raw

# バージョン番号を更新
$versionContent = $versionContent -replace '(#define FW_VERSION_MAJOR\s+)\d+', "`${1}$major"
$versionContent = $versionContent -replace '(#define FW_VERSION_MINOR\s+)\d+', "`${1}$minor"
$versionContent = $versionContent -replace '(#define FW_VERSION_PATCH\s+)\d+', "`${1}$patch"
[System.IO.File]::WriteAllText($versionFile, $versionContent, [System.Text.Encoding]::UTF8)

Write-Success "firmware_version.h を v$Version に更新"

#----------------------------------------------------------------------
# Step 2: ファームウェアビルド
#----------------------------------------------------------------------
if (-not $SkipFirmware) {
    Write-Step "Step 2: ファームウェアビルド"

    & "$ScriptDir\build_variants.ps1" -Version "v$Version" -OutputDir $OutputDir

    if ($LASTEXITCODE -ne 0) {
        Write-Fail "ファームウェアビルドに失敗しました"
        exit 1
    }

    # マジックナンバー検証
    $variants = Get-ChildItem "$OutputDir\Firmware_v${Version}_*.bin"
    foreach ($bin in $variants) {
        $bytes = [System.IO.File]::ReadAllBytes($bin.FullName)
        $magic = [System.Text.Encoding]::ASCII.GetString($bytes[0..3])
        if ($magic -ne "WFXR") {
            Write-Fail "$($bin.Name): マジックナンバーが不正 ($magic)"
            exit 1
        }
        Write-Success "$($bin.Name): マジックナンバー OK (WFXR)"
    }
} else {
    Write-Warn "ファームウェアビルドをスキップ"
}

#----------------------------------------------------------------------
# Step 3: サムネイル生成
#----------------------------------------------------------------------
if (-not $SkipThumbnails) {
    Write-Step "Step 3: サムネイル生成"

    & "$ScriptDir\capture_thumbnails.ps1" -Version "$Version"

    $thumbnails = Get-ChildItem "$OutputDir\thumbnail_v${Version}_*.png" -ErrorAction SilentlyContinue
    if ($thumbnails.Count -eq 0) {
        Write-Warn "サムネイルが生成されませんでした（手動で確認してください）"
    } else {
        foreach ($thumb in $thumbnails) {
            Write-Success "$($thumb.Name): $([math]::Round($thumb.Length / 1KB, 1)) KB"
        }
    }
} else {
    Write-Warn "サムネイル生成をスキップ"
}

#----------------------------------------------------------------------
# Step 4: ホストアプリビルド
#----------------------------------------------------------------------
if (-not $SkipHostApp) {
    Write-Step "Step 4: ホストアプリビルド"

    Push-Location $HostAppDir
    try {
        # csprojのバージョンを更新
        $csprojPath = "$HostAppDir\FULLMONI-WIDE-Terminal.csproj"
        $csprojContent = Get-Content $csprojPath -Raw
        $csprojContent = $csprojContent -replace '<Version>[^<]+</Version>', "<Version>$Version</Version>"
        [System.IO.File]::WriteAllText($csprojPath, $csprojContent, [System.Text.Encoding]::UTF8)
        Write-Success "csproj バージョンを v$Version に更新"

        # クリーン
        Write-Host "  Cleaning..."
        dotnet clean -c Debug --verbosity quiet 2>&1 | Out-Null

        # ビルド＆パブリッシュ (self-contained)
        Write-Host "  Publishing..."
        $publishResult = dotnet publish -c Debug -r win-x64 --self-contained true -o "$HostAppDir\publish" 2>&1
        if ($LASTEXITCODE -ne 0) {
            Write-Fail "ホストアプリのパブリッシュに失敗しました"
            $publishResult | Write-Host
            exit 1
        }

        # ZIP作成
        $zipPath = "$OutputDir\FULLMONI-WIDE-Terminal-win-x64.zip"
        if (Test-Path $zipPath) { Remove-Item $zipPath -Force }

        Compress-Archive -Path "$HostAppDir\publish\*" -DestinationPath $zipPath -CompressionLevel Optimal

        $zipSize = (Get-Item $zipPath).Length
        Write-Success "FULLMONI-WIDE-Terminal-win-x64.zip: $([math]::Round($zipSize / 1MB, 1)) MB"

    } finally {
        Pop-Location
    }
} else {
    Write-Warn "Windowsホストアプリビルドをスキップ（既存バージョンを引き継ぎ）"
}

#----------------------------------------------------------------------
# Step 4.5: Androidアプリビルド
#----------------------------------------------------------------------
if (-not $SkipAndroidApp) {
    Write-Step "Step 4.5: Androidアプリビルド"

    Push-Location $AndroidAppDir
    try {
        # build.gradle.ktsのバージョンを更新
        $gradlePath = "$AndroidAppDir\app\build.gradle.kts"
        $gradleContent = Get-Content $gradlePath -Raw
        $gradleContent = $gradleContent -replace 'versionName = "[^"]+"', "versionName = `"$Version`""
        [System.IO.File]::WriteAllText($gradlePath, $gradleContent, [System.Text.Encoding]::UTF8)
        Write-Success "build.gradle.kts バージョンを v$Version に更新"

        # JAVA_HOME設定
        $env:JAVA_HOME = "C:\Program Files\Android\Android Studio\jbr"

        # クリーンビルド
        Write-Host "  Building Release APK..."
        $buildResult = & .\gradlew.bat assembleRelease 2>&1
        if ($LASTEXITCODE -ne 0) {
            Write-Fail "Androidアプリのビルドに失敗しました"
            $buildResult | Write-Host
            exit 1
        }

        # APKをコピー
        $apkSource = "$AndroidAppDir\app\build\outputs\apk\release\app-release-unsigned.apk"
        $apkDest = "$OutputDir\FULLMONI-WIDE-Terminal-android.apk"

        if (-not (Test-Path $apkSource)) {
            # unsigned APKがない場合は debug を使用
            $apkSource = "$AndroidAppDir\app\build\outputs\apk\release\app-release.apk"
            if (-not (Test-Path $apkSource)) {
                $apkSource = "$AndroidAppDir\app\build\outputs\apk\debug\app-debug.apk"
                Write-Warn "リリースAPKが見つからないため、デバッグAPKを使用"
            }
        }

        Copy-Item $apkSource $apkDest -Force
        $apkSize = (Get-Item $apkDest).Length
        Write-Success "FULLMONI-WIDE-Terminal-android.apk: $([math]::Round($apkSize / 1MB, 1)) MB"

    } finally {
        Pop-Location
    }
} else {
    Write-Warn "Androidアプリビルドをスキップ（既存バージョンを引き継ぎ）"
}

#----------------------------------------------------------------------
# Step 5: release-manifest.json 生成
#----------------------------------------------------------------------
Write-Step "Step 5: release-manifest.json 生成"

$releaseDate = Get-Date -Format "yyyy-MM-dd"

# ファームウェアバリアント情報を収集
$variants = @()
$variantMeta = @{
    "aw001" = @{ name = "type1"; description = "tomoya723 ver" }
    "aw002" = @{ name = "type2"; description = "chaketek ver" }
}

if ($SkipFirmware -and $existingManifest) {
    # 既存マニフェストからファームウェア情報を引き継ぐ
    Write-Host "  既存マニフェストからファームウェア情報を引き継ぎます"
    foreach ($v in $existingManifest.firmware.variants) {
        $variants += @{
            id = $v.id
            name = $v.name
            description = $v.description
            file = $v.file
            thumbnail = $v.thumbnail
            size = $v.size
            sha256 = $v.sha256
        }
    }
    $fwVersion = $existingManifest.firmware.variants[0].file -replace 'Firmware_v([\d.]+)_.+\.bin', '$1'
    Write-Success "ファームウェア v$fwVersion を引き継ぎ"
} else {
    $firmwareBins = Get-ChildItem "$OutputDir\Firmware_v${Version}_*.bin" -ErrorAction SilentlyContinue
    foreach ($bin in $firmwareBins) {
        $variantId = $bin.Name -replace "Firmware_v${Version}_(.+)\.bin", '$1'
        $hash = (Get-FileHash $bin.FullName -Algorithm SHA256).Hash.ToLower()
        $size = $bin.Length
        $thumbName = "thumbnail_v${Version}_$variantId.png"

        $meta = $variantMeta[$variantId]
        if (-not $meta) {
            $meta = @{ name = $variantId; description = $variantId }
        }

        $variants += @{
            id = $variantId
            name = $meta.name
            description = $meta.description
            file = $bin.Name
            thumbnail = $thumbName
            size = $size
            sha256 = $hash
        }
    }
}

# Bootloader情報（既存を維持）
$bootloaderFile = Get-ChildItem "$OutputDir\Bootloader_*.mot" -ErrorAction SilentlyContinue | Select-Object -First 1
$bootloaderInfo = @{
    version = "0.1.2"
    file = "Bootloader_v0.1.2.mot"
    size = 231944
    sha256 = "7075f2c54cc895bfcb61ad7b78dcf2e35c7796833aa1eeb38e4c11f43154864d"
}
if ($bootloaderFile) {
    $bootloaderInfo.file = $bootloaderFile.Name
    $bootloaderInfo.size = $bootloaderFile.Length
    $bootloaderInfo.sha256 = (Get-FileHash $bootloaderFile.FullName -Algorithm SHA256).Hash.ToLower()
    $bootloaderInfo.version = $bootloaderFile.Name -replace 'Bootloader_v(.+)\.mot', '$1'
}

# ホストアプリ情報
$hostAppInfo = $null
if ($SkipHostApp -and $existingManifest -and $existingManifest.hostApps.windows) {
    # 既存マニフェストからホストアプリ情報を引き継ぐ
    $existingHostApp = $existingManifest.hostApps.windows
    $hostAppInfo = @{
        version = $existingHostApp.version
        file = $existingHostApp.file
        size = $existingHostApp.size
        sha256 = $existingHostApp.sha256
    }
    Write-Success "ホストアプリ v$($existingHostApp.version) を引き継ぎ"
} else {
    $hostAppZip = Get-Item "$OutputDir\FULLMONI-WIDE-Terminal-win-x64.zip" -ErrorAction SilentlyContinue
    if ($hostAppZip) {
        $hostAppInfo = @{
            version = $Version
            file = $hostAppZip.Name
            size = $hostAppZip.Length
            sha256 = (Get-FileHash $hostAppZip.FullName -Algorithm SHA256).Hash.ToLower()
        }
    }
}

# Androidアプリ情報
$androidAppInfo = $null
if ($SkipAndroidApp -and $existingManifest -and $existingManifest.hostApps.android) {
    # 既存マニフェストからAndroidアプリ情報を引き継ぐ
    $existingAndroidApp = $existingManifest.hostApps.android
    $androidAppInfo = @{
        version = $existingAndroidApp.version
        file = $existingAndroidApp.file
        size = $existingAndroidApp.size
        minSdk = $existingAndroidApp.minSdk
        sha256 = $existingAndroidApp.sha256
    }
    Write-Success "Androidアプリ v$($existingAndroidApp.version) を引き継ぎ"
} else {
    $androidApk = Get-Item "$OutputDir\FULLMONI-WIDE-Terminal-android.apk" -ErrorAction SilentlyContinue
    if ($androidApk) {
        $androidAppInfo = @{
            version = $Version
            file = $androidApk.Name
            size = $androidApk.Length
            minSdk = 21
            sha256 = (Get-FileHash $androidApk.FullName -Algorithm SHA256).Hash.ToLower()
        }
    }
}

# マニフェスト構築
$manifest = [ordered]@{
    schemaVersion = 1
    version = $Version
    releaseDate = $releaseDate
    releaseNotes = "https://github.com/tomoya723/FULLMONI-WIDE/releases/tag/v$Version"
    bootloader = $bootloaderInfo
    firmware = @{
        minimumBootloaderVersion = $bootloaderInfo.version
        variants = $variants
    }
}

if ($hostAppInfo -or $androidAppInfo) {
    $manifest.hostApps = @{}
    if ($hostAppInfo) {
        $manifest.hostApps.windows = $hostAppInfo
    }
    if ($androidAppInfo) {
        $manifest.hostApps.android = $androidAppInfo
    }
}

# JSON出力
$manifestPath = "$OutputDir\release-manifest.json"
$manifest | ConvertTo-Json -Depth 10 | Set-Content $manifestPath -Encoding UTF8

# ルートにもコピー
Copy-Item $manifestPath "$RootDir\release-manifest.json" -Force

Write-Success "release-manifest.json を生成"

#----------------------------------------------------------------------
# Step 6: 整合性検証
#----------------------------------------------------------------------
Write-Step "Step 6: 整合性検証"

$manifest = Get-Content $manifestPath -Raw | ConvertFrom-Json
$allValid = $true

# ファームウェア検証
foreach ($variant in $manifest.firmware.variants) {
    $filePath = "$OutputDir\$($variant.file)"
    if (-not (Test-Path $filePath)) {
        Write-Fail "$($variant.file) が見つかりません"
        $allValid = $false
        continue
    }

    $actualHash = (Get-FileHash $filePath -Algorithm SHA256).Hash.ToLower()
    $actualSize = (Get-Item $filePath).Length

    if ($actualHash -ne $variant.sha256) {
        Write-Fail "$($variant.file): SHA256不一致"
        $allValid = $false
    } elseif ($actualSize -ne $variant.size) {
        Write-Fail "$($variant.file): サイズ不一致"
        $allValid = $false
    } else {
        Write-Success "$($variant.file): OK ($actualSize bytes)"
    }
}

# サムネイル検証
foreach ($variant in $manifest.firmware.variants) {
    $thumbPath = "$OutputDir\$($variant.thumbnail)"
    if (Test-Path $thumbPath) {
        Write-Success "$($variant.thumbnail): OK"
    } else {
        Write-Warn "$($variant.thumbnail): 見つかりません"
    }
}

# ホストアプリ検証
if ($manifest.hostApps.windows) {
    $hostApp = $manifest.hostApps.windows
    $filePath = "$OutputDir\$($hostApp.file)"
    if (Test-Path $filePath) {
        $actualHash = (Get-FileHash $filePath -Algorithm SHA256).Hash.ToLower()
        $actualSize = (Get-Item $filePath).Length

        if ($actualHash -ne $hostApp.sha256) {
            Write-Fail "$($hostApp.file): SHA256不一致"
            $allValid = $false
        } elseif ($actualSize -ne $hostApp.size) {
            Write-Fail "$($hostApp.file): サイズ不一致"
            $allValid = $false
        } else {
            Write-Success "$($hostApp.file): OK ($([math]::Round($actualSize / 1MB, 1)) MB)"
        }
    } else {
        Write-Fail "$($hostApp.file) が見つかりません"
        $allValid = $false
    }
}

# Androidアプリ検証
if ($manifest.hostApps.android) {
    $androidApp = $manifest.hostApps.android
    $filePath = "$OutputDir\$($androidApp.file)"
    if (Test-Path $filePath) {
        $actualHash = (Get-FileHash $filePath -Algorithm SHA256).Hash.ToLower()
        $actualSize = (Get-Item $filePath).Length

        if ($actualHash -ne $androidApp.sha256) {
            Write-Fail "$($androidApp.file): SHA256不一致"
            $allValid = $false
        } elseif ($actualSize -ne $androidApp.size) {
            Write-Fail "$($androidApp.file): サイズ不一致"
            $allValid = $false
        } else {
            Write-Success "$($androidApp.file): OK ($([math]::Round($actualSize / 1MB, 1)) MB)"
        }
    } else {
        Write-Fail "$($androidApp.file) が見つかりません"
        $allValid = $false
    }
}

if (-not $allValid) {
    Write-Fail "整合性検証に失敗しました"
    exit 1
}

#----------------------------------------------------------------------
# Step 7: GitHub Release（オプション）
#----------------------------------------------------------------------
if ($Upload) {
    Write-Step "Step 7: GitHub Release 作成"

    $tag = "v$Version"

    # gh CLI確認
    if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
        Write-Fail "gh CLI がインストールされていません"
        exit 1
    }

    # 既存リリース確認
    $existingRelease = gh release view $tag 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Warn "リリース $tag は既に存在します。アセットを更新しますか？ (Y/N)"
        $confirm = Read-Host
        if ($confirm -ne 'Y' -and $confirm -ne 'y') {
            Write-Host "アップロードをキャンセルしました"
            exit 0
        }

        # 既存アセットを削除
        $assets = @("release-manifest.json", "FULLMONI-WIDE-Terminal-win-x64.zip")
        foreach ($variant in $manifest.firmware.variants) {
            $assets += $variant.file
            $assets += $variant.thumbnail
        }

        foreach ($asset in $assets) {
            gh release delete-asset $tag $asset --yes 2>&1 | Out-Null
        }
    } else {
        # 新規リリース作成
        $releaseNotes = @"
## FULLMONI-WIDE v$Version

### ファームウェア
$(foreach ($v in $manifest.firmware.variants) { "- $($v.file) - $($v.description)" })

### 対応Bootloader
- v$($manifest.bootloader.version) 以上

### ホストアプリ
- FULLMONI-WIDE-Terminal (Windows x64)
- FULLMONI-WIDE-Terminal (Android)
"@

        gh release create $tag --title "v$Version" --notes $releaseNotes
        if ($LASTEXITCODE -ne 0) {
            Write-Fail "リリース作成に失敗しました"
            exit 1
        }
    }

    # アセットアップロード
    $uploadFiles = @("$OutputDir\release-manifest.json")

    if ($manifest.bootloader.file -and (Test-Path "$OutputDir\$($manifest.bootloader.file)")) {
        $uploadFiles += "$OutputDir\$($manifest.bootloader.file)"
    }

    foreach ($variant in $manifest.firmware.variants) {
        $uploadFiles += "$OutputDir\$($variant.file)"
        $thumbPath = "$OutputDir\$($variant.thumbnail)"
        if (Test-Path $thumbPath) {
            $uploadFiles += $thumbPath
        }
    }

    if ($manifest.hostApps.windows) {
        $uploadFiles += "$OutputDir\$($manifest.hostApps.windows.file)"
    }

    if ($manifest.hostApps.android) {
        $uploadFiles += "$OutputDir\$($manifest.hostApps.android.file)"
    }

    Write-Host "  Uploading assets..."
    gh release upload $tag $uploadFiles --clobber

    if ($LASTEXITCODE -eq 0) {
        Write-Success "GitHub Release v$Version を作成しました"
        Write-Host "  https://github.com/tomoya723/FULLMONI-WIDE/releases/tag/$tag" -ForegroundColor Blue
    } else {
        Write-Fail "アセットのアップロードに失敗しました"
        exit 1
    }
} else {
    Write-Host "`n📦 リリースファイルが準備できました: $OutputDir" -ForegroundColor Green
    Write-Host "   GitHub にアップロードするには: .\release.ps1 -Version $Version -Upload" -ForegroundColor Gray
}

#----------------------------------------------------------------------
# 完了
#----------------------------------------------------------------------
Write-Host "`n========================================"  -ForegroundColor Green
Write-Host "  リリース準備完了: v$Version"             -ForegroundColor Green
Write-Host "========================================"  -ForegroundColor Green

Write-Host "`n生成されたファイル:" -ForegroundColor Cyan
Get-ChildItem $OutputDir | ForEach-Object {
    $size = if ($_.Length -gt 1MB) { "$([math]::Round($_.Length / 1MB, 1)) MB" } else { "$([math]::Round($_.Length / 1KB, 1)) KB" }
    Write-Host "  $($_.Name) ($size)"
}
