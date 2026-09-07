<#
.SYNOPSIS
  memory_digest を Windows タスクスケジューラに毎晩実行として登録する。

.DESCRIPTION
  既定は毎日 03:10。母艦がスリープ中でも復帰後に実行されるよう
  StartWhenAvailable を有効にしてある（バッテリー条件は無効化）。

  API キーはタスクの引数に書かず、ユーザー環境変数 LLAMA_API_KEY から読む。
  先に一度だけ以下を実行しておくこと:
      [Environment]::SetEnvironmentVariable("LLAMA_API_KEY", "Na6ce…", "User")

.EXAMPLE
  # 管理者権限は不要（自分のユーザーとして登録）
  .\register-task.ps1
  .\register-task.ps1 -At "02:30"
  .\register-task.ps1 -Unregister
#>
[CmdletBinding()]
param(
    [string]$TaskName = "QwenMemoryDigest",
    [string]$At = "03:10",
    [switch]$Unregister
)

$ErrorActionPreference = "Stop"
$here = Split-Path -Parent $MyInvocation.MyCommand.Path
$runner = Join-Path $here "run-memory-digest.ps1"

if ($Unregister) {
    Unregister-ScheduledTask -TaskName $TaskName -Confirm:$false
    Write-Host "タスク $TaskName を削除した"
    exit 0
}

if (-not (Test-Path $runner)) { throw "ランナーが見つからない: $runner" }

$action = New-ScheduledTaskAction `
    -Execute "powershell.exe" `
    -Argument "-NoProfile -ExecutionPolicy Bypass -File `"$runner`"" `
    -WorkingDirectory $here

$trigger = New-ScheduledTaskTrigger -Daily -At $At

$settings = New-ScheduledTaskSettingsSet `
    -StartWhenAvailable `
    -DontStopIfGoingOnBatteries `
    -AllowStartIfOnBatteries `
    -ExecutionTimeLimit (New-TimeSpan -Hours 2) `
    -MultipleInstances IgnoreNew

Register-ScheduledTask `
    -TaskName $TaskName `
    -Action $action `
    -Trigger $trigger `
    -Settings $settings `
    -Description "Open WebUI のチャットから事実を抽出し qwen38-1080ti/memory/*.md を更新する" `
    -Force | Out-Null

Write-Host "タスク $TaskName を毎日 $At に登録した"
Write-Host "手動実行:   Start-ScheduledTask -TaskName $TaskName"
Write-Host "状態確認:   Get-ScheduledTaskInfo -TaskName $TaskName"
Write-Host "ログ:       $here\logs\digest_YYYY-MM-DD.log"
