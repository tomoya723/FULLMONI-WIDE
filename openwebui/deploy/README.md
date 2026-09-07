# deploy — Filter を DB に反映する

Open WebUI の Function の実体は **DB の `function` テーブルの `content` カラム**にある。
ディスク上の `.py` を直しても読まれないので、これまでは管理画面で全文を貼り直していた
（README §5）。出先やスマホからはそれが困難なので、DB を直接更新して再起動するまでを
1コマンドにした。

## 使い方

```powershell
cd <リポジトリ>\openwebui\deploy

.\deploy-filters.ps1 -List      # function の id 一覧を確認
.\deploy-filters.ps1 -DryRun    # 差分だけ表示（更新しない）
.\deploy-filters.ps1            # 反映して再起動
.\deploy-filters.ps1 -Only force_web_search
```

反映対象は `deploy-filters.ps1` の `$targets` に定義してある。
id が実際の DB と違う場合は `-List` で確認してから直すこと。

## 安全策

| 装置 | 内容 |
|---|---|
| バックアップ | 更新前の行を `deploy\backups\<id>_<日時>.json` に丸ごと保存 |
| id の検証 | 指定した id が無ければ**何もせず**、存在する id の一覧を表示して終了する（決め打ちしない） |
| dry-run | `-DryRun` で文字数の変化と現在の Valves だけ確認できる |
| Valves のマージ | `--valves` で指定したキーだけ上書きし、既存の設定は残す |
| 復帰確認 | 再起動後に `/health` を叩いて Open WebUI の復帰を待つ |

## 再起動が必要な理由

Open WebUI は Function のモジュールをプロセス内にキャッシュする。管理画面から保存した
場合はそこでキャッシュが落ちるが、DB を直接書き換えた場合は落ちない。したがって
**`docker restart` まで込みで1セット**。`-NoRestart` はデバッグ用。

## 戻し方

```powershell
# バックアップから content を取り出して書き戻す
$b = Get-Content .\backups\force_web_search_2026-09-07_120000.json -Raw -Encoding UTF8 | ConvertFrom-Json
Set-Content .\restore.py -Value $b.content -Encoding UTF8
.\deploy-filters.ps1 -Only force_web_search   # $targets の File を restore.py に向けてから
```

素直には管理画面に貼り戻すのが早い。バックアップ JSON の `content` がそのまま全文。
