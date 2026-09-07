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

---

## 7. 「検索が不十分」の件(未解決 / 原因候補を特定)

前回の修正は**誤発火を止めただけ**で、検索結果の中身には一切触れていない。
上流ソースを読んだ範囲で、既定値のままだと確実にボトルネックになる箇所が 3 つある。
`backend/open_webui/config.py` の既定値:

| 設定 | 既定値 | 効き方 |
|---|---|---|
| `RAG_TOP_K` (`rag.top_k`) | **3** | 全ページを合わせて **チャンク 3 個だけ**がモデルに渡る |
| `CHUNK_SIZE` / `CHUNK_OVERLAP` | 1000 / 100 | 上と合わせて実質 **約 3,000 文字**しかコンテキストに入らない |
| `WEB_SEARCH_RESULT_COUNT` (`web.search.result_count`) | **3** | 検索エンジンから取る URL が 3 件 |
| `WEB_LOADER_ENGINE` (`web.loader.engine`) | `''` = `SafeWebBaseLoader` | aiohttp + BeautifulSoup。**JS 実行なし** |

「ページ本文が取れていない」ように見える主因はこの 2 系統のどちらか:

1. **本文はある程度取れているが、top_k=3 で削られてモデルに届いていない**
   → `RAG_TOP_K` を 8〜12 に上げる。または管理者パネル → ドキュメント →
   **「埋め込みと取得をバイパス (Bypass Embedding and Retrieval)」** を ON にすると、
   取得したページ本文を丸ごとコンテキストに入れる
   (`routers/retrieval.py:2911-2925`。`BYPASS_WEB_SEARCH_EMBEDDING_AND_RETRIEVAL`)。
   27B に長文を食わせる余裕があるならこちらのほうが素直。

2. **そもそもページのロードに失敗して黙って捨てられている**
   `get_web_loader()` は `continue_on_failure=True` で走り
   (`retrieval/web/utils.py:1015-1050`)、失敗した URL は `docs` に入らない。
   その後 `urls = [doc.metadata.get('source') for doc in docs ...]` で
   **ロードできた URL だけに絞り込まれる** (`routers/retrieval.py:2904-2909`)。
   403 を返すサイトや JS レンダリングのサイトは全滅する。
   → `docker logs open-webui` に出るロードエラーを確認。多いなら
   `WEB_LOADER_ENGINE=playwright` + `PLAYWRIGHT_WS_URL`(別コンテナ)を検討。

**切り分け方**: 「N件のソースを取得」の N と、`web.search.result_count` の値を比べる。
N が設定値より明らかに少ない → (2)。N は足りているのに回答が薄い → (1)。

---

## 8. 「Open WebUI が遅い / TTFT が長い」件(未解決 / 最有力の仮説)

**`TASK_MODEL` が未設定だと、補助タスクが全部 27B に飛ぶ。**

`utils/task.py:16-27` の `get_task_model_id()` は、`TASK_MODEL` が空なら
`task_model_id = default_model_id`、つまりチャットで選んでいるモデルをそのまま使う。
そして `config.py:2308-2315` の既定値は:

| タスク | 既定 |
|---|---|
| `ENABLE_TITLE_GENERATION` | **True** |
| `ENABLE_TAGS_GENERATION` | **True** |
| `ENABLE_FOLLOW_UP_GENERATION` | **True** |
| `ENABLE_SEARCH_QUERY_GENERATION` | **True** |
| `ENABLE_AUTOCOMPLETE_GENERATION` | False |

つまり検索 ON の 1 送信で 27B への呼び出しが最大 **5 回**発生する
(検索クエリ生成 → 本チャット → タイトル → タグ → フォローアップ)。

llama-server を既定の `-np 1`(並列スロット 1)で動かしている場合、これらは
**直列にキューイングされる**。前のターンのタイトル/タグ/フォローアップ生成が終わるまで
次のターンの本チャットが始まらないので、体感 TTFT がそのぶん伸びる。
「本チャット自体の prompt eval は速いのに待たされる」ならこれが原因。

### 検索 ON のときの実際の待ち行列

```
[1] 検索クエリ生成      : 27B 往復 (記憶入りの messages 込み)
[2] 検索 API            : SearXNG/Brave 等
[3] ページ取得          : SafeWebBaseLoader, 同時 10 (web.loader.concurrent_requests)
[4] チャンク + 埋め込み  : all-MiniLM-L6-v2 を open-webui コンテナ内の CPU で実行
[5] 取得 (top_k=3)
[6] 本チャット          : 27B 往復  ← ここでやっと最初のトークン
```

`RAG_EMBEDDING_ENGINE` の既定値は `''`(= SentenceTransformers をコンテナ内で実行)、
モデルは `sentence-transformers/all-MiniLM-L6-v2` (`config.py:996-1002`)。
GPU ではなく **open-webui コンテナの CPU** で回るので、[4] も無視できない。

### 効く順に手を打つ

1. **検索クエリ生成を OFF**(§4-a)。[1] が丸ごと消える。副次的に記憶混入も直る。
2. **タイトル/タグ/フォローアップ生成を OFF**、または `TASK_MODEL` に小さいモデル
   (別ポートの llama-server に 1.5B〜3B) を指定する。キュー詰まりが消える。
