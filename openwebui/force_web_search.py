"""
title: Force Web Search (API only)
author: tomoya723
version: 2.1.0
required_open_webui_version: 0.5.0
description: LocalMind などの API 直叩きのときだけ web_search を有効化する。ブラウザ UI の検索トグルには一切干渉しない。
"""

from typing import Optional

from pydantic import BaseModel, Field


class Filter:
    class Valves(BaseModel):
        priority: int = Field(
            default=0,
            description="フィルタ実行順。小さいほど先に走る。",
        )
        enabled: bool = Field(
            default=True,
            description="このフィルタ全体の有効/無効。",
        )
        force_override: bool = Field(
            default=False,
            description=(
                "API 側が features.web_search を明示指定していても True で上書きする。"
                "False なら明示指定を尊重し、未指定のときだけ True を入れる。"
            ),
        )
        debug: bool = Field(
            default=True,
            description="inlet に渡る metadata と判定結果を stdout に出す (docker logs open-webui で確認)。",
        )

    def __init__(self):
        self.valves = self.Valves()

    def _log(self, decision: str, md: dict, features_in) -> None:
        if not self.valves.debug:
            return
        print(
            "[force_web_search] "
            f"decision={decision} "
            f"task={md.get('task')!r} "
            f"session_id={md.get('session_id')!r} "
            f"chat_id={md.get('chat_id')!r} "
            f"message_id={md.get('message_id')!r} "
            f"direct={md.get('direct')!r} "
            f"md_keys={sorted(md.keys())} "
            f"features_in={features_in!r}",
            flush=True,
        )

    def inlet(self, body: dict, __metadata__: Optional[dict] = None) -> dict:
        if not self.valves.enabled:
            return body

        # ------------------------------------------------------------------
        # 重要: inlet の時点では body["metadata"] はまだ存在しない。
        # Open WebUI は utils/middleware.py の process_chat_payload() 内で
        #   1. filter inlet を実行
        #   2. そのあと form_data["metadata"] = metadata を代入
        # という順序になっている (v0.6.x 〜 v0.8.x で共通)。
        # したがって body.get("metadata", {}) は常に {} であり、
        # 「metadata に session_id が無い」という条件はブラウザからの
        # 通常チャットでも必ず真になってしまう。これが誤発火の原因。
        # 正しい取得口は inlet の引数 __metadata__ 。
        # ------------------------------------------------------------------
        features_in = body.get("features")
        if isinstance(features_in, dict):
            features_in = dict(features_in)

        md = __metadata__ if isinstance(__metadata__, dict) else {}
        if not md and isinstance(body.get("metadata"), dict):
            md = body["metadata"]

        # 1) タイトル生成 / タグ生成 / 検索クエリ生成などの内部補助タスクは素通し。
        #    これらは metadata に task / task_body を持つ
        #    (backend/open_webui/routers/tasks.py, constants.py の TASKS)。
        if md.get("task") or "task_body" in md:
            self._log("skip:internal_task", md, features_in)
            return body

        # 2) ブラウザ UI からの通常チャットかどうかの判定。
        #
        #    v2.0.0 は session_id だけで見ていたが、これは socket.io 由来の値なので
        #    PWA でソケットが落ちている状態（画面が固まり通知経由で応答が届く状況）では
        #    空になる。その結果 UI からのチャットを「API 直叩き」と誤判定し、
        #    検索トグルを OFF にしていても web_search を注入してしまっていた。
        #
        #    chat_id は main.py chat_completion() が form_data から pop する値で、
        #    HTTP リクエストボディに載る。ソケットの状態に左右されない。
        #    ブラウザは必ず実 ID を送り、LocalMind など OpenAI 互換クライアントは
        #    送らない（空になる。だから LocalMind の会話は DB に永続化されない）。
        #    どちらか一方でも立っていれば UI 由来とみなす。
        ui_markers = [k for k in ("chat_id", "session_id") if md.get(k)]
        if ui_markers:
            self._log("skip:ui_session(" + ",".join(ui_markers) + ")", md, features_in)
            return body

        # 3) chat_id も session_id も task も無い = API 直叩き。ここだけ注入する。
        features = body.get("features")
        if not isinstance(features, dict):
            features = {}
            body["features"] = features

        if self.valves.force_override or "web_search" not in features:
            features["web_search"] = True
            self._log("inject:web_search=True", md, features_in)
        else:
            self._log("skip:explicit_features", md, features_in)

        return body
