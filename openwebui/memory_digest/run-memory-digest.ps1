<#
.SYNOPSIS
  memory_digest.py を母艦で実行するラッパー。Windows タスクスケジューラから呼ぶ想定。

.DESCRIPTION
  - llama-server が落ちていれば memory_digest.py 側が何もせず正常終了する
  - Docker Desktop が起動していない場合はここで検出して終了する（深夜にエラーを積まない）
  - 実行ログを logs/ に日付つきで残す

.EXAMPLE
  .\run-memory-digest.ps1                       # 通常実行（既定 24 時間分）
  .\run-memory-digest.ps1 -DryRun               # 書き込まず内容だけ確認
  .\run-memory-digest.ps1 -Consolidate          # 今回かならず profile/projects へ統合
  .\run-memory-digest.ps1 -WindowHours 168      # 直近1週間分をまとめて処理
#>
[CmdletBinding()]
param(
    [string]$MemoryDir = "C:\Users\tomoy\Git\qwen38-1080ti\memory",
    [string]$ApiBase   = "http://100.87.81.4:8080",
    # API キーは環境変数 LLAMA_API_KEY で渡すのが安全。引数にも書けるが履歴に残る点に注意
    [string]$ApiKey    = $env:LLAMA_API_KEY,
    [string]$Container = "open-webui",
    [double]$WindowHours = 24,
    [switch]$Consolidate,
    [switch]$NoConsolidate,
    [switch]$DryRun,
    [switch]$Verbose2,
    [int]$Probe = 0
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$script = Join-Path $here "memory_digest.py"

# ---- ログ
$logDir = Join-Path $here "logs"
if (-not (Test-Path $logDir)) { New-Item -ItemType Directory -Path $logDir | Out-Null }
$logFile = Join-Path $logDir ("digest_{0:yyyy-MM-dd}.log" -f (Get-Date))

function Write-Log([string]$msg) {
    $line = "[{0:yyyy-MM-dd HH:mm:ss}] {1}" -f (Get-Date), $msg
    Write-Host $line
    [IO.File]::AppendAllText($logFile, $line + "`r`n", [Text.UTF8Encoding]::new($true))
}

# ---- python を探す
$python = $null
foreach ($cand in @("python", "py")) {
    $cmd = Get-Command $cand -ErrorAction SilentlyContinue
    if ($cmd) { $python = $cmd.Source; break }
}
if (-not $python) { Write-Log "python が見つからない。中止"; exit 2 }

# ---- Docker Desktop の生存確認
try {
    $null = & docker inspect $Container --format '{{.State.Running}}' 2>$null
    if ($LASTEXITCODE -ne 0) { Write-Log "コンテナ $Container が見つからない（Docker 未起動？）。中止"; exit 0 }
} catch {
    Write-Log "docker コマンドが使えない。中止"; exit 0
}

# ---- 引数を組み立て
$argsList = @(
    $script,
    "--memory-dir", $MemoryDir,
    "--api-base",   $ApiBase,
    "--container",  $Container,
    "--window-hours", $WindowHours
)
if ($ApiKey)        { $argsList += @("--api-key", $ApiKey) }
if ($Consolidate)   { $argsList += "--consolidate" }
if ($NoConsolidate) { $argsList += "--no-consolidate" }
if ($DryRun)        { $argsList += "--dry-run" }
if ($Verbose2)      { $argsList += "--verbose" }
if ($Probe -gt 0)   { $argsList += @("--probe", $Probe) }

Write-Log "開始: $python $script (window=${WindowHours}h)"

# 子プロセスの出力を UTF-8 で受け取る
$prevEncoding = [Console]::OutputEncoding
[Console]::OutputEncoding = [Text.UTF8Encoding]::new($false)
$env:PYTHONIOENCODING = "utf-8"
try {
    & $python @argsList 2>&1 | ForEach-Object { Write-Log $_ }
    $code = $LASTEXITCODE
} finally {
    [Console]::OutputEncoding = $prevEncoding
}

Write-Log "終了コード: $code"

# ---- 古いログの掃除（30日）
Get-ChildItem $logDir -Filter "digest_*.log" |
    Where-Object { $_.LastWriteTime -lt (Get-Date).AddDays(-30) } |
    Remove-Item -Force -ErrorAction SilentlyContinue

exit $code
