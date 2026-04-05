param(
    [string]$RepoRoot = (Split-Path $PSScriptRoot -Parent)
)

$ErrorActionPreference = 'Stop'

$fwDir = Join-Path $RepoRoot 'Firmware'

function Get-RuleSection {
    param(
        [string]$MkPath,
        [string]$DefaultRule
    )

    if (Test-Path $MkPath) {
        $raw = Get-Content -Path $MkPath -Raw
        $marker = '# Each subdirectory must supply rules for building sources it contributes'
        $idx = $raw.IndexOf($marker)
        if ($idx -ge 0) {
            return $raw.Substring($idx).TrimEnd("`r", "`n")
        }
    }

    return $DefaultRule
}

function Make-VarBlock {
    param(
        [string]$Name,
        [string[]]$Items
    )

    if ($Items.Count -eq 0) {
        return @()
    }

    $lines = @("$Name += \")
    for ($i = 0; $i -lt $Items.Count; $i++) {
        if ($i -lt $Items.Count - 1) {
            $lines += "$($Items[$i]) \" 
        } else {
            $lines += "$($Items[$i]) "
        }
    }
    return $lines
}

function Sync-SubdirMk {
    param(
        [string]$MkPath,
        [string]$SourceDir,
        [string]$SrcPrefix,
        [string]$ObjPrefix,
        [string]$DefaultRule
    )

    $ruleSection = Get-RuleSection -MkPath $MkPath -DefaultRule $DefaultRule

    $sources = @()
    if (Test-Path $SourceDir) {
        $sources = Get-ChildItem -Path $SourceDir -Filter '*.c' -File |
            Sort-Object Name |
            ForEach-Object { $_.Name }
    }

    $srcItems = $sources | ForEach-Object { "$SrcPrefix/$_" }
    $lstItems = $sources | ForEach-Object { [System.IO.Path]::GetFileNameWithoutExtension($_) + '.lst' }
    $depItems = $sources | ForEach-Object { "$ObjPrefix/" + [System.IO.Path]::GetFileNameWithoutExtension($_) + '.d' }
    $objItems = $sources | ForEach-Object { "$ObjPrefix/" + [System.IO.Path]::GetFileNameWithoutExtension($_) + '.o' }

    $out = @(
        '################################################################################',
        '# Automatically-generated file. Do not edit!',
        '################################################################################',
        '',
        '# Add inputs and outputs from these tool invocations to the build variables '
    )

    $out += Make-VarBlock -Name 'C_SRCS' -Items $srcItems
    if ($srcItems.Count -gt 0) { $out += '' }

    $out += Make-VarBlock -Name 'LST' -Items $lstItems
    if ($lstItems.Count -gt 0) { $out += '' }

    $out += Make-VarBlock -Name 'C_DEPS' -Items $depItems
    if ($depItems.Count -gt 0) { $out += '' }

    $out += Make-VarBlock -Name 'OBJS' -Items $objItems
    if ($objItems.Count -gt 0) { $out += '' }

    $out += @(
        'MAP += \',
        'Firmware.map ',
        '',
        '',
        $ruleSection
    )

    $content = ($out -join "`r`n") + "`r`n"
    Set-Content -Path $MkPath -Value $content -Encoding UTF8
}

Sync-SubdirMk `
    -MkPath (Join-Path $fwDir 'HardwareDebug/src/ui/subdir.mk') `
    -SourceDir (Join-Path $fwDir 'src/ui') `
    -SrcPrefix '../src/ui' `
    -ObjPrefix './src/ui' `
    -DefaultRule @'
# Each subdirectory must supply rules for building sources it contributes
src/ui/%.o: ../src/ui/%.c
'@

Sync-SubdirMk `
    -MkPath (Join-Path $fwDir 'HardwareDebug/src/ui/images/subdir.mk') `
    -SourceDir (Join-Path $fwDir 'src/ui/images') `
    -SrcPrefix '../src/ui/images' `
    -ObjPrefix './src/ui/images' `
    -DefaultRule @'
# Each subdirectory must supply rules for building sources it contributes
src/ui/images/%.o: ../src/ui/images/%.c
'@

Sync-SubdirMk `
    -MkPath (Join-Path $fwDir 'HardwareDebug/src/ui/fonts/subdir.mk') `
    -SourceDir (Join-Path $fwDir 'src/ui/fonts') `
    -SrcPrefix '../src/ui/fonts' `
    -ObjPrefix './src/ui/fonts' `
    -DefaultRule @'
# Each subdirectory must supply rules for building sources it contributes
src/ui/fonts/%.o: ../src/ui/fonts/%.c
'@

Sync-SubdirMk `
    -MkPath (Join-Path $fwDir 'HardwareDebug/src/ui_binding/subdir.mk') `
    -SourceDir (Join-Path $fwDir 'src/ui_binding') `
    -SrcPrefix '../src/ui_binding' `
    -ObjPrefix './src/ui_binding' `
    -DefaultRule @'
# Each subdirectory must supply rules for building sources it contributes
src/ui_binding/%.o: ../src/ui_binding/%.c
'@

Write-Host 'Synchronized UI-related subdir.mk files for active design.' -ForegroundColor Green
