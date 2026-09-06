# ローカル LLM 環境 全仕様

最終更新: 2026-09-06

このドキュメントは、あなた（qwen3.8-27b）が動作している環境そのものの仕様書である。
自分がどのハードウェアの上で、どのような経路でリクエストを受け取り、
どんな前処理・後処理が施されているかを把握するために使うこと。

---

## 0. 一行で言うと

Windows 11 の自作機（Tesla V100 + GTX 1080 Ti の 2 枚差し）で llama.cpp の
llama-server を動かし、その前段に Docker の Open WebUI を置いた個人用のローカル LLM 環境。
Tailscale 経由でスマホからも使える。

---

## 1. ハードウェア構成

### 母艦

| 項目 | 値 |
|---|---|
| ホスト名 | `ryzen-desktop-1` |
| Tailscale FQDN | `ryzen-desktop-1.taila6a786.ts.net` |
| OS | Windows 11 Home |
| RAM | 15.9 GB（常時 92〜95% 使用中） |
| ストレージ C: | 953 GB（270 GB 使用） |
| ストレージ D: DATA | 5589 GB（534 GB 使用） |
| ネットワーク | Wi-Fi 2（400〜550 Mbps） |

### IP アドレス

| アドレス | 用途 |
|---|---|
| `192.168.0.5` | LAN（プライマリ） |
| `100.87.81.4` | Tailscale。llama-server はここにバインドされる |
| `192.168.192.1` | 仮想アダプタ（未使用） |

### GPU

CUDA / nvidia-smi ともにインデックスは以下の順。

| # | GPU | VRAM | 電力制限 | ドライバモデル | Bus | Arch |
|---|---|---|---|---|---|---|
| 0 | Tesla V100-SXM2-16GB | 16.0 GB | **100 W** | TCC | 01:00.0 | 70 (Volta) |
| 1 | NVIDIA GeForce GTX 1080 Ti ARMOR 11G OC | 11.0 GB | 280 W | WDDM | 0B:00.0 | 61 (Pascal) |

**性能特性の非対称性が重要。** V100 は HBM2 + テンソルコアで prompt processing が速い。
1080 Ti は Pascal でテンソルコアが無く、同じ仕事をさせると遅い。
そのため重み配分は V100 寄りにしてある（後述の `--tensor-split 5,2`）。

V100 は 100 W に絞られている（電源・冷却の都合）。定格 300 W の 1/3。

---

## 2. 推論サーバ（llama-server）

### 2-1. モデル

