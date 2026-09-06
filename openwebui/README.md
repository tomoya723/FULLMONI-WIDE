# Open WebUI: 検索トグル OFF でも Web 検索が走る問題

対象: 母艦 Docker の `open-webui` (ポート 3000)、推論バックエンド llama-server (100.87.81.4:8080)、
モデル `qwen3.8-27b` に適用している自作 Filter `force_web_search` / `memory_file_filter`。

---

## 1. 結論(根本原因)

`force_web_search` の inlet が使っている

```python
body.get("metadata", {})
```

が **常に空 dict** になっている。したがって「session_id が無い」という条件はブラウザからの
通常チャットでも必ず真になり、UI のトグル状態と無関係に `features.web_search = True` が
毎回注入されていた。

`metadata` が空になるのは Open WebUI の処理順序のため。`backend/open_webui/utils/middleware.py`
の `process_chat_payload()` は

1. パイプライン inlet → **Filter の inlet を実行**
2. …その後に `form_data["metadata"] = metadata` を代入

という順番になっている。inlet が呼ばれる時点では `form_data`(= `body`)に `metadata` キーが
そもそも存在しない。`session_id` は `main.py` の `chat_completion()` で
`form_data.pop("session_id", None)` により **body から抜かれて** 別 dict の `metadata` に入るので、
body 側を見ても永久に見つからない。

検証済みの上流ソース (すべて `backend/open_webui/`。行番号は上流 `main` ブランチ時点のもので、
導入バージョンによってはズレる。関数名で追ってほしい):

| 場所 | 内容 |
|---|---|
| `main.py:1243-1251` | `metadata = { ... "session_id": form_data.pop("session_id", None), ... }` — body から pop される |
| `utils/middleware.py:2635` | `process_filter_functions(..., filter_type="inlet", form_data=form_data, ...)` |
| `utils/middleware.py:2840` | `form_data["metadata"] = metadata` ← **inlet より後** |
| `utils/filter.py:135-144` | `get_filter_params()`。inlet の引数に `__metadata__` を宣言すれば本物の metadata が渡る |

v0.6.5 / v0.6.15 / v0.6.22 / v0.6.30 / v0.7.0 / v0.8.0 / main の全タグで
「inlet が先、`form_data["metadata"]` 代入が後」の順序は同じであることを確認済み。

### 正しい取得口

inlet のシグネチャに `__metadata__` を宣言する。`get_filter_params()` は
`sig.parameters` に含まれるキーだけを渡すので、宣言しない限り渡ってこない。

```python
def inlet(self, body: dict, __metadata__: Optional[dict] = None) -> dict:
    md = __metadata__ or {}
    session_id = md.get("session_id")   # ← ブラウザなら socket.io の session id が入る
```

---

## 2. 仮説の訂正: 補助タスクは inlet を通っていない

> 「タイトル生成・タグ生成・検索クエリ生成でも inlet を通り、session_id が無いのでヒットする」

これは **不成立**。`routers/tasks.py` の各タスク (`/api/v1/tasks/title/completions`,
`/queries/completions` など) は

```python
payload = await process_pipeline_inlet_filter(request, payload, user, models)   # ← Pipelines のみ
...
return await generate_chat_completion(request, form_data=payload, user=user)
```

という経路で、`process_chat_payload()` を通らない。つまり **Filter 関数の inlet は補助タスクでは
呼ばれない**(呼ばれるのは Pipelines のインレット)。1 送信あたり inlet は 1 回だけ。

ただし補助タスクの payload には確かに

```python
'metadata': {
    'task': str(TASKS.QUERY_GENERATION),   # 'query_generation' 等
    'task_body': form_data,
    'chat_id': ...,
}
```

が付く (`routers/tasks.py:188,253,318,377,454,530,585,640` / `constants.py:135-148`)。
将来 Open WebUI が経路を変えた場合の保険として、task ガードは入れておく価値がある。
今回の修正コードにも入れてある。

### 症状 2(検索クエリに記憶の語が混ざる)の本当の原因

`chat_web_search_handler()` (`utils/middleware.py:1500-1527`) が

