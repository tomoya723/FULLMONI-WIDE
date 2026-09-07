# memory_digest — Open WebUI の会話から記憶を育てる夜間バッチ

Open WebUI のセッションに溜まった情報を、毎晩まとめて `qwen38-1080ti/memory/*.md` に
反映するためのバッチ。会話中に推論を増やさないので、応答速度に一切影響しない。

## なぜバッチなのか

記憶の自動更新には3つのやり方がある。

| 方式 | 内容 | 判定 |
|---|---|---|
| A. Filter の `outlet` で毎ターン抽出 | 応答のたびに抽出推論が走る | 会話が遅くなる。LocalMind 経由では `outlet` が呼ばれない（README §12）ので取りこぼす |
| B. 明示トリガ（「記憶して」） | 現行の `memory_file_injector.py` の実装 | 確実だが、言い忘れたら残らない |
| **C. 夜間バッチで DB から抽出** | **本ディレクトリ** | 会話が遅くならない。ブラウザ由来の会話はすべて拾える。`inbox.md` 肥大化（README §13 の残課題）も統合で同時に潰せる |

C は B を置き換えるものではなく併用する。B は「今すぐ確実に覚えさせたい」ときの手段として残す。

## 構成

```
バッチ（母艦の Python）
  ├─ docker exec で webui.db を読む   … named volume の中なのでホストから直接触れない
  │    └─ 読み取り専用。WAL のまま SELECT するだけで Open WebUI は止めない
  ├─ 直近 N 時間に更新されたチャットを抽出（state ファイルで重複除外）
  ├─ llama-server (100.87.81.4:8080) に投げて「新しく分かった事実」を抽出
  ├─ memory/inbox.md に日付つきで追記            ← 毎晩
  └─ memory/profile.md, projects.md へ統合        ← 週1（既定 7 日ごと）
```

Open WebUI 側から見た記憶ファイルは bind mount:

| ホスト | コンテナ |
|---|---|
| `C:\Users\tomoy\Git\qwen38-1080ti\memory` | `/app/memory` |

DB は named volume `open-webui`（`/var/lib/docker/volumes/open-webui/_data`）なので
ホストから直接は読めない。よって `docker cp` + `docker exec python` で吸い出している。

## 出力方針（ハイブリッド）

- **毎晩**: 抽出した事実を `inbox.md` に `## YYYY-MM-DD (自動抽出)` 見出しで追記するだけ。
  既存ファイルは書き換えないので事故が起きない。
- **週1**: `inbox.md` を `profile.md` / `projects.md` へ統合し、`inbox.md` を空にする。
  これで `inbox.md` が無限に伸びてプリフィルを食う問題が解消する。

事実は `- [カテゴリ] 内容` の形で出る。カテゴリは `profile` / `project` / `preference` / `todo`。

## 安全装置

| 装置 | 内容 |
|---|---|
| バックアップ | 書き込み前に必ず `memory/.backup/YYYY-MM-DD_HHMMSS/` へ全 `.md` を退避（既定 14 世代保持） |
| 縮小ガード | 統合結果が既存の半分未満に縮んだら「記憶を消した」とみなして中止。`--force` で解除 |
| JSON 検証 | 統合はモデルに JSON を返させる。解釈できなければ何も書かずに中止 |
| DB 読み取り専用 | `mode=ro` で開く。失敗時のみ通常オープンにフォールバックするが SELECT しか発行しない |
| llama-server 生存確認 | `/v1/models` が 200 でなければ何もせず正常終了（深夜にエラーを積まない） |
| state ファイル | `memory/.digest_state.json` に最後に処理したチャットの更新時刻を記録し、同じ会話を二度抽出しない |
| 既知事実の除外 | 既存の `profile.md` / `projects.md` / `inbox.md` を毎回プロンプトに渡し、「既に書いてあることは書くな」と指示している |

## 使い方

### 初回セットアップ

```powershell
# 1. API キーをユーザー環境変数に入れる（引数に書くとコマンド履歴に残るため）
[Environment]::SetEnvironmentVariable("LLAMA_API_KEY", "Na6ce…", "User")

# 2. 新しい PowerShell を開いてから、まず書き込まずに動作確認
cd <このディレクトリ>
.\run-memory-digest.ps1 -DryRun -WindowHours 72 -Verbose2

# 3. 問題なければ本番実行
.\run-memory-digest.ps1

# 4. 毎晩 03:10 に登録
.\register-task.ps1
```

### 日常運用

```powershell
Start-ScheduledTask  -TaskName QwenMemoryDigest   # 手動で今すぐ動かす
Get-ScheduledTaskInfo -TaskName QwenMemoryDigest  # 前回の実行結果
Get-Content .\logs\digest_2026-09-07.log -Tail 40 # ログ
.\register-task.ps1 -Unregister                   # 登録解除
```

### 直接叩く場合

```powershell
python .\memory_digest.py --dry-run --window-hours 168 -v
python .\memory_digest.py --consolidate            # 今回かならず統合まで行う
python .\memory_digest.py --no-consolidate         # 追記だけ
python .\memory_digest.py --include-archived       # アーカイブ済みチャットも対象
```

## 主なオプション

| オプション | 既定 | 意味 |
|---|---|---|
| `--window-hours` | 24 | 何時間前まで遡るか（state がそれより新しければ state を優先） |
| `--consolidate-days` | 7 | 統合の間隔 |
| `--keep-backups` | 14 | 残すバックアップ世代数（0 で無制限） |
| `--max-tokens` | 1200 | 抽出1回あたりの出力上限 |
| `--timeout` | 600 | 1リクエストのタイムアウト秒 |
| `--think` | off | 既定では `chat_template_kwargs.enable_thinking=false` を送って高速化している |
| `--dry-run` | off | 書き込まず結果を表示 |
| `--force` | off | 縮小ガードを無視 |

環境変数 `LLAMA_API_KEY` / `LLAMA_API_BASE` / `QWEN_MEMORY_DIR` でも既定値を上書きできる。

## 制約・既知の限界

- **LocalMind 経由の会話は拾えない。** Open WebUI は `chat_id` が空のリクエストを永続化しない
  （README §12）。DB に無いものはバッチにも見えない。スマホの会話も残したいなら
  LocalMind ではなく PWA 版 Open WebUI を使うこと。
- 抽出は 27B のローカルモデルによるものなので、取りこぼしや粒度のばらつきがある。
  週1の統合結果は時々目視で確認したほうがよい。バックアップから戻せる。
- `memory/` 直下に `.backup/` と `.digest_state.json` が増えるが、
  `memory_file_injector.py` はファイル名を明示指定して読むため注入内容には影響しない。
- チャット1件あたり直近 40 メッセージ、1メッセージ 2000 文字までに切っている
  （長大な貼り付けログを丸ごと投げないため）。定数はスクリプト冒頭で変更可能。