3. **llama-server の並列スロットを増やす** (`-np 2` 以上)。VRAM と相談。
4. **埋め込みをバイパス**(§7-1)。[4][5] が消える。ただし [6] の prompt tokens は増える。

### 測ってほしいもの

`prompt eval time` の行を、1 送信につき**出てくる全部**拾ってほしい。
1 送信で何行出るかがそのまま「27B を何回叩いているか」になる。

| ケース | 期待される prompt eval の行数 |
|---|---|
| 検索 OFF・既存チャット | 3〜4 (本チャット + タグ + フォローアップ) |
| 検索 OFF・新規チャット | 4〜5 (+ タイトル) |
| 検索 ON | 上記 + 1 (クエリ生成) |

§4-a と上記 2 を適用後は **本チャット 1 行だけ**になるはず。
ここまで測れれば、残った遅さが llama-server 側(モデル/量子化/オフロード)なのか
Open WebUI 側なのか、はっきり切り分けられる。

---

## 9. 「llama-server を直接叩くと速い」との整合性

辻褄は合う。ただしそれだけでは §8 の 3 系統のどれが効いているかは決まらないので、
`prompt eval time` の **tokens 数**で切り分ける。

Open WebUI を挟むと増えるものは 3 系統ある。

| 系統 | 内容 | ログ上の見え方 |
|---|---|---|
| **A. 往復回数** | title / tags / follow_up / query 生成が同じ 27B に飛ぶ (§8) | `prompt eval` の**行数**が増える |
| **B. プロンプトが太る** | 記憶注入 + 検索で取得したページ本文 | 1 行あたりの **tokens が大きい** |
| **C. プロンプトキャッシュが毎回壊れる** | 下記 | 毎ターン tokens ≒ **会話全体**になる |

### C が本命の可能性が高い理由

llama.cpp の server は、スロットに前回のプロンプトの KV キャッシュを持っていて、
新しいリクエストとの **共通接頭辞 (LCP)** だけを再利用する。
`prompt eval time = ... / N tokens` の N は「**実際に評価したトークン数**」= キャッシュに
乗らなかった分だけ。だから:

- キャッシュが効いている → N は新しいユーザー発言ぶんだけの小さい数
- キャッシュが壊れている → N ≒ 会話全体のトークン数(毎ターン全再評価)

Open WebUI 経由だと、この接頭辞が壊れる要因が 3 つある。

1. **`{{CURRENT_TIME}}` / `{{CURRENT_DATETIME}}` をシステムプロンプトに使っている**
   `utils/payload.py:17-41` の `resolve_system_prompt()` → `utils/task.py:83-94` の
   `prompt_template()` が

   ```python
   formatted_time = current_date.strftime('%I:%M:%S %p')   # ← 秒まで入る
   template = template.replace('{{CURRENT_TIME}}', formatted_time)
   template = template.replace('{{CURRENT_DATETIME}}', f'{formatted_date} {formatted_time}')
   ```

   と展開する。**秒が入るのでプロンプトの先頭が毎回変わり、共通接頭辞が事実上ゼロになる。**
   使うなら日付だけ変わる `{{CURRENT_DATE}}` に落とす。

2. **記憶注入の内容が毎ターン変わる、または先頭に入る**
   `memory_file_filter` が毎回同じ文字列を同じ位置に入れているなら接頭辞として再利用されるが、
   タイムスタンプや件数、順序が揺れると 1 と同じことが起きる。

3. **補助タスクが同じスロットの KV キャッシュを上書きする**
   llama-server が `-np 1` だとスロットは 1 個。タイトル生成のプロンプトはチャットとは
   まったく別物なので、これが走った瞬間にスロットのキャッシュは会話のものではなくなる。
   → **次のターンのチャットは会話全体を再評価**することになる。
   A(往復が増える)と C(キャッシュが壊れる)が掛け算で効く。

「短いメッセージを送っただけなのに、直叩きより桁違いに遅い」なら C。
「1 回ぶんは速いが待たされる」なら A。「1 回が重い」なら B。

### 公平な比較のしかた

直叩きが `{"content": "ping"}` のような短文だと、単に入力が短いから速いだけになる。
比較するなら **同じ会話履歴を llama-server に直接投げて** `prompt eval` を見比べること。

### 見るべきログ

検索 OFF で、同じチャットに 2 回続けて短い発言を投げる。2 回目の
`prompt eval time = ... / N tokens` の N が

- **小さい (数十)** → キャッシュは効いている。C は無罪、A か B を追う
- **会話全体ぶん** → C 確定。上の 1〜3 を順に潰す

### C への対処

- システムプロンプトから `{{CURRENT_TIME}}` / `{{CURRENT_DATETIME}}` を外す
- 補助タスクを OFF にするか `TASK_MODEL` を別インスタンスへ逃がす (§8)
- llama-server を `-np 2` 以上にしてスロットを分ける
  (llama.cpp はスロット選択に LCP 類似度を使うので、会話用と補助タスク用に自然と分かれる)
- `--cache-reuse N` を付けて、接頭辞の途中が欠けても再利用できるようにする

---

## 10. C(プロンプトキャッシュ破壊)確定後の手順

C の要因は §9 の 1〜3 のどれか、または複数同時。**二分探索で1回ずつ潰す。**
毎回「同じチャットに短い発言を2回連投して、2回目の `prompt eval` の N を見る」で判定する。