| 項目 | 値 |
|---|---|
| 本体 | `Qwen3.8-27B-Q4_K_M.gguf` (16.55 GB) |
| エイリアス | `qwen3.8-27b` |
| 画像エンコーダ | `mmproj-Qwen3.8-27B-f16.gguf` (0.86 GB) |
| 投機デコード | モデル内蔵の MTP（Multi-Token Prediction）ヘッド |
| 配置 | `C:\Users\tomoy\Git\qwen38-1080ti\models\qwen38\` |

退避用に QAT Q2_0 版（`qwen38-27b-qat-q2_0.gguf`, 8.15 GB）も置いてある（`-Q2` で切替）。

### 2-2. llama.cpp

| 項目 | 値 |
|---|---|
| コミット | `4d9176092` (b10818) |
| ビルド Arch | `61;70`（Pascal + Volta） |
| 配置 | `C:\Users\tomoy\Git\qwen38-1080ti\llama.cpp\build\bin\Release\llama-server.exe` |
| ビルドスクリプト | `10-build.ps1` |

### 2-3. 起動スクリプト `30-run-server.ps1`

常用の起動:

```powershell
.\30-run-server.ps1 -Tailscale -ApiKey <key>
```

#### パラメータ既定値

| パラメータ | 既定値 | 意味 |
|---|---|---|
| `-Ctx` | 49152 | コンテキスト長。KV f16 で約 65 KB/token → 約 3.1 GB |
| `-Port` | 8080 | |
| `-TensorSplit` | `5,2` | V100:1080Ti の重み配分（= 2.5:1）。実測でこれが最良 |
| `-Parallel` | 1 | 同時スロット数（`-np`）。単独利用なので 1 |
| `-UBatch` | **512** | 物理バッチ（`-ub`）。語彙 248k のロジットバッファがこれに比例（512 で約 508 MB） |
| `-KV` | `f16` | KV キャッシュ量子化。Pascal では量子化 KV にすると prompt 処理が約 10 倍遅くなる |
| `-ImageMaxTokens` | 1024 | 画像 1 枚あたりのトークン上限 |
| `-Think` | なし（off） | 付けると `--reasoning on` |
| `-NoMtp` / `-NoVision` / `-Spec` / `-Q2` / `-SafeKV` | — | 比較・退避用スイッチ |

#### llama-server に実際に渡される引数

```
-m       <models>\Qwen3.8-27B-Q4_K_M.gguf
--alias  qwen3.8-27b
-ngl     99                      # 全レイヤ GPU
--tensor-split 5,2
-np      1
-ub      512
-c       49152
-fa      on                      # Flash Attention
-ctk     f16
-ctv     f16
--temp            0.7
--dry-multiplier  0.8
--reasoning       off            # -Think で on
--cache-reuse     256            # ※ mmproj により無効化される（後述）
--jinja                          # tool_calls / chat_template_kwargs に必要
--spec-type       draft-mtp      # 内蔵 MTP 投機デコード
--mmproj          <models>\mmproj-Qwen3.8-27B-f16.gguf
--image-min-tokens 1024
--image-max-tokens 1024
--api-key         <key>
--host            100.87.81.4
--port            8080
```

#### 重要な副作用

起動時に必ずこの警告が出る。

```
W srv load_model: cache_reuse is not supported by multimodal, it will be disabled
```

**mmproj（画像入力）を読み込むと `--cache-reuse` が無効化される。**
画像入力は必須要件のため mmproj は外さない。したがって
**プロンプトキャッシュの部分再利用は効かない前提**で設計している。

### 2-4. 実測性能

| 指標 | 値 |
|---|---|
| prompt eval（prefill） | **364〜385 tok/s** |
| generation（decode） | **26.5〜33.0 tok/s** |
| 投機デコード採択率 | 短文 0.79〜1.00 / 長文 RAG 時 0.52〜0.59 |
| 画像エンコード | 1322 トークンで 5.1 秒 |
| VRAM 使用（ts 5,2） | V100 約 15.4 GB / 1080 Ti 約 9.0〜9.3 GB |

`-UBatch` 別の prefill 実測:

| `-ub` | prefill | 備考 |
|---|---|---|
| 256 | 約 171 tok/s | 旧既定 |
| **512** | **364〜385 tok/s** | 現行。最速 |
| 1024 | 339 tok/s | ロジットバッファ増で逆に低下 |

**VRAM に余裕が無い（両 GPU 合計 27 GB に対し本体 16.55 GB + mmproj 0.86 GB + KV 3.1 GB）**
ため、補助タスク用に 2 つめのモデルを載せる余地は無い。

---

## 3. Open WebUI

### 3-1. 稼働形態

| 項目 | 値 |
|---|---|
| 実行環境 | Docker Desktop（コンテナ名 `open-webui`） |
| ポート | 3000 / 3001 |
| 外部公開 | Tailscale serve 経由の HTTPS（`https://ryzen-desktop-1.taila6a786.ts.net`） |
| llama-server 接続 | OpenAI 互換接続 `http://100.87.81.4:8080/v1`（`connection_type = external`） |
| 記憶フォルダ | 母艦の `~/Git/qwen38-1080ti/memory` をコンテナ内 `/app/memory` に bind mount |

### 3-2. モデル設定（`qwen3.8-27b`）

**システムプロンプト:**

```
検索結果が提供されている場合は、学習知識より検索結果を優先して回答してください。
回答は簡潔に。箇条書きは合計5項目以内、前置きと締めの一文は書かないこと。
```

**フィルタ:** `Force_web_search` と `Memory File Injector` の両方が有効。

**機能:**

| 有効 | 無効 |
|---|---|
| 視覚 / ファイルアップロード / ファイルコンテキスト / ウェブ検索 / 画像生成 / コードインタプリタ / Terminal / 引用 / ステータス更新 / メモリ | 使用量 / 組み込みツール |

**デフォルト機能:** ウェブ検索 **ON**（新規チャットは常に検索有効で始まる）

**Function Calling:** `legacy`（native ではない）。したがって Open WebUI 側の
「クエリ生成 → 検索 → RAG 注入」という強制パスが使われ、モデルが自分で
`search_web` ツールを呼ぶ経路は使っていない。

### 3-3. ウェブ検索設定

