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
# Id は DB の実値。-List で確認済み (2026-09-07):
#   force_web_search / memory_file_filter
# 記憶フィルタの id は memory_file_injector ではなく memory_file_filter
$targets = @(
    @{ Id = "force_web_search";   File = Join-Path $root "force_web_search.py";     Valves = @{ debug = $true } },
    @{ Id = "memory_file_filter"; File = Join-Path $root "memory_file_injector.py"; Valves = $null }
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
    $applied = 0
    $missing = 0
    foreach ($t in $targets) {
        if ($Only -and $t.Id -ne $Only) { continue }
        if (-not (Test-Path $t.File)) {
            Write-Host "スキップ (ファイル無し): $($t.File)" -ForegroundColor Yellow
            continue
        }

        Write-Host ""
        Write-Host "=== $($t.Id) ===" -ForegroundColor Cyan

        $a = @($script, "--file", $t.File, "--id", $t.Id, "--container", $Container)
        if ($t.Valves) {
            # PowerShell からネイティブコマンドに JSON 文字列を渡すと二重引用符が
            # 剥がれて {debug:true} になる。一時ファイル経由で渡して回避する
            $vf = Join-Path $env:TEMP ("owui_valves_{0}.json" -f $t.Id)
            [IO.File]::WriteAllText($vf, ($t.Valves | ConvertTo-Json -Compress), [Text.UTF8Encoding]::new($false))
            $a += @("--valves-file", $vf)
        }
        if ($DryRun)   { $a += "--dry-run" }
        # 再起動は最後に1回だけで足りるので、個別実行では抑止する
        $a += "--no-restart"

        & $python @a
        switch ($LASTEXITCODE) {
            0 { $applied++ }
            2 {
                # id が DB に無い。上に候補一覧が出ているので、止めずに次へ進む
                $missing++
                Write-Host "→ この id は DB に無いので飛ばした。上の一覧から正しい id を確認し、" -ForegroundColor Yellow
                Write-Host "  deploy-filters.ps1 の `$targets を直すこと" -ForegroundColor Yellow
            }
            default { throw "$($t.Id) の反映に失敗 (終了コード $LASTEXITCODE)" }
        }
    }

    Write-Host ""
    Write-Host "反映 $applied 件 / id 不一致 $missing 件" -ForegroundColor Cyan

    if ($applied -eq 0) {
        Write-Host "何も更新していないので再起動しない" -ForegroundColor Yellow
        return
    }

    if (-not $DryRun -and -not $NoRestart) {
        Write-Host ""
        Write-Host "コンテナを再起動する ..." -ForegroundColor Cyan
        # ネイティブコマンドの stderr は $ErrorActionPreference="Stop" 下で
        # 終了エラーに変換される。再起動直後はまだ応答できず stderr が出るのが
        # 正常なので、この区間だけ Continue にしてホスト側から確認する
        $prevEap = $ErrorActionPreference
        $ErrorActionPreference = "Continue"
        try {
            docker restart $Container | Out-Null
            for ($i = 0; $i -lt 40; $i++) {
                Start-Sleep -Seconds 3
                try {
                    $r = Invoke-WebRequest "http://127.0.0.1:3000/health" -UseBasicParsing -TimeoutSec 3
                    if ($r.StatusCode -eq 200) { $restarted = $true; break }
                } catch {
                    # まだ起動中。次の周回で再試行する
                }
            }
        } finally {
            $ErrorActionPreference = $prevEap
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