### 手順 1: 補助タスクを全部 OFF にして測る

管理者パネル → 設定 → インターフェースで以下を全部 OFF:

- タイトル生成 (`task.title.enable`)
- タグ生成 (`task.tags.enable`)
- フォローアップ生成 (`task.follow_up.enable`)
- 検索クエリ生成 (`task.query.search.enable`)

→ **N が小さくなった → 要因は §9-3(スロットのKVキャッシュ上書き)で確定。**
   手順 3 へ。
→ **N が変わらない → 要因は §9-1 か §9-2。** 手順 2 へ。

### 手順 2: プロンプトの先頭が毎回変わっていないか

**2-a. システムプロンプトを確認**

管理者パネル → モデル → `qwen3.8-27b` のシステムプロンプト、および
設定 → 一般 のデフォルトシステムプロンプトに
`{{CURRENT_TIME}}` / `{{CURRENT_DATETIME}}` が入っていないか見る。

入っていたら消す。日付が要るなら `{{CURRENT_DATE}}` に落とす
(こちらは `%Y-%m-%d` なので 1 日 1 回しか変わらない)。
`{{CURRENT_TIME}}` は `%I:%M:%S %p` で **秒まで**入る (`utils/task.py:85-93`)。

**2-b. 記憶 Filter を一時的に無効化して測る**

モデル設定から `memory_file_filter` のチェックを外して 2 回連投。

→ N が小さくなった → **記憶注入が先頭で毎回変わっている**。下の「記憶注入の直し方」へ。
→ 変わらない → 2-a のシステムプロンプト側。

### 手順 3: 恒久対処

#### (a) 補助タスクを別モデルに逃がす ※ハマりどころあり

補助タスクを全部 OFF のままでもいいが、タイトルくらいは欲しいなら
`TASK_MODEL` に小さいモデル(別ポートの llama-server で 1.5B〜3B)を割り当てる。

**注意: 設定欄が 2 つあり、llama-server の場合は「外部」側でないと効かない。**

`utils/task.py:16-27`:

```python
def get_task_model_id(default_model_id, task_model, task_model_external, models):
    task_model_id = default_model_id
    if models.get(task_model_id, {}).get('connection_type') == 'local':
        if task_model and task_model in models:
            task_model_id = task_model            # ← TASK_MODEL
    else:
        if task_model_external and task_model_external in models:
            task_model_id = task_model_external   # ← TASK_MODEL_EXTERNAL
    return task_model_id
```

`connection_type` の既定値は接続の種類で決まる (`routers/openai.py:734`, `routers/ollama.py:424`):

| 接続 | 既定の `connection_type` | 効く設定 |
|---|---|---|
| OpenAI 互換 (= llama-server) | `external` | **`TASK_MODEL_EXTERNAL`**(外部モデル用タスクモデル) |
| Ollama | `local` | `TASK_MODEL`(ローカルモデル用タスクモデル) |

llama-server を OpenAI 互換接続として登録しているなら、
**`TASK_MODEL` 側にいくら入れても無視される。**「外部モデル用タスクモデル」に入れること。
(接続設定で connection_type を Local に切り替えている場合は逆になるので、
 効かなかったらもう一方を試す)

#### (b) llama-server 側

- `-np 2` 以上にしてスロットを分ける。llama.cpp はスロット選択に LCP 類似度を使うので、
  会話用と補助タスク用が自然と別スロットに落ち着く。
  **ただし `-np` はコンテキストを分割する**(スロットあたり `n_ctx / np`)ので、
  `--ctx-size` も併せて増やすこと。VRAM と相談。
- `--cache-reuse N` を付ける。接頭辞の途中が変わっても、後続チャンクを
  コンテキストシフトで再利用できるようになる。§9-1/§9-2 の緩和に効く。

#### (c) 記憶注入の直し方 — 「変わるものは後ろへ」

`process_chat_payload()` は **DB から会話履歴をロードした後に** inlet を呼ぶ
(`utils/middleware.py` の `load_messages_from_db` → `filter_type='inlet'` の順)。
inlet の注入は DB に保存されないので、毎ターン

```
[system] + [DB から復元した履歴] + [記憶注入]
```

が組み立て直される。ここで **記憶を先頭(index 0 の system メッセージ)に入れると、
内容が少しでも揺れた瞬間に共通接頭辞がゼロになる。**

対策は 2 つ。どちらかでよい。

1. **注入位置を末尾に move する** — `add_or_update_system_message()` のような
   index 0 への挿入をやめ、`body["messages"][-1]` (最後のユーザー発言) の content に
   前置き/後置きする。`[system] + [履歴]` の接頭辞が固定されるのでキャッシュが効く。
2. **注入内容を完全に決定的にする** — 記憶ファイルの読み込み順を固定し、
   タイムスタンプ・件数・「現在時刻」などの揺れる要素を一切入れない。
   毎回バイト単位で同一なら index 0 のままでも接頭辞として再利用される。

1 のほうが確実。2 は「記憶ファイルを編集した直後の 1 回だけキャッシュミス」で済むので、
記憶を頻繁に書き換えないなら 2 でも実用上問題ない。

---

## 12. 最終結果(実測)

検索ONの1ターン(「川口市の天気」)にかかる時間:

