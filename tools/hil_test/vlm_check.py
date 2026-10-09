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

既知かつ現時点で意図的に未対応の表示、またはこのカメラ撮影パイプライン側の既知の制約（これらだけを\
理由にpass:falseにしないこと。気になればmismatchesに書いてよいが、他に不一致がなければpassはtrueの\
ままにする）:
- 右上の「x1000rpm」ラベルとLVGLパフォーマンスモニタ(FPS/CPU表示)の重なり
- 画面全体を塗りつぶす大面積の警告背景色(セル背景反転警告など)は、現在のカメラの自動露出/\
ホワイトバランスにより実機の色より薄く・暖色方向にシフトして写ることが確認済み(例: 実機で赤
(#B31822)の背景が写真では薄いオレンジやクリーム色に、実機で黄色(#FFD245)の背景が写真では
ほぼ白に写ることがある)。そのセルの背景が「点灯(不透明化)しているか・していないか」と、
大まかな色味が寒色系(青緑など、明らかに意図しない色)ではなく暖色系(赤・オレンジ・黄・\
クリームのいずれか)であることだけを確認し、赤と黄の正確な区別が写真から付かない場合でも\
そのことだけを理由にpass:falseにしないこと。
- 同じ警告背景セル内でも、大きいタイトル文字や数値(例: 「OIL TEMP」「140」)は色がはっきり\
写るが、単位ラベルのような小さいフォント(17px程度)は実機で同じ色指定でも写真では解像度・\
ボケの影響で背景に溶け込んで薄く写ることが確認済み。タイトルや数値の文字色が期待どおりなら、\
単位ラベルだけが薄く/判読しにくいことを理由にpass:falseにしないこと(気になればmismatchesに\
書いてよい)。

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