| 項目 | 値 |
|---|---|
| ウェブ検索 | 有効 |
| Web Search Confirmation | 無効 |
| 検索エンジン | DDGS |
| **DDGS Backend** | **duckduckgo 固定**（「自動（ランダム）」は不可。理由は §6-2） |
| 検索結果数 | 2（**クエリ1本あたり**。2 クエリなら 4 URL） |
| 同時リクエスト（検索） | 0（無制限） |
| Fetch URL Content Length Limit | 2500 ※ウェブ検索には効かない（§6-3） |
| ドメインフィルター | なし |
| **埋め込みと検索をバイパス** | **OFF**（= チャンク分割して top_k 取得する） |
| Web ローダーをバイパス | OFF（= ページ本文を実際に取得する） |
| プロキシ環境を信頼する | ON |
| ウェブローダーエンジン | デフォルト（`SafeWebBaseLoader`。aiohttp + BeautifulSoup、JS 実行なし） |
| SSL 証明書を確認 | ON |
| 同時リクエスト（ローダー） | 10 |

### 3-4. ドキュメント（RAG）設定

| 項目 | 値 |
|---|---|
| 埋め込みエンジン | デフォルト（SentenceTransformers、Open WebUI コンテナ内 CPU 実行） |
| **埋め込みモデル** | **`intfloat/multilingual-e5-small`** |
| 埋め込みバッチサイズ | 16 |
| フルコンテキストモード | OFF |
| ハイブリッド検索 | OFF |
| Reranking Batch Size | 32 |
| **トップ K** | **6** |
| RAG テンプレート | 既定 |
| チャンク分割 | markdown header text splitter |

**埋め込みモデルは日本語対応が必須。** 既定の `sentence-transformers/all-MiniLM-L6-v2`
は英語専用で、日本語では類似度がほぼ機能せず、検索結果からの関連チャンク選択が
実質ランダムになる（§6-1）。

### 3-5. インターフェース（タスク）設定

| 項目 | 値 |
|---|---|
| ローカルタスクモデル | 現在のモデル |
| 外部タスクモデル | 現在のモデル（= qwen3.8-27b 自身が補助タスクも処理する） |
| タイトル生成 | OFF |
| 検索クエリ生成 | ON |
| 検索クエリ生成プロンプト | **空欄（= Open WebUI の既定テンプレートを使う）** |

補助タスク用の小さいモデルを別途立てる案は、VRAM に余裕が無いため見送っている。

---

## 4. Filter 関数

Open WebUI の Filter は **DB（`function` テーブルの `content` カラム）に保存され、
リクエストのたびに DB から読み込まれる**。ホスト側の `.py` ファイルは参照されない。
変更は管理者パネル → 関数 のエディタで保存する必要がある。

### 4-1. `force_web_search` v2.0.0

**目的:** スマホアプリなどの API 直叩きのときだけ `web_search` を有効化する。
ブラウザ UI の検索トグルには一切干渉しない。

**判定ロジック（`inlet`）:**

| 条件 | 判定 | 動作 |
|---|---|---|
| `metadata` に `task` または `task_body` あり | 内部補助タスク | 素通し |
| `metadata` に `session_id` あり | ブラウザ UI | 素通し（UI のトグルに一任） |
| どちらも無い | API 直叩き | `features.web_search = True` を注入 |

**実装上の要点:** `metadata` は `inlet` の引数 `__metadata__` から取る。
`body["metadata"]` や `body["session_id"]` を見てはいけない（§6-4）。

**Valves:** `priority=0`, `enabled=true`, `force_override=false`, `debug=true`

### 4-2. `Memory File Injector` v0.2.0

**目的:** `memory/*.md` を system prompt の先頭に固定注入し、
「記憶して」等のキーワードで会話から事実を抽出して `inbox.md` に追記する。

**Valves:**

| Valve | 値 |
|---|---|
| `memory_dir` | `/app/memory` |
| `files` | `profile.md,projects.md,inbox.md` |
| `web_search_files` | `profile.md,projects.md,inbox.md`（= 検索の有無によらず常時注入） |
| `skip_on_internal_task` | true |
| `max_chars_per_file` | 6000 |
| `max_chars_total` | 12000 |
| `append_datetime` | true |
| `datetime_format` | `%Y-%m-%d %H:00 (%a)`（時単位。分まで入れると prefix cache が毎分壊れる） |
| `trigger_keywords` | 記憶して, 覚えておいて, 覚えて, メモして, remember this, save to memory |
| `inbox_file` | `inbox.md` |
| `context_turns` | 8 |
| `api_base` | `http://100.87.81.4:8080` |
| `model` | `qwen3.8-27b` |
| `max_tokens` | 600 |
| `disable_thinking` | true（`chat_template_kwargs.enable_thinking=false` を送る） |
| `debug` | true |