| 段階 | 時間 |
|---|---|
| 当初 | **85.7秒** |
| 設定・パラメータ調整後 | 46.7秒 |
| 記憶スキップ + 簡潔指示 | **23.0秒** |

### 効いた対策(効果順)

| # | 対策 | 内容 |
|---|---|---|
| 1 | **`-UBatch 128` → `512`** | prefill 270 → **385 tok/s**。既定値(512)より小さくしていたのが原因。1024 は 339 tok/s と逆に遅くなるので 512 が最適 |
| 2 | **「埋め込みと検索をバイパス」を OFF** | 全文注入をやめ top_k=3 のチャンク取得に。プロンプト **18,553 → 4,648トークン**。CPU埋め込みは16〜40チャンクで **1〜2.5秒**しかかからず、懸念は杞憂だった |
| 3 | **記憶注入を検索ターンでスキップ** | 記憶ブロック4,112文字(約2,000トークン)がクエリ生成と本チャストの両方に乗っていた。約11秒削減 |
| 4 | **システムプロンプトに簡潔指示** | 「箇条書きは合計5項目以内、前置きと締めの一文は書かない」。生成 440 → 250トークン程度、約8秒削減 |
| 5 | **DDGS Backend を duckduckgo に固定** | 「自動(ランダム)」だと Brave が 429、Yandex が低品質、Google が `hl=en-US&cr=countryUS` で英語圏検索、grokipedia が5秒タイムアウト。固定で解決 |

### 効かなかった / 逆効果だった案

- **`-UBatch 1024 / 2048`** — 512 より 12% 遅い
- **検索結果数・Fetch Content Limit を絞る** — バイパスOFF後はモデルに渡る量が `クエリ数 × top_k` で決まるため、ページ数や文字数を絞ってもプロンプトは縮まない。むしろ外れURLを引いたときのリカバリが効かなくなる。**広く集めて top_k で絞る**のが正しい
- **埋め込みが遅いという仮説** — 実測 1〜2.5秒。犯人ではなかった
- **補助タスクが主犯という仮説** — 検索ターンでは合計7秒(全体の8%)。検索なしターンでは相対的に効くが、そちらは元々3秒で終わっている

### 残っている改善余地

- **タイトル/タグ生成を OFF、または `TASK_MODEL_EXTERNAL` に小さいモデルを割り当てる**(約3秒)。
  `utils/task.py` の `get_task_model_id()` は `connection_type` で参照先が分岐する。
  OpenAI互換接続(llama-server)は `external` なので、**`TASK_MODEL` ではなく `TASK_MODEL_EXTERNAL`** でないと効かない
- **mmproj を外す** — 起動ログの `cache_reuse is not supported by multimodal, it will be disabled` のとおり、
  画像モデルを読むと `--cache-reuse` が無効化される。画像入力を使わないなら外すと
  プロンプトキャッシュの部分再利用が効き、VRAM も 0.86GB 空く
- **`-TensorSplit` の見直し** — 1080Ti(Pascal, テンソルコア無し)が prefill を律速している可能性。
  `-c` を下げて KV を減らし V100 側に寄せられれば、もう一段速くなる余地がある

### 副次的に判明した設定

- 「デフォルト機能 → ウェブ検索」が ON だと**新規チャットが常に検索ONで始まる**。
  記憶注入のスキップ条件と組み合わさると記憶が事実上無効になるので、
  記憶を効かせたいなら OFF にするか `web_search_files` に `profile.md` を指定する
- 検索クエリ生成プロンプトテンプレートは**空欄が「既定を使う」の意味**。
  ここに追記のつもりで文章を入れると**テンプレート全体が置き換わり**、
  `{{MESSAGES:END:6}}` と JSON 出力指示が消えてクエリ生成が壊れる

---

## 13. 採用した最終構成（§12 から一部変更）

§12 で「記憶注入を検索ターンでスキップ」して 23.0 秒を出したが、**運用方針として記憶は検索の
有無によらず常時 ON にすることを選んだ**ため、そのぶんは戻している。以下が実際に採用した構成。

### 制約（前提）

- **画像入力は必須**。したがって mmproj は外さない。
  → `cache_reuse is not supported by multimodal, it will be disabled` は受け入れる。
  記憶ブロックはプロンプトキャッシュに乗らない前提でコストを考える
- **記憶は常時 ON**。検索ターンでも名前・職業などを答えられる必要がある
- VRAM は V100 14.5/16GB、1080Ti 9.9/11.2GB でほぼ埋まっている。
  → 補助タスク用に 2 つめの llama-server を立てる案（`TASK_MODEL_EXTERNAL`）は**非現実的として撤回**

### 設定一覧

| 対象 | 項目 | 値 |
|---|---|---|
| Open WebUI 関数 | `force_web_search` | v2.0.0（`__metadata__` で判定） |
| Open WebUI 関数 | `Memory File Injector` | v0.2.0、`web_search_files` = `profile.md,projects.md,inbox.md`（= 常時注入） |
| ウェブ検索 | エンジン | DDGS |
| ウェブ検索 | DDGS Backend | **duckduckgo 固定**（自動＝ランダムは不可） |
| ウェブ検索 | 検索結果数 | 2 |
| ウェブ検索 | Fetch URL Content Length Limit | 2500 |
| ウェブ検索 | 埋め込みと検索をバイパス | **OFF** |
| インターフェース | 検索クエリ生成プロンプト | **空欄**（既定を使う） |
| インターフェース | タイトル生成 | **OFF** |
| モデル | システムプロンプト | 検索結果優先 + 「箇条書きは合計5項目以内、前置きと締めの一文は書かない」 |
| モデル | デフォルト機能 → ウェブ検索 | ON |
| llama-server | `-UBatch` | **512** |
| llama-server | `-TensorSplit` | 5,2 |
| llama-server | mmproj | 読み込む（画像入力のため） |

