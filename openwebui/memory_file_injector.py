"""
title: Memory File Injector (profile/projects/inbox)
author: tomoya723
version: 0.2.0
description: memory/*.md を system prompt 先頭に固定注入し、「記憶して」等のキーワードで会話から事実を抽出して inbox.md に追記する Filter。web_search 有効時と内部補助タスクでは注入量を切り替えられる。
"""

import os
import re
import json
import asyncio
from datetime import datetime
from typing import Optional, Callable, Awaitable, Any

from pydantic import BaseModel, Field


THINK_RE = re.compile(r"<think>.*?</think>", re.DOTALL)


class Filter:
    class Valves(BaseModel):
        priority: int = Field(
            default=0,
            description="Filter の実行順（小さいほど先）。force_web_search より先に動かしたいなら小さく",
        )
        memory_dir: str = Field(
            default="/app/memory",
            description="コンテナ内の記憶フォルダ（母艦の ~/Git/qwen38-1080ti/memory を bind mount）",
        )
        files: str = Field(
            default="profile.md,projects.md,inbox.md",
            description="注入するファイル（この順で連結）",
        )
        web_search_files: str = Field(
            default="",
            description=(
                "ウェブ検索が有効なターンに注入するファイル。空なら記憶を注入しない。"
                "検索ターンは取得ページ本文でプロンプトが膨らみ、記憶はほぼ使われないため既定は空。"
                "ペルソナ（モデル設定側の system prompt）はここをどう設定しても常に残る。"
            ),
        )
        skip_on_internal_task: bool = Field(
            default=True,
            description=(
                "タイトル生成・タグ生成・検索クエリ生成などの内部補助タスクでは注入しない。"
                "現行の Open WebUI では補助タスクに inlet は来ないが将来の保険。"
            ),
        )
        max_chars_per_file: int = Field(
            default=6000,
            description="1ファイルあたりの注入上限（超過分は末尾を切る。目安: 日本語 1文字≈1トークン強）",
        )
        max_chars_total: int = Field(
            default=12000, description="注入合計の上限"
        )
        append_datetime: bool = Field(
            default=True,
            description="system prompt の末尾に現在日時を付ける（モデル設定側の {{CURRENT_DATETIME}} は外し、記憶ブロックを先頭に置くことで llama-server の prefix cache を効かせる）",
        )
        datetime_format: str = Field(
            default="%Y-%m-%d %H:00 (%a)",
            description=(
                "付加する日時の書式。分・秒まで入れると 1 分ごとに system prompt が変わり、"
                "llama-server の prefix cache がそこから先で無効になる。時単位か日単位を推奨。"
            ),
        )
        # ---- 書き込み（抽出） ----
        trigger_keywords: str = Field(
            default="記憶して,覚えておいて,覚えて,メモして,remember this,save to memory",
            description="直近のユーザー発言にこれらが含まれたときだけ抽出推論を走らせる（カンマ区切り）",
        )
        inbox_file: str = Field(default="inbox.md", description="抽出結果の追記先")
        context_turns: int = Field(
            default=8, description="抽出に渡す直近メッセージ数（user/assistant 合計）"
        )
        max_chars_per_message: int = Field(
            default=2500, description="抽出に渡す各メッセージの上限文字数"
        )
        api_base: str = Field(
            default="http://100.87.81.4:8080",
            description="抽出推論を投げる OpenAI 互換エンドポイント（llama-server）",
        )
        api_key: str = Field(default="", description="llama-server の API キー")
        model: str = Field(
            default="qwen3.8-27b",
            description="抽出に使うモデル名（llama-server は基本無視するが送る）",
        )
        max_tokens: int = Field(default=600, description="抽出結果の最大トークン")
        disable_thinking: bool = Field(
            default=True,
            description="chat_template_kwargs.enable_thinking=false を送る（Qwen3 系の thinking を切って速く）",
        )
        request_timeout_s: int = Field(default=120, description="抽出リクエストのタイムアウト")
        debug: bool = Field(default=False, description="stdout にログを出す")

    def __init__(self):
        self.valves = self.Valves()

    # ------------------------------------------------------------------ utils
    def _log(self, *a):
        if self.valves.debug:
            print("[memory_file_filter]", *a, flush=True)

    @staticmethod
    def _content_text(content: Any) -> str:
        """OpenAI 形式の content（str または [{type:text,...},{type:image_url,...}]）から文字列だけ取り出す"""
        if isinstance(content, str):
            return content
        if isinstance(content, list):
            parts = []
            for p in content:
                if isinstance(p, dict) and p.get("type") == "text":
                    parts.append(p.get("text", ""))
            return "\n".join(parts)
        return ""

    def _read_memory_block(self, files_csv: Optional[str] = None) -> str:
        source = self.valves.files if files_csv is None else files_csv
        names = [n.strip() for n in source.split(",") if n.strip()]
        chunks = []
        total = 0
        for name in names:
            path = os.path.join(self.valves.memory_dir, name)
            try:
                with open(path, "r", encoding="utf-8") as f:
                    text = f.read().strip()
            except FileNotFoundError:
                self._log("missing", path)
                continue
            except Exception as e:
                self._log("read error", path, e)
                continue
            if not text:
                continue
            if len(text) > self.valves.max_chars_per_file:
                text = text[: self.valves.max_chars_per_file] + "\n…(truncated)"
            remaining = self.valves.max_chars_total - total
            if remaining <= 0:
                break
            if len(text) > remaining:
                text = text[:remaining] + "\n…(truncated)"
            chunks.append(f"### {name}\n{text}")
            total += len(text)
        if not chunks:
            return ""
        return (
            "<memory>\n"
            "以下はユーザーに関する長期記憶（過去のセッションで確定した事実）。"
            "回答の前提として使うこと。ここに無いことは知っているふりをしない。\n\n"
            + "\n\n".join(chunks)
            + "\n</memory>"
        )

    @staticmethod
    def _is_internal_task(md: dict) -> bool:
        """タイトル生成 / タグ生成 / 検索クエリ生成などの内部補助タスクか。

        Open WebUI は routers/tasks.py でこれらの payload に
        metadata["task"] / metadata["task_body"] を付ける。
        """
        return bool(md.get("task")) or "task_body" in md

    def _web_search_enabled(self, body: dict, md: dict) -> bool:
        """このターンでウェブ検索が有効か。

        inlet の時点では body["features"] がまだ残っている（middleware が
        features を pop するのは inlet の後）。__metadata__["features"] は
        main.py が inlet 前に取ったスナップショットなのでフォールバックに使う。
        """
        for src in (body.get("features"), md.get("features")):
            if isinstance(src, dict) and src.get("web_search"):
                return True
        return False

    # ------------------------------------------------------------------ inlet
    async def inlet(
        self,
        body: dict,
        __user__: Optional[dict] = None,
        __metadata__: Optional[dict] = None,
        __event_emitter__: Optional[Callable[[Any], Awaitable[None]]] = None,
    ) -> dict:
        md = __metadata__ if isinstance(__metadata__, dict) else {}

        # 1) 内部補助タスクは素通し
        if self.valves.skip_on_internal_task and self._is_internal_task(md):
            self._log("skip: internal task", repr(md.get("task")))
            return body

        # 2) 検索ターンは注入対象を切り替える（既定は注入しない）
        if self._web_search_enabled(body, md):
            files_csv = self.valves.web_search_files
            if not files_csv.strip():
                self._log("skip: web_search turn")
                return body
            self._log("web_search turn -> files:", files_csv)
        else:
            files_csv = self.valves.files

        block = self._read_memory_block(files_csv)
        if not block:
            return body

        messages = body.get("messages", [])
        suffix = ""
        if self.valves.append_datetime:
            suffix = "\n\n現在日時: " + datetime.now().strftime(self.valves.datetime_format)

        sys_idx = next(
            (i for i, m in enumerate(messages) if m.get("role") == "system"), None
        )
        if sys_idx is None:
            messages.insert(0, {"role": "system", "content": block + suffix})
        else:
            existing = self._content_text(messages[sys_idx].get("content", ""))
            # 記憶ブロック → 既存 system prompt → 日時 の順（先頭を安定させて prefix cache を効かせる）
            messages[sys_idx]["content"] = (
                block + ("\n\n" + existing if existing else "") + suffix
            )
        body["messages"] = messages
        self._log("injected", len(block), "chars")
        return body

    # ----------------------------------------------------------------- outlet
    def _triggered(self, messages: list) -> Optional[str]:
        last_user = next(
            (m for m in reversed(messages) if m.get("role") == "user"), None
        )
        if not last_user:
            return None
        text = self._content_text(last_user.get("content", ""))
        low = text.lower()
        for kw in [k.strip() for k in self.valves.trigger_keywords.split(",")]:
            if kw and kw.lower() in low:
                return kw
        return None

    def _transcript(self, messages: list) -> str:
        convo = [m for m in messages if m.get("role") in ("user", "assistant")]
        convo = convo[-self.valves.context_turns :]
        lines = []
        for m in convo:
            t = self._content_text(m.get("content", ""))
            t = THINK_RE.sub("", t).strip()
            if len(t) > self.valves.max_chars_per_message:
                t = t[: self.valves.max_chars_per_message] + "…"
            lines.append(f"[{m['role']}]\n{t}")
        return "\n\n".join(lines)

    def _post_json(self, url: str, payload: dict, headers: dict) -> dict:
        """標準ライブラリだけで POST（Open WebUI 側の依存に左右されない）"""
        import urllib.request
        import urllib.error

        req = urllib.request.Request(
            url, data=json.dumps(payload).encode("utf-8"), headers=headers, method="POST"
        )
        try:
            with urllib.request.urlopen(req, timeout=self.valves.request_timeout_s) as r:
                return json.loads(r.read().decode("utf-8"))
        except urllib.error.HTTPError as e:
            raise RuntimeError(f"HTTP {e.code}: {e.read()[:300]!r}")

    async def _extract(self, transcript: str) -> str:
        system = (
            "あなたは会話ログから『将来のセッションでも役立つ、ユーザーに関する事実』だけを抽出する係です。\n"
            "出力は Markdown の箇条書きのみ。各行は '- ' で始め、1行1事実、簡潔に、日本語で。\n"
            "含めるもの: ユーザーが述べた決定・方針・環境/構成の事実・進行中の作業の状態・好み。\n"
            "含めないもの: あなた（アシスタント）の提案や一般知識、推測、その場限りの情報、挨拶。\n"
            "ユーザーが『これを記憶して』と明示した内容は必ず含める。該当がなければ '- (なし)' とだけ出力。\n"
            "前置き・後書き・見出しは書かない。"
        )
        payload = {
            "model": self.valves.model,
            "messages": [
                {"role": "system", "content": system},
                {"role": "user", "content": "会話ログ:\n\n" + transcript},
            ],
            "max_tokens": self.valves.max_tokens,
            "temperature": 0.2,
            "stream": False,
        }
        if self.valves.disable_thinking:
            payload["chat_template_kwargs"] = {"enable_thinking": False}
        headers = {"Content-Type": "application/json"}
        if self.valves.api_key:
            headers["Authorization"] = f"Bearer {self.valves.api_key}"
        url = self.valves.api_base.rstrip("/") + "/v1/chat/completions"
        # イベントループを塞がないようスレッドで実行
        data = await asyncio.to_thread(self._post_json, url, payload, headers)
        content = data["choices"][0]["message"].get("content", "") or ""
        content = THINK_RE.sub("", content).strip()
        # 箇条書き行だけ残す
        bullets = [
            ln.strip() for ln in content.splitlines() if ln.strip().startswith("- ")
        ]
        return "\n".join(bullets)

    def _append_inbox(self, bullets: str, chat_title: str) -> str:
        path = os.path.join(self.valves.memory_dir, self.valves.inbox_file)
        stamp = datetime.now().strftime("%Y-%m-%d %H:%M")
        header = f"\n## {stamp}" + (f" — {chat_title}" if chat_title else "") + "\n"
        os.makedirs(self.valves.memory_dir, exist_ok=True)
        with open(path, "a", encoding="utf-8") as f:
            f.write(header + bullets + "\n")
        return path

    async def outlet(
        self,
        body: dict,
        __user__: Optional[dict] = None,
        __event_emitter__: Optional[Callable[[Any], Awaitable[None]]] = None,
    ) -> dict:
        messages = body.get("messages", [])
        kw = self._triggered(messages)
        if not kw:
            return body

        async def status(desc: str, done: bool = False):
            if __event_emitter__:
                await __event_emitter__(
                    {"type": "status", "data": {"description": desc, "done": done}}
                )

        await status(f"記憶を抽出中…（トリガー: {kw}）")
        try:
            transcript = self._transcript(messages)
            bullets = await self._extract(transcript)
            if not bullets or bullets.strip() == "- (なし)":
                await status("記憶: 追記する事実なし", True)
                return body
            title = ""
            chat = body.get("chat") or {}
            if isinstance(chat, dict):
                title = chat.get("title", "") or ""
            path = self._append_inbox(bullets, title)
            n = len(bullets.splitlines())
            await status(f"記憶: {n} 件を inbox.md に追記（次のターンから反映）", True)
            self._log("appended", n, "bullets ->", path)
        except Exception as e:
            self._log("extract failed", repr(e))
            await status(f"記憶の抽出に失敗: {e}", True)
        return body
