"""
memory_file_filter.py に貼り付けるガード片。

目的:
  1. タイトル生成 / タグ生成 / 検索クエリ生成などの内部補助タスクに
     記憶が注入されるのを防ぐ (保険。詳細は README.md の「症状2」参照)。
  2. 誤って body["metadata"] を見に行かないようにする
     (inlet の時点では body に metadata キーは存在しない)。

使い方: memory_file_filter.py の Filter クラスに _skip_injection() を足し、
        inlet の先頭で下記 3 行を入れる。

        def inlet(self, body: dict, __metadata__: Optional[dict] = None) -> dict:
            if self._skip_injection(__metadata__):
                return body
            ...  # 既存の記憶注入処理
"""

from typing import Optional


def _skip_injection(self, __metadata__: Optional[dict] = None) -> bool:
    """内部補助タスクのリクエストなら True (= 記憶を注入せず素通しさせる)。"""
    md = __metadata__ if isinstance(__metadata__, dict) else {}

    # backend/open_webui/routers/tasks.py が付ける印。
    # 値は constants.py の TASKS: title_generation / tags_generation /
    # query_generation / follow_up_generation / emoji_generation /
    # image_prompt_generation / autocomplete_generation /
    # function_calling / moa_response_generation
    if md.get("task") or "task_body" in md:
        if getattr(self.valves, "debug", False):
            print(
                f"[memory_file_filter] skip: internal task={md.get('task')!r}",
                flush=True,
            )
        return True

    return False