### 速度

| 段階 | 時間 |
|---|---|
| 当初 | 85.7 秒 |
| 設定・パラメータ調整後 | 46.7 秒 |
| （参考）記憶スキップ時の実測 | 23.0 秒 |
| **採用構成**（記憶常時 ON + タイトル生成 OFF） | **31 秒前後（見込み）** |

最後の行は未実測の見込み値。記憶を戻すと +11 秒、タイトル生成 OFF で −2.7 秒という内訳。

### 残っている改善余地

1. **`inbox.md` の肥大化** — 追記専用の設計なので放置すると増え続け、毎ターン 2 回（クエリ生成 +
   本チャット）prefill に乗る。定期的に `profile.md` / `projects.md` へ統合して `inbox.md` を
   空にする運用が必要。現状 4,112 文字（約 2,000 トークン）で prefill 約 11 秒ぶん
2. **記憶ブロックの縮小** — profile.md 4KB / projects.md 3KB を毎ターン 27B に読ませる必要が
   あるかの見直し。半分にできれば約 5 秒
3. **`-c 49152` を下げて `-TensorSplit` を V100 寄りに** — prefill を律速しているのは
   1080Ti（Pascal、テンソルコア無し）の可能性が高い。コンテキストを 16384 程度に下げれば
   KV が数 GB 空き、`6,1` に寄せられる。起動ログのデバイス別 KV buffer size から下げ幅を判断する

---

## 14. 「検索が不十分」の真因は埋め込みモデルだった（§7 の訂正）

§7 で挙げた「ページのロード失敗」「top_k が小さい」はどちらも主因ではなかった。
真因は **埋め込みモデルが英語専用だったこと**。

```
'embedding_config': "{'engine': '', 'model': 'sentence-transformers/all-MiniLM-L6-v2'}"
```

`sentence-transformers/all-MiniLM-L6-v2`（Open WebUI の既定値）は英語のみで学習されている。
日本語のクエリと日本語の文書を投げても意味のある類似度が出ず、**top_k の選択がほぼランダム**
になっていた。ページ自体は正しく取得できているのに、その中から関連チャンクを選ぶ段階で
壊れていたということ。

症状としては、天気を聞いているのに「サイトのメニュー項目一覧」「全国平均気温の統計」
「過去の天気アーカイブ」といったチャンクが上位に来る。うまくいった回は、当たりのチャンクが
たまたま上位に来ただけだった。

### 対処

管理者パネル → 設定 → ドキュメント

| 項目 | 変更前 | 変更後 |
|---|---|---|
| 埋め込みモデル | `sentence-transformers/all-MiniLM-L6-v2` | **`intfloat/multilingual-e5-small`** |
| 埋め込みモデルバッチサイズ | 1 | **16** |
| トップ K | 3 | **6** |

モデル名を書き換えたあと、**右のダウンロードアイコンを押して取得**する必要がある（約470MB）。
ウェブ検索のコレクションは毎回作り直しなので再インデックスは不要。

### 効果（「明日の立川市の天気は」/ 「明日の三鷹の天気は」）

変更前は「検索結果には具体的な天気データが含まれていません」と答えられなかったものが、
変更後は tenki.jp / weathernews.jp / AccuWeather の**当該市の予報ページ本文**が上位に来て、
時間帯別の降水確率・気温・警報まで答えられるようになった。

`multilingual-e5-small` は MiniLM より大きい（118M vs 22M）ので埋め込みは少し遅くなるが、
38チャンクでも数秒。精度差を考えれば十分見合う。

### 併せて訂正: `Fetch URL Content Length Limit` はウェブ検索に効かない

この設定（`web.fetch.max_content_length`）を参照しているのは `routers/retrieval.py:396` の
設定マッピングだけで、`process_web_search` → `get_web_loader()` の経路には渡っていない。
ネイティブ関数呼び出しの `fetch_url` ツール専用。

実際、うまくいった回に取れていたチャンクは `start_index: 5410` で、2500文字の上限が
効いていたら到達できない位置にある。

つまり §12 に書いた「18,553 → 4,648 トークン」の削減は**すべて「埋め込みと検索をバイパス」を
OFF にした効果**であり、文字数上限は無関係だった。この値は検索の挙動を変えない。

### 未検証: ネイティブ関数呼び出しによる自動判定

`utils/middleware.py:2669-2678` より、`function_calling` が `legacy` のときだけ強制 RAG 経路が
走る。`native` にすると代わりに `search_web` / `fetch_url` がツールとしてモデルに渡され
（`utils/tools.py:677-685`）、モデルが必要と判断したときだけ呼ぶ。

- llama-server 側は `--jinja` が既に入っているので準備済み
- 必要なのは Open WebUI 側の 2 設定（Function Calling を Native、機能の「組み込みツール」を ON）
- ただし検索するターンは「ツール呼び出し → 結果注入 → 再生成」で生成が 2 回走るため、
  速くなるとは限らない