```python
messages = form_data['messages']          # ← inlet 適用“後”の messages
res = await generate_queries(request, {'model': ..., 'messages': messages, ...}, user)
```

としているため、`memory_file_filter` が inlet で注入した記憶(system メッセージ)が
そのままクエリ生成プロンプトに載る。`query_generation_template()` →
`replace_messages_variable()` (`utils/task.py:197-237`) は role でのフィルタを一切していないので、
system メッセージも `{{MESSAGES:END:6}}` に含まれる。

つまり「補助タスクに記憶 Filter が乗っている」のではなく、
**記憶が乗った本チャットの messages がクエリ生成に横流しされている**。inlet に task ガードを
足しても症状 2 は消えない。対処は §4 を参照。

---

## 3. 修正した `force_web_search`

`force_web_search.py` を参照。判定は以下:

| リクエスト | `__metadata__` の特徴 | 挙動 |
|---|---|---|
| 内部補助タスク | `task` / `task_body` あり | 素通し |
| ブラウザ UI の通常チャット | `session_id` あり | 素通し(UI トグルに一任) |
| API 直叩き (LocalMind) | `session_id` も `task` も無し | `features.web_search = True` を注入 |

Valves:

- `enabled` — フィルタ全体の ON/OFF
- `force_override` — API 側が `features.web_search` を明示指定していても上書きするか (既定 False)
- `debug` — `__metadata__` の中身と判定結果を stdout に出す。`docker logs -f open-webui` で確認する用。
  確認用の print は削除せず **この debug Valve に集約**した。既定 OFF なので通常運用では黙る。

ローカルでロジック単体テスト済み(5 ケース、期待どおり):

```
ブラウザ トグルOFF   -> skip:ui_session        features={'web_search': False}
ブラウザ トグルON    -> skip:ui_session        features={'web_search': True}
補助タスク           -> skip:internal_task     features=None
API 直叩き           -> inject:web_search=True features={'web_search': True}
API + 明示 False     -> skip:explicit_features features={'web_search': False}
```

---

## 4. 記憶が検索クエリに混ざる件の対処

優先度順。**(a) だけで症状 2 は消える**。

### (a) 検索クエリ生成そのものを止める(推奨)

管理者パネル → 設定 → インターフェース → **「検索クエリ生成 (Search Query Generation)」を OFF**。

`routers/tasks.py:403-411` で `task.query.search.enable` が False だと `generate_queries` は
400 を返し、`chat_web_search_handler` の except 節が
`queries = [user_message]`(ユーザーが打った生テキスト)にフォールバックする
(`utils/middleware.py:1559-1561` の except 節)。記憶は一切混ざらない。

副次効果として **LLM 往復が 1 回減る**。qwen3 の 27B をローカル 1080Ti 系で回している以上、
クエリ生成の 1 往復は TTFT にそのまま乗るので、TTFT 短縮にも効く。

### (b) `memory_file_filter` に task ガードを入れる(保険)

`memory_filter_guard.py` の `_skip_injection()` を `memory_file_filter.py` に取り込み、
inlet の先頭 3 行に入れる。§2 のとおり現行バージョンでは補助タスクに inlet は来ないので
これ単体では症状 2 は消えないが、`body.get("metadata")` 誤用の修正と将来の保険になる。

> `memory_file_filter.py` の実体 (`C:\Users\tomoy\Git\qwen38-1080ti`) はこのセッションから
> 読めなかったため、当てるべき差分ではなく貼り付け用スニペットの形にしてある。
> 実ファイルを見せてもらえれば直接パッチを書く。

### (c) 将来オプション: `request` フックへ移す

Open WebUI の `main` ブランチには `filter_type='request'` フック
(`utils/middleware.py:3104`) がある。これは Web 検索・RAG が終わった **後**、モデル送信直前に
走るので、ここで記憶を注入すればクエリ生成には絶対に載らない。
ただし v0.6.5〜v0.8.0 のリリースタグには存在しない(main のみ)ので、
使うなら Open WebUI を上げてからにすること。

---

## 5. 反映手順(ファイルを直すだけでは効かない)

