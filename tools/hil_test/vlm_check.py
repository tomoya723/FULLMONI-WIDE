"""VLM-based pass/fail check: does the captured LCD photo match what the
scenario says it should show?

Shells out to the Claude Code CLI (`claude -p ...`) already installed/logged
in on this machine, instead of calling the Anthropic API directly -- no
separate ANTHROPIC_API_KEY needed. Claude Code reads the image itself via its
Read tool (given the file path), so we only pass the path + prompt text.
"""
from __future__ import annotations

import json
import re
import subprocess
from dataclasses import dataclass

PROMPT_TEMPLATE = """\
あなたは自動車用デジタルメータークラスタの検査員です。 {image_path} を読んでください。\
物理LCD(800x256のRGB565パネル)をUSBカメラで撮影した写真です。

このシナリオで送信したCAN値から期待される表示内容:
{expect}

写真を見て、期待内容と実際の表示が一致しているか判定してください。警告灯の点灯/消灯、数値、文字の\
誤字脱字、表示位置のずれ・重なりも確認してください。写真の歪み・反射・若干の色味の違いはカメラ起因\
なので無視してください。

必ず次のJSON形式のみで回答してください(説明文やコードブロックのバッククォートは不要):
{{"pass": true または false, "observed": "実際に見えた内容の要約", "mismatches": ["不一致点を箇条書き。一致していれば空配列"]}}
"""


@dataclass
class VlmResult:
    passed: bool
    observed: str
    mismatches: list[str]
    raw: str


def _extract_json(text: str) -> dict:
    match = re.search(r"\{.*\}", text, re.DOTALL)
    if not match:
        raise ValueError(f"No JSON object found in response: {text!r}")
    return json.loads(match.group(0))


def check(image_path: str, expect: str, model: str | None = None, timeout_s: float = 120.0) -> VlmResult:
    prompt = PROMPT_TEMPLATE.format(image_path=image_path, expect=expect)

    cmd = ["claude", "-p", prompt, "--output-format", "json", "--permission-mode", "bypassPermissions"]
    if model:
        cmd += ["--model", model]

    proc = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", timeout=timeout_s)
    if proc.returncode != 0:
        raise RuntimeError(f"claude CLI exited {proc.returncode}: {proc.stderr}")

    envelope = _extract_json(proc.stdout)
    if envelope.get("is_error"):
        raise RuntimeError(f"claude CLI reported an error: {envelope.get('result')}")

    parsed = _extract_json(envelope["result"])
    return VlmResult(
        passed=bool(parsed.get("pass", False)),
        observed=str(parsed.get("observed", "")),
        mismatches=list(parsed.get("mismatches", [])),
        raw=envelope["result"],
    )