現状の legacy でも、クエリ生成が `{"queries": []}` を返せば検索はスキップされるので、
実質「必要なときだけ検索」にはなっている。判定 1 往復ぶんのコストを払っているだけ。
**今回は legacy のままとする判断をした。**

---

## 15. llama-server 起動スクリプトと sparkDash（付随して直したもの）

Open WebUI 本体とは別系統だが、同じ母艦の同じ調査中に出たので併せて記録する。

### 15-1. `30-run-server.ps1` の既定値

セッション中は `-UBatch 512 -TensorSplit 5,2` を**毎回コマンドラインで渡していた**だけで、
スクリプトの既定値は変えていなかった。引数なしで起動すると元に戻る状態だったので、
`param()` の既定値を実測値に合わせた。

| 項目 | 変更前 | 変更後 |
|---|---|---|
| `$UBatch` | 256 | **512** |
| `$TensorSplit` | `@("2","1")` | **`@("5","2")`** |

`-ub` はスクリプトのコメントどおり「語彙 248k のロジットバッファがこれに比例」する。
512 で約 508MB、1024 で約 1GB。VRAM に余裕がないため 1024 は 385→339 tok/s と逆に遅くなる。
**512 が最適**。

#### ハマったこと: `param()` 内のコメントで次の行を飲む

```powershell
    [int]$UBatch = 512,          # 物理バッチ。…
                                 # 実測 512 が最速。…    [switch]$Tailscale,
```

`#` から行末までがコメントなので、**`[switch]$Tailscale,` がパラメータ定義から消える**。
`-Tailscale` を付けて起動すると「パラメータ名 'Tailscale' に一致するパラメータが
見つかりません」になる。コメントを複数行にするときは、次のパラメータを必ず独立行に置く。

### 15-2. sparkDash が `Invalid API Key` を出し続けた件

llama-server のコンソールが 2 秒ごとに `unauthorized: Invalid API Key` で埋まり、
`print_timing` が読めなくなっていた。

**原因: 同じ llama-server (:8080) を 2 つの Spark から監視していた。**

`config/sparks-secrets.json`:

```json
"llmApiKeys": {
  "ryzen-v100":   "…",
  "ryzen-1080ti": "…"
}
```

API キーは `llmApiKeys[sparkId][port]` と **Spark ごと × ポートごと**に保管される
(`server/collectors/LlmProbe.js` の `_apiKey()`)。V100 側だけ直しても 1080Ti 側が
古いキーで叩き続けるため、直したつもりが再発しているように見えていた。
2 秒ごとに複数本エラーが出ていたのは、プローブが 2 本走っていたから。

#### 正しい対処

**Edit Spark → 「LLM monitoring」のチェックを外す。**

`server/sparks/SparkMonitor.js:146` が
`const ports = this._llmMonitoringEnabled() ? this._llmPorts() : []`
なので、これだけでその Spark の LLM プローブが完全に止まる。GPU 監視は影響を受けない。

LLM カードの `× Remove` は**使わないこと**。この操作で Spark ごと消えて、
1080Ti のタブと GPU 監視まで失った（`+` から GPU index 1 で作り直して復旧）。

llama-server は tensor-split で 2 枚にまたがる 1 プロセスなので、LLM サービスは
「V100 のもの」でも「1080Ti のもの」でもなく**ホストのもの**。登録は 1 つが正しい。

#### 切り分けに使えた知識

**表示ラベルで状態が分かる**（`LlmProbe.js:1336-1341`）:

| 表示 | 意味 |
|---|---|
| `Bad API key` | キーが**保存されていて**、401/403 で拒否された |
| `Auth required` | キーが保存されていない（または復号に失敗して消えた） |

「Bad API key」が出ている時点で、保存経路や暗号化ストアの故障ではなく
**値の不一致**だと確定できる。

**このビルドの llama.cpp の認証免除エンドポイントは `/health` だけ**:

```powershell
$key = '<バナーの値>'
foreach ($p in '/health','/v1/models','/props','/slots','/metrics') {
  try {
    $r = Invoke-WebRequest "http://100.87.81.4:8080$p" -Headers @{Authorization="Bearer $key"} -UseBasicParsing
    "{0,-12} -> {1}" -f $p, $r.StatusCode
  } catch { "{0,-12} -> {1}" -f $p, $_.Exception.Response.StatusCode.value__ }
}
```

正しいキーでの結果: `/health` `/v1/models` `/props` `/slots` が 200、
`/metrics` は 501（`--metrics` 未指定なだけで認証エラーではない）。
キー無しだと `/health` 以外すべて 401。**サーバ側とクライアント側のどちらが悪いかを
一発で切り分けられる**ので、この 5 行は残しておく価値がある。

#### 秘密の保管場所（参考）

- `config/sparks-secrets.json` — AES-256-GCM の暗号文
- `config/.secrets-key` — 暗号鍵（`SPARKDASH_SECRETS_KEY` 環境変数があればそちらが優先）
- 復号に失敗した場合はキーが読み込まれず「Auth required」表示になるため、
  「Bad API key」が出ているなら鍵ファイルや config ボリュームの問題ではない

---

## 16. 記憶の自動蓄積（夜間バッチ / `memory_digest/`）

### きっかけ

