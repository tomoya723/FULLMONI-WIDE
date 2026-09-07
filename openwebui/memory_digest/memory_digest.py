#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
memory_digest.py — Open WebUI のチャット履歴から事実を抽出し memory/*.md を更新する夜間バッチ

前提（このリポジトリの README / LOCAL_LLM_ENVIRONMENT.md の構成）:
  - Open WebUI は Docker Desktop 上のコンテナ `open-webui`
  - DB は named volume `open-webui` の中（/app/backend/data/webui.db, SQLite + WAL）
    → ホストから直接触れないので `docker exec` 経由で読む（読み取り専用）
  - memory/ は bind mount
      host: C:\\Users\\tomoy\\Git\\qwen38-1080ti\\memory
      cont: /app/memory
    → ホスト側から直接書ける
  - 推論は llama-server (http://100.87.81.4:8080, OpenAI 互換)

処理の流れ:
  1. llama-server の生存確認（落ちていたら何もせず終了）
  2. コンテナ内で webui.db を読み、直近 N 時間に更新されたチャットを JSON で吸い出す
  3. 既に処理済みのチャットは state ファイルで除外
  4. llama-server に投げて「新しく分かった事実」を抽出
  5. memory/ をバックアップしてから inbox.md に日付付きで追記
  6. 週1（既定: 日曜 or 前回統合から7日以上）で profile.md / projects.md へ統合

標準ライブラリのみ。母艦の Python 3.8+ でそのまま動く。
"""

from __future__ import annotations

import argparse
import datetime as dt
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import urllib.error
import urllib.request
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

# --------------------------------------------------------------------------- 既定値

DEFAULT_CONTAINER = "open-webui"
DEFAULT_DB_PATH = "/app/backend/data/webui.db"
DEFAULT_MEMORY_DIR = r"C:\Users\tomoy\Git\qwen38-1080ti\memory"
DEFAULT_API_BASE = "http://100.87.81.4:8080"
DEFAULT_MODEL = "qwen3.8-27b"

STATE_FILE = ".digest_state.json"
BACKUP_DIR = ".backup"
INBOX_FILE = "inbox.md"
PROFILE_FILE = "profile.md"
PROJECTS_FILE = "projects.md"

# 1リクエストに詰め込む会話本文の上限（文字）。超えたら複数リクエストに分割する。
MAX_CHARS_PER_REQUEST = 20000
# 1メッセージあたりの上限（長大な貼り付けログを丸ごと投げない）
MAX_CHARS_PER_MESSAGE = 2000
# 1チャットあたりに使うメッセージ数の上限（末尾から）
MAX_MESSAGES_PER_CHAT = 40

# --------------------------------------------------------------------------- ログ

_VERBOSE = False


def log(*args: Any) -> None:
    ts = dt.datetime.now().strftime("%H:%M:%S")
    print(f"[{ts}]", *args, flush=True)


def vlog(*args: Any) -> None:
    if _VERBOSE:
        log(*args)


# --------------------------------------------------------------------------- コンテナから DB を読む

# コンテナ内で実行する抽出スクリプト。stdout ではなくファイルに書いて docker cp で回収する
# （Windows の docker exec 経由だと stdout のエンコーディングで日本語が壊れることがあるため）。
_EXTRACT_PY = r'''# -*- coding: utf-8 -*-
import json, sqlite3, sys, time

db_path = sys.argv[1]
since = float(sys.argv[2])
out_path = sys.argv[3]

def connect(path):
    try:
        return sqlite3.connect("file:%s?mode=ro" % path, uri=True)
    except sqlite3.Error:
        return sqlite3.connect(path)

conn = connect(db_path)
conn.row_factory = sqlite3.Row

def norm_ts(v):
    """Open WebUI の updated_at は秒だが、版によってミリ秒/マイクロ秒のことがある。"""
    try:
        v = float(v)
    except (TypeError, ValueError):
        return 0.0
    while v > 1e11:
        v /= 1000.0
    return v

rows = conn.execute(
    "select id, title, updated_at, created_at, archived, chat from chat"
).fetchall()

out = []
for r in rows:
    ts = norm_ts(r["updated_at"]) or norm_ts(r["created_at"])
    if ts <= since:
        # since と同時刻のチャットは前回処理済み（state の last_chat_updated_at）
        continue
    try:
        chat = json.loads(r["chat"]) if r["chat"] else {}
    except Exception:
        chat = {}
    out.append({
        "id": r["id"],
        "title": r["title"] or "",
        "updated_at": ts,
        "archived": bool(r["archived"]),
        "chat": chat,
    })

out.sort(key=lambda c: c["updated_at"])
with open(out_path, "w", encoding="utf-8") as f:
    json.dump({"chats": out, "now": time.time()}, f, ensure_ascii=False)
print("extracted %d chats" % len(out))
'''


def run(cmd: List[str], **kw: Any) -> subprocess.CompletedProcess:
    vlog("$", " ".join(cmd))
    return subprocess.run(cmd, capture_output=True, **kw)


def fetch_chats(container: str, db_path: str, since_epoch: float) -> List[dict]:
    """コンテナ内の webui.db から since_epoch 以降に更新されたチャットを取得する。"""
    tmpdir = Path(tempfile.mkdtemp(prefix="owui_digest_"))
    try:
        local_py = tmpdir / "extract_chats.py"
        local_py.write_text(_EXTRACT_PY, encoding="utf-8")

        cp = run(["docker", "cp", str(local_py), f"{container}:/tmp/extract_chats.py"])
        if cp.returncode != 0:
            raise RuntimeError(f"docker cp に失敗: {cp.stderr.decode('utf-8', 'replace').strip()}")

        ex = run([
            "docker", "exec", container,
            "python", "/tmp/extract_chats.py", db_path, str(since_epoch), "/tmp/owui_chats.json",
        ])
        if ex.returncode != 0:
            raise RuntimeError(
                "コンテナ内での抽出に失敗: " + ex.stderr.decode("utf-8", "replace").strip()
            )
        vlog(ex.stdout.decode("utf-8", "replace").strip())

        local_json = tmpdir / "owui_chats.json"
        back = run(["docker", "cp", f"{container}:/tmp/owui_chats.json", str(local_json)])
        if back.returncode != 0:
            raise RuntimeError(f"docker cp (回収) に失敗: {back.stderr.decode('utf-8', 'replace').strip()}")

        data = json.loads(local_json.read_text(encoding="utf-8"))
        return data.get("chats", [])
    finally:
        run(["docker", "exec", container, "rm", "-f", "/tmp/extract_chats.py", "/tmp/owui_chats.json"])
        shutil.rmtree(tmpdir, ignore_errors=True)


# --------------------------------------------------------------------------- チャット JSON → 平文

def _clean(text: str) -> str:
    """<think> ブロックとソース引用マーカーを落とし、長さを詰める。"""
    text = re.sub(r"<think>.*?</think>", "", text, flags=re.S)
    text = re.sub(r"<details[^>]*>.*?</details>", "", text, flags=re.S)
    text = re.sub(r"\[\d+\]", "", text)
    text = re.sub(r"\n{3,}", "\n\n", text).strip()
    if len(text) > MAX_CHARS_PER_MESSAGE:
        text = text[:MAX_CHARS_PER_MESSAGE] + " …(略)"
    return text


def _branch_from_history(hist: dict) -> List[dict]:
    """history.messages（分岐ツリー）から、表示中の枝を古い順で取り出す。

    Open WebUI は currentId が指す葉から parentId を辿った経路を画面に出している。
    currentId が無い版のために timestamp 順のフォールバックも持つ。
    """
    raw = hist.get("messages")
    if isinstance(raw, list):
        return [m for m in raw if isinstance(m, dict)]
    if not isinstance(raw, dict) or not raw:
        return []

    cur = hist.get("currentId") or hist.get("current_id")
    if cur and cur in raw:
        chain: List[dict] = []
        seen = set()
        node = cur
        while node and node in raw and node not in seen:
            seen.add(node)
            m = raw[node]
            if isinstance(m, dict):
                chain.append(m)
            node = m.get("parentId") or m.get("parent_id") if isinstance(m, dict) else None
        chain.reverse()
        if chain:
            return chain

    return sorted(
        (m for m in raw.values() if isinstance(m, dict)),
        key=lambda m: m.get("timestamp") or 0,
    )


def _content_text(content: Any) -> str:
    """content が文字列でない版（マルチモーダルの配列）にも対応する。"""
    if isinstance(content, str):
        return content
    if isinstance(content, list):
        return "\n".join(
            p.get("text", "")
            for p in content
            if isinstance(p, dict) and p.get("type") == "text"
        )
    return ""


def messages_of(chat_obj: dict) -> List[Tuple[str, str]]:
    """Open WebUI の chat カラムから (role, content) のリストを取り出す。

    版によって形が違う:
      - chat["history"]["messages"] : id -> message の分岐ツリー（本体）
      - chat["messages"]            : 配列。版によっては先頭1件しか入っていない

    どちらか件数の多いほうを採る。実測（2026-09 時点の構成）では
    chat["messages"] が 1 件しか無く、history 側に 6 件入っていた。
    """
    hist = chat_obj.get("history")
    from_hist = _branch_from_history(hist) if isinstance(hist, dict) else []

    flat = chat_obj.get("messages")
    from_flat = [m for m in flat if isinstance(m, dict)] if isinstance(flat, list) else []

    seq = from_hist if len(from_hist) >= len(from_flat) else from_flat

    out: List[Tuple[str, str]] = []
    for m in seq:
        role = m.get("role") or ""
        if role not in ("user", "assistant"):
            continue
        content = _clean(_content_text(m.get("content")))
        if content:
            out.append((role, content))
    return out[-MAX_MESSAGES_PER_CHAT:]


def render_chat(chat: dict) -> str:
    lines = [f"### チャット: {chat.get('title') or '(無題)'}"]
    when = dt.datetime.fromtimestamp(chat.get("updated_at") or 0).strftime("%Y-%m-%d %H:%M")
    lines.append(f"(最終更新 {when})")
    for role, content in messages_of(chat.get("chat") or {}):
        who = "ユーザー" if role == "user" else "アシスタント"
        lines.append(f"\n**{who}:** {content}")
    return "\n".join(lines)


def batch_chats(chats: List[dict], limit: int) -> List[List[str]]:
    """レンダリング済みチャットを文字数上限で複数バッチに割る。"""
    batches: List[List[str]] = []
    cur: List[str] = []
    cur_len = 0
    for c in chats:
        text = render_chat(c)
        if not text.strip():
            continue
        if len(text) > limit:
            text = text[:limit] + "\n…(以下略)"
        if cur and cur_len + len(text) > limit:
            batches.append(cur)
            cur, cur_len = [], 0
        cur.append(text)
        cur_len += len(text)
    if cur:
        batches.append(cur)
    return batches


# --------------------------------------------------------------------------- llama-server

class Llama:
    def __init__(self, base: str, api_key: str, model: str, timeout: int, no_think: bool):
        self.base = base.rstrip("/")
        self.api_key = api_key
        self.model = model
        self.timeout = timeout
        self.no_think = no_think

    def _headers(self) -> Dict[str, str]:
        h = {"Content-Type": "application/json"}
        if self.api_key:
            h["Authorization"] = f"Bearer {self.api_key}"
        return h

    def alive(self) -> bool:
        req = urllib.request.Request(f"{self.base}/v1/models", headers=self._headers())
        try:
            with urllib.request.urlopen(req, timeout=10) as r:
                return r.status == 200
        except Exception as e:  # noqa: BLE001
            vlog("alive check failed:", e)
            return False

    def chat(self, system: str, user: str, max_tokens: int, temperature: float = 0.2) -> str:
        payload: Dict[str, Any] = {
            "model": self.model,
            "messages": [
                {"role": "system", "content": system},
                {"role": "user", "content": user},
            ],
            "temperature": temperature,
            "max_tokens": max_tokens,
            "stream": False,
        }
        if self.no_think:
            payload["chat_template_kwargs"] = {"enable_thinking": False}

        req = urllib.request.Request(
            f"{self.base}/v1/chat/completions",
            data=json.dumps(payload, ensure_ascii=False).encode("utf-8"),
            headers=self._headers(),
            method="POST",
        )
        with urllib.request.urlopen(req, timeout=self.timeout) as r:
            body = json.loads(r.read().decode("utf-8"))
        text = body["choices"][0]["message"].get("content") or ""
        return re.sub(r"<think>.*?</think>", "", text, flags=re.S).strip()


# --------------------------------------------------------------------------- プロンプト

EXTRACT_SYSTEM = """\
あなたは会話ログから「長期記憶に残す価値のある事実」だけを抜き出す抽出エンジンです。

出力ルール:
- 箇条書きのみ。前置き・要約・感想・挨拶は一切書かない。
- 各行は `- [カテゴリ] 事実` の形式。カテゴリは profile / project / preference / todo のいずれか。
  - profile    : ユーザー本人の属性・環境・所有物（職業、機材、OS、ネットワーク構成など）
  - project    : 進行中の作業・構築物・その設定値や決定事項
  - preference : 好み・方針・禁止事項（「〜はマスト」「〜はしない」など）
  - todo       : 未完了の宿題・次にやると決めたこと
- 1行1事実。固有名詞・数値・パス・バージョンは省略せずそのまま残す。
- 一時的な話題（その場限りの質問、雑談、天気の問い合わせ結果など）は書かない。
- 既知の事実（後述の「既存の記憶」に既に書かれている内容）は書かない。
  ただし既存の記述と食い違う場合は `- [profile] 更新: ...（旧: ...）` の形で1行書く。
- 抽出すべき事実が1つも無ければ `NONE` とだけ出力する。
"""

EXTRACT_USER_TMPL = """\
## 既存の記憶（ここに書かれている内容は再掲しないこと）

{memory}

## 新しい会話ログ

{conversations}

## 指示

上記の会話ログから、既存の記憶に無い新しい事実だけを抽出して箇条書きで出力せよ。
"""

CONSOLIDATE_SYSTEM = """\
あなたは長期記憶ファイルの編集者です。既存の記憶ファイルと、未整理の受信箱（inbox）を受け取り、
受信箱の内容を既存ファイルへ統合した「完全な全文」を返します。

厳守事項:
- 既存の記述は原則として削除しない。矛盾する新情報がある場合のみ、古い記述を新しい記述で置き換える。
- 重複する記述はまとめる。
- Markdown の見出し構成は既存ファイルのものを踏襲する。既存が空なら適切な見出しを作る。
- 事実のみを書く。推測・脚色・埋め合わせをしない。
- 出力は次の JSON オブジェクト1つだけ。前後に説明やコードフェンス以外の文字を書かない。

{"profile": "<profile.md の全文>", "projects": "<projects.md の全文>", "consumed": ["統合済みの inbox 見出し", ...]}
"""

CONSOLIDATE_USER_TMPL = """\
## 現在の profile.md

{profile}

## 現在の projects.md

{projects}

## 未整理の inbox.md

{inbox}

## 指示

inbox の内容を profile / projects へ統合し、指定の JSON で全文を返せ。
ユーザー本人の属性・環境は profile、作業や構築物とその設定は projects に振り分ける。
"""


# --------------------------------------------------------------------------- ファイル操作

def read_text(path: Path) -> str:
    if not path.exists():
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    # 改行は LF に統一（bind mount 越しにコンテナから読むため）
    # Path.write_text の newline 引数は 3.10+ なので open() を使う
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        f.write(text.replace("\r\n", "\n"))


def backup_memory(memory_dir: Path) -> Path:
    stamp = dt.datetime.now().strftime("%Y-%m-%d_%H%M%S")
    dest = memory_dir / BACKUP_DIR / stamp
    dest.mkdir(parents=True, exist_ok=True)
    for p in memory_dir.glob("*.md"):
        shutil.copy2(p, dest / p.name)
    return dest


def prune_backups(memory_dir: Path, keep: int) -> None:
    root = memory_dir / BACKUP_DIR
    if not root.is_dir():
        return
    dirs = sorted((d for d in root.iterdir() if d.is_dir()), key=lambda d: d.name)
    for d in dirs[:-keep] if keep > 0 else []:
        shutil.rmtree(d, ignore_errors=True)
        vlog("古いバックアップを削除:", d.name)


def load_state(memory_dir: Path) -> dict:
    p = memory_dir / STATE_FILE
    if not p.exists():
        return {}
    try:
        return json.loads(p.read_text(encoding="utf-8"))
    except Exception:  # noqa: BLE001
        return {}


def save_state(memory_dir: Path, state: dict) -> None:
    (memory_dir / STATE_FILE).write_text(
        json.dumps(state, ensure_ascii=False, indent=2), encoding="utf-8"
    )


# --------------------------------------------------------------------------- 統合

def parse_json_object(text: str) -> Optional[dict]:
    """モデル出力から JSON オブジェクトを取り出す（コードフェンス付きも許容）。"""
    m = re.search(r"```(?:json)?\s*(\{.*?\})\s*```", text, flags=re.S)
    candidate = m.group(1) if m else None
    if candidate is None:
        start = text.find("{")
        end = text.rfind("}")
        if start == -1 or end <= start:
            return None
        candidate = text[start : end + 1]
    try:
        obj = json.loads(candidate)
        return obj if isinstance(obj, dict) else None
    except json.JSONDecodeError:
        return None


def consolidate(llama: Llama, memory_dir: Path, dry_run: bool, force: bool) -> bool:
    profile = read_text(memory_dir / PROFILE_FILE)
    projects = read_text(memory_dir / PROJECTS_FILE)
    inbox = read_text(memory_dir / INBOX_FILE)

    if not inbox.strip():
        log("統合: inbox.md が空なので何もしない")
        return False

    user = CONSOLIDATE_USER_TMPL.format(
        profile=profile or "(空)", projects=projects or "(空)", inbox=inbox
    )
    log("統合: llama-server に問い合わせ中 …")
    raw = llama.chat(CONSOLIDATE_SYSTEM, user, max_tokens=6000, temperature=0.1)
    obj = parse_json_object(raw)
    if not obj:
        log("統合: JSON を解釈できなかったので中止（既存ファイルは無傷）")
        vlog("raw:", raw[:1000])
        return False

    new_profile = (obj.get("profile") or "").strip()
    new_projects = (obj.get("projects") or "").strip()
    if not new_profile and not new_projects:
        log("統合: 出力が空だったので中止")
        return False

    # 縮小ガード: 既存の半分未満に縮んだら事故とみなして中止
    for name, old, new in (
        (PROFILE_FILE, profile, new_profile),
        (PROJECTS_FILE, projects, new_projects),
    ):
        if old.strip() and len(new) < len(old) * 0.5 and not force:
            log(
                f"統合: {name} が {len(old)} → {len(new)} 文字に縮んだため中止"
                "（意図的なら --force）"
            )
            return False

    if dry_run:
        log("統合 (dry-run): 以下を書き込む予定")
        print("----- profile.md -----")
        print(new_profile)
        print("----- projects.md -----")
        print(new_projects)
        return False

    dest = backup_memory(memory_dir)
    log("統合: バックアップ", dest)
    if new_profile:
        write_text(memory_dir / PROFILE_FILE, new_profile + "\n")
    if new_projects:
        write_text(memory_dir / PROJECTS_FILE, new_projects + "\n")

    # 統合済みの inbox は退避してから空にする（原本はバックアップに残っている）
    archived = memory_dir / BACKUP_DIR / f"inbox_{dt.datetime.now():%Y-%m-%d_%H%M%S}.md"
    write_text(archived, inbox)
    write_text(
        memory_dir / INBOX_FILE,
        f"<!-- {dt.datetime.now():%Y-%m-%d} に profile.md / projects.md へ統合済み -->\n",
    )
    log(f"統合: 完了（profile {len(new_profile)}字 / projects {len(new_projects)}字）")
    return True


# --------------------------------------------------------------------------- 診断

def probe(chats: List[dict], n: int) -> int:
    """チャット JSON の構造を表示する。messages_of が読めているかの切り分け用。"""
    total_chars = 0
    empty = 0
    for c in chats:
        got = messages_of(c.get("chat") or {})
        chars = sum(len(t) for _, t in got)
        total_chars += chars
        if chars == 0:
            empty += 1

    log(f"チャット {len(chats)} 件 / 抽出できた本文 合計 {total_chars} 文字 / 本文ゼロ {empty} 件")
    log(f"--- 直近 {min(n, len(chats))} 件の構造 ---")

    for c in chats[-n:]:
        obj = c.get("chat") or {}
        print()
        print(f"■ {c.get('title') or '(無題)'}  [{c.get('id', '')[:8]}]")
        print(f"   トップレベルのキー: {sorted(obj.keys())}")

        m = obj.get("messages")
        print(f"   chat.messages          : {type(m).__name__}"
              + (f" / {len(m)} 件" if hasattr(m, "__len__") else ""))

        hist = obj.get("history")
        h = hist.get("messages") if isinstance(hist, dict) else None
        print(f"   chat.history.messages  : {type(h).__name__}"
              + (f" / {len(h)} 件" if hasattr(h, "__len__") else ""))

        # 生のメッセージ1件の形を見る（role と content の型）
        raw = None
        if isinstance(m, list) and m:
            raw = m[0]
        elif isinstance(h, dict) and h:
            raw = next(iter(h.values()))
        elif isinstance(h, list) and h:
            raw = h[0]
        if isinstance(raw, dict):
            print(f"   生メッセージのキー     : {sorted(raw.keys())}")
            print(f"   role={raw.get('role')!r} content の型={type(raw.get('content')).__name__}")

        got = messages_of(obj)
        chars = sum(len(t) for _, t in got)
        print(f"   採用した枝            : {len(_branch_from_history(hist) if isinstance(hist, dict) else [])} 件 (history) "
              f"vs {len(m) if isinstance(m, list) else 0} 件 (messages)")
        print(f"   messages_of -> {len(got)} 件 / {chars} 文字")
        if got:
            print(f"   先頭: {got[0][1][:120]}")
    return 0


# --------------------------------------------------------------------------- メイン

def main() -> int:
    global _VERBOSE

    ap = argparse.ArgumentParser(
        description="Open WebUI のチャットから事実を抽出して memory/*.md を更新する夜間バッチ"
    )
    ap.add_argument("--container", default=DEFAULT_CONTAINER)
    ap.add_argument("--db-path", default=DEFAULT_DB_PATH)
    ap.add_argument("--memory-dir", default=os.environ.get("QWEN_MEMORY_DIR", DEFAULT_MEMORY_DIR))
    ap.add_argument("--api-base", default=os.environ.get("LLAMA_API_BASE", DEFAULT_API_BASE))
    ap.add_argument("--api-key", default=os.environ.get("LLAMA_API_KEY", ""))
    ap.add_argument("--model", default=DEFAULT_MODEL)
    ap.add_argument("--window-hours", type=float, default=24.0, help="遡る時間（既定 24h）")
    ap.add_argument("--max-tokens", type=int, default=1200, help="抽出1回あたりの出力上限")
    ap.add_argument("--timeout", type=int, default=600, help="1リクエストのタイムアウト秒")
    ap.add_argument("--think", action="store_true", help="thinking を有効にする（既定は無効で高速）")
    ap.add_argument("--consolidate", action="store_true", help="今回かならず統合まで行う")
    ap.add_argument("--no-consolidate", action="store_true", help="統合を行わない")
    ap.add_argument("--consolidate-days", type=int, default=7, help="統合の間隔（日）")
    ap.add_argument("--keep-backups", type=int, default=14, help="残すバックアップ世代数（0で無制限）")
    ap.add_argument("--include-archived", action="store_true", help="アーカイブ済みチャットも対象にする")
    ap.add_argument("--dry-run", action="store_true", help="書き込まず結果を表示するだけ")
    ap.add_argument("--force", action="store_true", help="統合の縮小ガードを無視する")
    ap.add_argument("-v", "--verbose", action="store_true")
    ap.add_argument(
        "--probe",
        type=int,
        default=0,
        metavar="N",
        help="診断モード: 直近 N 件のチャットの構造を表示して終了（推論もファイル更新も行わない）",
    )
    args = ap.parse_args()

    _VERBOSE = args.verbose

    memory_dir = Path(args.memory_dir)
    if not memory_dir.is_dir():
        log(f"エラー: memory ディレクトリが見つからない: {memory_dir}")
        return 2

    llama = Llama(args.api_base, args.api_key, args.model, args.timeout, not args.think)
    if not llama.alive():
        log(f"llama-server ({args.api_base}) に到達できないので何もせず終了")
        return 0

    state = load_state(memory_dir)
    now = dt.datetime.now().timestamp()
    since = now - args.window_hours * 3600
    # 前回の実行位置がそれより新しければそちらを使う（重複抽出の防止）
    last_seen = float(state.get("last_chat_updated_at") or 0)
    if last_seen > since:
        since = last_seen
    log(f"対象: {dt.datetime.fromtimestamp(since):%Y-%m-%d %H:%M} 以降に更新されたチャット")

    try:
        chats = fetch_chats(args.container, args.db_path, since)
    except Exception as e:  # noqa: BLE001
        log("エラー:", e)
        return 1

    if not args.include_archived:
        chats = [c for c in chats if not c.get("archived")]

    if args.probe:
        return probe(chats, args.probe)

    if not chats:
        log("新しいチャットは無し")
    else:
        log(f"{len(chats)} 件のチャットを処理する")

    memory_ctx = "\n\n".join(
        f"### {name}\n{read_text(memory_dir / name) or '(空)'}"
        for name in (PROFILE_FILE, PROJECTS_FILE, INBOX_FILE)
    )

    facts: List[str] = []
    for i, batch in enumerate(batch_chats(chats, MAX_CHARS_PER_REQUEST), start=1):
        user = EXTRACT_USER_TMPL.format(memory=memory_ctx, conversations="\n\n".join(batch))
        log(f"抽出 {i} 回目 ({len(user)} 文字) …")
        try:
            out = llama.chat(EXTRACT_SYSTEM, user, max_tokens=args.max_tokens)
        except (urllib.error.URLError, OSError, KeyError, ValueError) as e:  # noqa: BLE001
            log("  抽出に失敗:", e)
            continue
        if out.strip().upper().startswith("NONE"):
            log("  新しい事実なし")
            continue
        for line in out.splitlines():
            line = line.strip()
            if line.startswith(("-", "*", "・")):
                line = "- " + line.lstrip("-*・ ").strip()
                if line != "-" and line not in facts:
                    facts.append(line)

    if facts:
        log(f"新しい事実 {len(facts)} 件")
        block = (
            f"\n## {dt.datetime.now():%Y-%m-%d} (自動抽出)\n\n" + "\n".join(facts) + "\n"
        )
        if args.dry_run:
            log("dry-run: inbox.md への追記内容")
            print(block)
        else:
            backup_memory(memory_dir)
            inbox_path = memory_dir / INBOX_FILE
            write_text(inbox_path, read_text(inbox_path).rstrip("\n") + "\n" + block)
            log(f"{INBOX_FILE} に追記した")
    else:
        log("追記する事実は無かった")

    if chats:
        state["last_chat_updated_at"] = max(c["updated_at"] for c in chats)
    state["last_run_at"] = now
    state["last_run_facts"] = len(facts)

    # ---- 統合するか判定
    do_consolidate = args.consolidate
    if not do_consolidate and not args.no_consolidate:
        last_cons = float(state.get("last_consolidated_at") or 0)
        if now - last_cons >= args.consolidate_days * 86400:
            do_consolidate = True
    if args.no_consolidate:
        do_consolidate = False

    if do_consolidate:
        if consolidate(llama, memory_dir, args.dry_run, args.force):
            state["last_consolidated_at"] = now

    if not args.dry_run:
        prune_backups(memory_dir, args.keep_backups)
        save_state(memory_dir, state)

    log("完了")
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        sys.exit(130)