Filter の実体は **DB の `function` テーブルの `content` カラム**にある。
`utils/plugin.py:375-403` の `get_function_module_from_cache()` は inlet のたびに

```python
function = await Functions.get_function_by_id(function_id)
content = function.content          # ← DB から読む
```

としており、**ホスト側の .py ファイルは一切参照されない**。

反映手順:

1. 管理者パネル → 関数 (Functions) → `force_web_search` を開く
2. エディタの中身を `force_web_search.py` の内容で全置換して保存

コンテナ再起動は不要。`function_contents_cache` は content の一致で判定しているので、
保存した瞬間に次のリクエストからモジュールが再ロードされる (`utils/plugin.py:399-403`)。

### PowerShell 5.1 でファイルを書く場合

`Get-Content` / `Set-Content` は既定コードページで化けるので使わない:

```powershell
$path = 'C:\Users\tomoy\Git\qwen38-1080ti\force_web_search.py'
# 読む
$src = [IO.File]::ReadAllText($path, [Text.UTF8Encoding]::new($false))
# 書く (BOM なし UTF-8, LF)
[IO.File]::WriteAllText($path, $src, [Text.UTF8Encoding]::new($false))
# クリップボードへ (管理画面に貼る用)
[IO.File]::ReadAllText($path, [Text.UTF8Encoding]::new($false)) | Set-Clipboard
```

---

## 6. 実測手順(未実施 — 母艦にアクセスできないため)

このセッションはクラウド上のコンテナで動いており、母艦の Docker にも
llama-server (100.87.81.4:8080) にも到達できない。以下は手元で流してほしい手順。
結果を貼ってもらえれば TTFT の切り分けまで続ける。

### 6-1. inlet が何回呼ばれるか / metadata の中身

`force_web_search` の Valves で `debug = true` にしてから:

```powershell
docker logs -f --tail 0 open-webui
```

を流しつつ、ブラウザで 1 回送信する。期待される出力は **1 行だけ**:

```
[force_web_search] decision=skip:ui_session task=None session_id='<socket id>' chat_id='...' ...
```

- 2 行以上出る → 想定外。全行を貼ってほしい
- `session_id=None` で `md_keys` に `session_id` すら無い → `__metadata__` が渡っていない
  (Open WebUI が古い)。その場合は `md_keys` の実測値を教えてほしい

### 6-2. トグル OFF / ON の動作確認

| 操作 | 期待 |
|---|---|
| ブラウザで検索トグル **OFF** → 送信 | 「N件のソースを取得」が出ない / ログは `skip:ui_session` |
| ブラウザで検索トグル **ON** → 送信 | 検索が走り、ソースが取得できる |
| LocalMind から API 直叩き | ログは `inject:web_search=True`、検索が走る |

### 6-3. prompt eval time / 処理トークン数

llama-server のコンソール (100.87.81.4) に出る行を、検索 OFF / ON それぞれで拾う:

```
prompt eval time = XXXX.XX ms /  NNNN tokens ( XX.XX ms per token,  XX.XX tokens per second)
       eval time = ...
```

検索 ON のときは **1 送信につき複数回**出るはず(クエリ生成 → 本チャット、
タイトル/タグ生成も走ればさらに増える)。切り分けたいのは:

- 検索 OFF の本チャット 1 回分の prompt tokens
- 検索 ON の本チャット 1 回分の prompt tokens(取得ページ本文が乗るので激増するはず)
- クエリ生成タスク分の prompt tokens

§4 (a) でクエリ生成を切ると、この 3 番目が丸ごと消える。

llama-server 側に直接投げて素の値を測るなら:

```powershell
$key = 'Na6ce...'   # 大文字始まりの正しいキー。コンテナ環境変数に残っている小文字の値は古いので使わない
$body = @{ model='qwen3.8-27b'; messages=@(@{role='user'; content='ping'}); max_tokens=1 } | ConvertTo-Json -Depth 5
Measure-Command {
  Invoke-RestMethod -Uri 'http://100.87.81.4:8080/v1/chat/completions' -Method Post `
    -Headers @{ Authorization = "Bearer $key" } -ContentType 'application/json' -Body $body
}
```