「Open WebUI のセッションの中で蓄積された情報を自動で記録やナレッジ登録できないか」。
現行の `memory_file_injector.py` は**「記憶して」と明示的に言ったときだけ**抽出推論を
走らせる方式（§4）なので、言い忘れた事実は残らない。

### 検討した3案と選択

| 案 | 内容 | 判定 |
|---|---|---|
| A | Filter の `outlet` で毎ターン抽出 | ✗ 応答のたびに抽出推論が走り会話が遅くなる。さらに LocalMind 経由では `outlet` が呼ばれない（§12）ので取りこぼす |
| B | 明示トリガ（現行） | △ 確実だが言い忘れると残らない。**廃止せず併用する** |
| **C** | **夜間バッチで Open WebUI の DB から抽出** | **採用** |

C を選んだ理由:

1. **会話速度に一切影響しない。** 推論は寝ている間にまとめて走る
2. **ブラウザ由来の会話をすべて拾える。** 言い忘れが無くなる
3. **`inbox.md` 肥大化問題（§13 の残課題）を同時に潰せる。** 週1で `profile.md` /
   `projects.md` へ統合し `inbox.md` を空にする運用が組み込める

### 調べた事実（推測で決め打ちしていない）

`docker exec` / `docker inspect` で実際に確認した結果:

| 項目 | 実測値 |
|---|---|
| DB | `/app/backend/data/webui.db` — SQLite、WAL モード（`webui.db-wal` 3.3 MB, `-shm` 32 KB が存在） |
| DB のマウント形態 | **named volume** `open-webui` → `/var/lib/docker/volumes/open-webui/_data`。**ホスト（Windows）から直接は触れない** |
| memory のマウント形態 | **bind mount** `C:\Users\tomoy\Git\qwen38-1080ti\memory` → `/app/memory`。**ホストから直接書ける** |
| `chat` テーブルの列 | `id, user_id, title, created_at, updated_at, share_id, archived, chat, pinned, meta, folder_id, tasks, summary, last_read_at, current_message_id, variables, timer_at` |
| チャット件数 | 57 |

この2つのマウント形態の違いがそのまま実装方針を決めた
— **DB は `docker exec` 経由で読み、`memory/` はホストから直接書く**。

### 実装

`openwebui/memory_digest/` に3ファイル:

| ファイル | 役割 |
|---|---|
| `memory_digest.py` | 本体。標準ライブラリのみ（母艦に pip 不要） |
| `run-memory-digest.ps1` | 母艦用ラッパー。Docker 生存確認 + UTF-8 ログ出力 |
| `register-task.ps1` | Windows タスクスケジューラへの登録／解除 |

処理の流れ:

```
1. llama-server /v1/models で生存確認 → 落ちていれば何もせず終了（深夜にエラーを積まない）
2. コンテナ内で webui.db を mode=ro で開き、直近 N 時間に更新されたチャットを JSON 化
   → docker cp で回収（docker exec の stdout は Windows で日本語が壊れるため経由しない）
3. state ファイルの last_chat_updated_at より新しいものだけを対象にする
4. 既存の profile/projects/inbox を「既知の事実」としてプロンプトに同梱し、
   llama-server に「新しく分かった事実だけ」を箇条書きで出させる
5. memory/ をバックアップしてから inbox.md に日付見出しで追記
6. 7日ごとに inbox.md を profile.md / projects.md へ統合（JSON で全文を返させる）
```

### 事故を起こさないための設計

記憶ファイルの**上書き**は事故が怖いので、多重に止める:

- 書き込み前に必ず `memory/.backup/YYYY-MM-DD_HHMMSS/` へ全 `.md` を退避（14世代保持）
- 統合結果が既存の**半分未満に縮んだら中止**（`--force` で解除）
- モデル出力が JSON として解釈できなければ**何も書かずに中止**
- 毎晩の処理は `inbox.md` への**追記のみ**。既存ファイルには触らない
- DB は `mode=ro`。発行するのは `SELECT` だけで Open WebUI は止めない
- `--dry-run` で書き込まずに内容だけ確認できる

### 検証

抽出スクリプトと本体ロジックはサンプル DB を作って通してある。確認した点:

- `updated_at` が秒／ミリ秒どちらでも正しく正規化される（版差対策）
- `chat["messages"]`（配列）と `chat["history"]["messages"]`（辞書）の両形式を読める
- マルチモーダルの `content` が配列の場合に text 部分だけ拾う
- `<think>` ブロックと `[1]` 形式の引用マーカーを落とす
- 縮小ガードが実際に上書きを止める（`profile.md` が 35→1 文字になるケースで中止を確認）

### 制約

- **LocalMind 経由の会話はバッチにも見えない。** `chat_id` が空だと Open WebUI は
  永続化しない（§12）。DB に無いものは抽出できない。スマホの会話も残したいなら
  PWA 版 Open WebUI を使うこと（§15 以降で導入済み）
- 抽出は 27B のローカルモデルなので取りこぼしや粒度のばらつきがある。
  週1の統合結果は時々目視で確認する。バックアップから戻せる

詳細な使い方は `openwebui/memory_digest/README.md`。

### 追記: PowerShell 5.1 と BOM

`.ps1` を **BOM なし UTF-8** で保存すると、PowerShell 5.1 は日本語 Windows 上で
それを Shift-JIS として読む。結果、スクリプト内の日本語リテラルが化ける
（実例: `終了コード: 0` が `邸ゆコ・さ 綢シ綢・ 0` になった）。

