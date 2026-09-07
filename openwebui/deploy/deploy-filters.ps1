<#
.SYNOPSIS
  Filter をまとめて Open WebUI の DB に反映し、コンテナを再起動する。

.DESCRIPTION
  管理画面で全文を貼り直す作業を置き換える。出先やスマホからでも1コマンドで済む。
  更新前の行は deploy\backups\ に JSON で退避される。

.EXAMPLE
  .\deploy-filters.ps1 -List          # function の id 一覧を見る
  .\deploy-filters.ps1 -DryRun        # 差分だけ確認
  .\deploy-filters.ps1                # 反映して再起動
  .\deploy-filters.ps1 -Only force_web_search
#>
[CmdletBinding()]
param(
    [string]$Container = "open-webui",
    [string]$Only = "",
    [switch]$DryRun,
    [switch]$List,
    [switch]$NoRestart
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$root = Split-Path -Parent $here          # openwebui/
$script = Join-Path $here "deploy_filter.py"

# 反映する Filter: function テーブルの id -> ローカルの .py
# id が違う場合は -List で実際の id を確認してからここを直す
$targets = @(
    @{ Id = "force_web_search";    File = Join-Path $root "force_web_search.py";    Valves = '{"debug":true}' },
    @{ Id = "memory_file_injector"; File = Join-Path $root "memory_file_injector.py"; Valves = "" }
)

$python = $null
foreach ($cand in @("python", "py")) {
    $cmd = Get-Command $cand -ErrorAction SilentlyContinue
    if ($cmd) { $python = $cmd.Source; break }
}
if (-not $python) { throw "python が見つからない" }

$env:PYTHONIOENCODING = "utf-8"
$prev = [Console]::OutputEncoding
[Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)

try {
    if ($List) {
        & $python $script --file $targets[0].File --id "__list__" --container $Container --list
        return
    }

    $restarted = $false
    foreach ($t in $targets) {
        if ($Only -and $t.Id -ne $Only) { continue }
        if (-not (Test-Path $t.File)) {
            Write-Host "スキップ (ファイル無し): $($t.File)" -ForegroundColor Yellow
            continue
        }

        Write-Host ""
        Write-Host "=== $($t.Id) ===" -ForegroundColor Cyan

        $a = @($script, "--file", $t.File, "--id", $t.Id, "--container", $Container)
        if ($t.Valves) { $a += @("--valves", $t.Valves) }
        if ($DryRun)   { $a += "--dry-run" }
        # 再起動は最後に1回だけで足りるので、個別実行では抑止する
        $a += "--no-restart"

        & $python @a
        if ($LASTEXITCODE -ne 0) { throw "$($t.Id) の反映に失敗 (終了コード $LASTEXITCODE)" }
    }

    if (-not $DryRun -and -not $NoRestart) {
        Write-Host ""
        Write-Host "コンテナを再起動する ..." -ForegroundColor Cyan
        docker restart $Container | Out-Null
        for ($i = 0; $i -lt 40; $i++) {
            Start-Sleep -Seconds 3
            docker exec $Container python -c "import urllib.request;urllib.request.urlopen('http://127.0.0.1:8080/health',timeout=3);print('ok')" 2>$null | Out-Null
            if ($LASTEXITCODE -eq 0) { $restarted = $true; break }
        }
        if ($restarted) {
            Write-Host "Open WebUI 復帰を確認した" -ForegroundColor Green
        } else {
            Write-Host "復帰確認がタイムアウトした。docker logs open-webui を確認すること" -ForegroundColor Red
        }
    }
} finally {
    [Console]::OutputEncoding = $prev
}