**注入の形:**

```
[system メッセージ]
  <memory>
  以下はユーザーに関する長期記憶（過去のセッションで確定した事実）。
  回答の前提として使うこと。ここに無いことは知っているふりをしない。

  ### profile.md
  …
  ### projects.md
  …
  ### inbox.md
  …
  </memory>

  （モデル設定側のシステムプロンプト）

  現在日時: 2026-09-06 17:00 (Sat)
```

記憶ブロックが**先頭**に来るのは、llama-server のプレフィックスキャッシュを
効かせるため（ただし mmproj により `--cache-reuse` は無効なので、現状は
スロットのキャッシュがそのまま残っている場合のみ効く）。

**現在の注入量: 約 1,615 文字。**

**書き込み（`outlet`）:** 直近のユーザー発言に `trigger_keywords` が含まれるときだけ、
llama-server に別リクエストを投げて会話から事実を抽出し、`inbox.md` に追記する。
`inbox.md` は追記専用なので、放置すると増え続けて毎ターンのコストになる。
定期的に `profile.md` / `projects.md` に統合すること。

### 4-3. `memory/` の構成

```
~/Git/qwen38-1080ti/memory/
├── profile.md     約 4 KB  ユーザーのプロフィール
├── projects.md    約 3 KB  進行中プロジェクト
├── inbox.md       約 1 KB  抽出された事実の追記先
├── README.md      約 6 KB  説明書（注入対象外）
└── projects/      サブフォルダ
```

---

## 5. 1 ターンで起きること

### 5-1. 検索 ON のブラウザチャット（実測）

```
[1] POST /api/chat/completions
      ↓
[2] Filter inlet
      force_web_search       → session_id あり → 素通し
      Memory File Injector   → 記憶 約1,615文字を system 先頭に注入
      ↓
[3] 検索クエリ生成タスク（llama-server 往復 1 回目）
      既定テンプレート + 直近 6 メッセージ（記憶入り）
      → {"queries": [...]} または {"queries": []}
      空なら検索をスキップし「検索クエリは生成されません」と表示
      ↓
[4] DuckDuckGo 検索 → URL 収集（クエリ数 × 検索結果数）
      ↓
[5] SafeWebBaseLoader でページ本文取得（同時 10、失敗は黙って除外）
      ↓
[6] markdown header splitter でチャンク化
      → multilingual-e5-small で埋め込み（CPU、バッチ 16）
      → 一時コレクション web-search-<userid>-<hash> に格納
      ↓
[7] クエリごとに top_k=6 で取得 → コンテキストとして messages に注入
      ↓
[8] 本チャット（llama-server 往復 2 回目）
      ↓
[9] Filter outlet
      Memory File Injector → トリガー語があれば抽出（llama-server 往復 3 回目）
```

### 5-2. 実測時間（「川口市の天気」1 ターン）

| 区間 | 時間 |
|---|---|
| クエリ生成 | 6〜9 秒 |
| 検索 API | 2〜4 秒 |
| ページ取得（4〜8 ページ） | 0.6〜3 秒 |
| 埋め込み + 取得（16〜40 チャンク） | 2〜4 秒 |
| 本チャット prefill（4,000〜4,700 トークン） | 11〜12 秒 |
| 本チャット生成（350〜440 トークン） | 13〜17 秒 |
| **合計** | **約 30〜45 秒** |

検索 OFF のターンは本チャット 1 回だけなので **1.5〜3 秒**。

### 5-3. API 直叩き（スマホアプリ）

`session_id` が無いため `force_web_search` が `web_search=True` を注入する。
以降の流れはブラウザと同じ。

---

## 6. 設計上の決定と、その理由

### 6-1. 埋め込みモデルは日本語対応必須

Open WebUI の既定 `sentence-transformers/all-MiniLM-L6-v2` は英語専用。
日本語で使うと、天気を聞いているのに「サイトのメニュー項目一覧」「過去の天気アーカイブ」
「全国平均気温の統計」といったチャンクが上位に来る。
`intfloat/multilingual-e5-small` に変えて解決した。

### 6-2. DDGS Backend は固定する

「自動（ランダム）」だと毎回別のバックエンドに当たり、
Brave が 429、Yandex が低品質、Google が `hl=en-US&cr=countryUS` で英語圏検索、
grokipedia が 5 秒タイムアウト、という状態になる。`duckduckgo` 固定で安定した。

