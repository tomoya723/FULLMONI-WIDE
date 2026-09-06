"""
title: Force Web Search (API only)
author: tomoya723
version: 2.0.0
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
            default=False,
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

        # 2) ブラウザ UI からの通常チャットは socket.io の session_id を持つ。
        #    UI の検索トグルに完全に任せて何もしない。
        if md.get("session_id"):
            self._log("skip:ui_session", md, features_in)
            return body

        # 3) session_id も task も無い = API 直叩き。ここだけ web_search を注入する。
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