**`.ps1` は UTF-8 BOM 付きで保存すること。** これは §5 に書いた
「PowerShell 5.1 でファイルを書き換えるときは `[IO.File]` の UTF8 読み書きを使う」
と同じ話の裏返しで、*書く側* にも BOM が要るという点が抜けていた。
ログファイルも `[Text.UTF8Encoding]::new($true)` で作り、
`Get-Content` がそのまま読めるようにしてある。

### 追記: chat カラムの本体は `history.messages` だった

`--probe` を足して実測した結果:

```
チャット 57 件 / 抽出できた本文 合計 807 文字 / 本文ゼロ 0 件
chat.messages          : list / 1 件      ← ユーザーの最初の発言だけ
chat.history.messages  : dict / 6 件      ← 本体はこちら
messages_of -> 1 件 / 28 文字
```

初版は `chat["messages"]`（配列）を優先し、空のときだけ `history` を見ていた。
ところがこの版の `chat["messages"]` には**先頭1件しか入っていない**ため、
57 件のチャットから 807 文字しか読めず、抽出が毎回「新しい事実なし」になっていた。

`history.messages` は id をキーにした**分岐ツリー**で、`currentId` が指す葉から
`parentId` を辿った経路が画面に表示されている枝にあたる。再生成で捨てられた枝を
混ぜないよう、この経路を復元して古い順に並べる実装に変えた。
`currentId` が無い版のために timestamp 順のフォールバックも持たせ、
最終的には history 側と `chat["messages"]` の**件数が多いほう**を採る。

ここでも「推測でキー名を決め打ちしない」が効いた。構造を表示させるまで、
`chat["messages"]` が空でないのに中身が1件だけ、という形は想像できなかった。

### 実測結果（2026-09-07 初回導入）

| 段階 | 実測 |
|---|---|
| 対象 | チャット 57 件（過去30日）、会話本文 35,112 文字 |
| 抽出 | 3 バッチ（18,852 / 24,141 / 9,512 文字）、所要 **80 秒** |
| 抽出結果 | 新しい事実 **19 件** |
| 統合 | `profile.md` 2,200 字 / `projects.md` 1,844 字、所要 **63 秒** |

抽出できた事実の例（設計どおりカテゴリ分けされている）:

```
- [profile] 更新: 所在地は埼玉県（宮城県仙台市出身）（旧: 記載なし）
- [profile] 所属: NONSAPO-GmbH（ノンサポ電子）
- [project] FULLMONI-Legacy: サードパーティECUのCAN通信に対応したオープン
           ハードウェアのデジタルメーターモジュール（3.5インチTFT液晶、
           CPU: ルネサス H8SX1655、CAN 2.0B、RS232C、タッチパネル対応）
- [project] NAロードスター: モアパワー化（NA8C + BP-VET）のため MoTeC を使用
- [project] llama-server の tensor-split は 5,2（旧: 2,1）
- [project] prefill 速度は 364〜385 tok/s、decode 速度は 26.5〜33.0 tok/s
```

設計どおりに動いた点:

- **矛盾する事実は `更新: …（旧: …）` の形で出る**（tensor-split 2,1 → 5,2）
- **一時的な話題（天気の問い合わせなど）は抽出されない**
- 統合後も既存の記憶が全部残った。応答ルール（J.A.R.V.I.S.）、
  「Continue の Built-in Tools と Fusion ツールは同時に有効にしない」、
  CUDA OOM の回避策、`sparkDash_new` の作業フォルダ、Voron 2.4、Scan2Solid、
  PPA-CF ブラケット、いずれも消えていない

統合で見つかった不具合（修正済み）:

- 初版のプロンプトはファイル間の整合を指示していなかったため、
  `projects.md` の tensor-split が 5,2 に更新されても
  `profile.md` の母艦セクションが 2,1 のまま取り残された。
  「同じ設定値が両方に現れる場合は新しいほうに揃える」を明示して解消。

### 運用

Windows タスクスケジューラに `QwenMemoryDigest` として毎日 03:10 で登録。
初回は 30 日分・57 件を一括処理したが、通常運用は窓 24 時間なのでこの負荷にはならない。

```powershell
Start-ScheduledTask  -TaskName QwenMemoryDigest    # 手動で今すぐ
Get-ScheduledTaskInfo -TaskName QwenMemoryDigest   # 前回の結果
Get-Content .\logs\digest_2026-09-07.log -Encoding UTF8 -Tail 40
```

### 稼働確認（2026-09-07）

タスクスケジューラ経由での実行を `Start-ScheduledTask` で確認済み。手で叩くのとは
プロセスコンテキストが違うため、`python` / `docker` / ユーザー環境変数 `LLAMA_API_KEY`
がタスクからも見えることをここで確かめている。

```
LastTaskResult : 0
NextRunTime    : 2026/09/08 3:10:00

[13:13:26] 開始: …python.exe … (window=24h)
[13:13:26] 対象: 2026-09-07 11:17 以降に更新されたチャット
[13:13:26] 新しいチャットは無し
[13:13:27] 終了コード: 0
```

ログに文字化けが無いことで、`.ps1` の BOM 付与が効いていることも同時に確認できる。