### 6-3. `Fetch URL Content Length Limit` はウェブ検索に効かない

この設定（`web.fetch.max_content_length`）はネイティブ関数呼び出しの
`fetch_url` ツール専用。`process_web_search` → `get_web_loader()` の経路には渡らない。
ウェブ検索の挙動は変わらないので、この値をいじっても意味がない。

### 6-4. Filter の `inlet` では `body["metadata"]` は存在しない

Open WebUI は `utils/middleware.py` の `process_chat_payload()` で

1. Filter の `inlet` を実行
2. **そのあと** `form_data["metadata"] = metadata` を代入

という順序になっている（v0.6.x〜v0.8.x で共通）。
また `session_id` は `main.py` の `chat_completion()` が
`form_data.pop("session_id", None)` で body から抜いて別 dict に移す。

したがって `inlet` で `body.get("metadata")` も `body.get("session_id")` も
**常に空**になる。正しい取得口は `inlet` の引数 `__metadata__`。

### 6-5. 画像入力は必須要件

そのため mmproj を外さない。結果として `--cache-reuse` は無効のままとなる。
「速度のために mmproj を外す」という選択肢は**採らない**。

### 6-6. 記憶は検索の有無によらず常時 ON

検索ターンで記憶注入をスキップすれば 1 ターンあたり約 11 秒短縮できるが、
「私の名前は？」のような質問に答えられなくなるため、常時 ON を選択している。

### 6-7. 補助タスクは同じ 27B が処理する

`TASK_MODEL_EXTERNAL` に小さいモデルを割り当てれば速くなるが、
VRAM に余裕が無いため見送り。代わりにタイトル生成を OFF にして往復を減らしている。

---

## 7. 監視（sparkDash）

母艦の GPU / RAM / ネットワーク / ストレージと LLM サービスを監視するダッシュボード。

| Spark | GPU 監視 | LLM 監視 |
|---|---|---|
| `ryzen-v100`（Tesla V100） | ○（`gpuIndex: 0`） | ○（:8080、API キーあり） |
| `ryzen-1080ti`（GTX 1080 Ti） | ○（`gpuIndex: 1`） | **✕（LLM monitoring 無効）** |

llama-server は tensor-split で 2 枚にまたがる 1 プロセスなので、
LLM サービスの登録は 1 つだけにしてある。2 つ登録すると API キーが
Spark ごとに別管理（`llmApiKeys[sparkId][port]`）になり、片方だけ古いキーのまま
`Invalid API Key` を出し続けることになる。

---

## 8. 既知の制約・注意点

1. **検索クエリに日付が入ると過去アーカイブに刺さることがある。**
   tenki.jp は日付ごとに「過去の天気」ページを持っているため、
   「2026年9月6日」を含むクエリがそちらにマッチする場合がある。

2. **`inbox.md` は追記専用で増え続ける。** 毎ターン 2 回 prefill に乗るため、
   定期的に `profile.md` / `projects.md` に統合して空にする運用が必要。

3. **`SafeWebBaseLoader` は JS を実行しない。** JS レンダリングのサイトや
   403 を返すサイトは本文が取れず、黙って結果から除外される。

4. **検索ターンはプレフィックスキャッシュが構造的に効かない。**
   毎回新しいページ本文が入るため、`f_sim_best` が 0.1 前後になる。

5. **RAM が常時 92〜95%。** Open WebUI コンテナ、Docker Desktop、
   llama-server が同居しているため余裕が無い。

6. **Ollama と共存できない。** llama-server が上がっている間は
   Ollama 側のモデルは VRAM に載らない（CPU に溢れて極端に遅くなる）。
   起動スクリプトが自動で `ollama stop` を実行する。

---

## 9. ファイル配置

```
C:\Users\tomoy\Git\qwen38-1080ti\
├── 10-build.ps1              llama.cpp のビルド
├── 20-download-models.ps1    モデルのダウンロード
├── 30-run-server.ps1         llama-server の起動
├── llama.cpp\build\bin\Release\llama-server.exe
├── models\
│   ├── qwen38\
│   │   ├── Qwen3.8-27B-Q4_K_M.gguf
│   │   ├── mmproj-Qwen3.8-27B-f16.gguf
│   │   └── qwen38-27b-qat-q2_0.gguf     （退避用）
│   └── dflash2\                          （外部ドラフト、現在未使用）
└── memory\                               → コンテナ内 /app/memory
    ├── profile.md
    ├── projects.md
    ├── inbox.md
    └── README.md
```
