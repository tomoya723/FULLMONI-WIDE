#!/usr/bin/env python3
"""スマホ単体で完結する静的観測エージェント: 撮る → Claude が判定する。

capture.py で1フレーム取得し、チェックリスト（と任意のゴールデン画像）と
突き合わせて Claude に PASS/FAIL を出させる。実機LCDの静的検証ループの最小形。

準備:
  pip install anthropic pillow numpy
  export ANTHROPIC_API_KEY=...
  # 判定モデルは OBSERVE_MODEL で上書き可（既定 claude-sonnet-5）

使い方:
  python observe_agent.py --checklist checklist.md \
      [--golden golden.png] [--homography homography.json] [--source ipwebcam|termux]
"""
import argparse
import base64
import os
import subprocess
import sys

MODEL = os.environ.get("OBSERVE_MODEL", "claude-sonnet-5")
HERE = os.path.dirname(os.path.abspath(__file__))


def img_block(path):
    mt = "image/png" if path.lower().endswith(".png") else "image/jpeg"
    with open(path, "rb") as f:
        data = base64.standard_b64encode(f.read()).decode()
    return {"type": "image", "source": {"type": "base64", "media_type": mt, "data": data}}


def main():
    ap = argparse.ArgumentParser(description="Standalone on-device LCD verifier.")
    ap.add_argument("--checklist", required=True, help="検証項目 (Markdown/テキスト)")
    ap.add_argument("--golden", help="ゴールデン参照画像（任意）")
    ap.add_argument("--homography", help="homography.json（矯正する場合）")
    ap.add_argument("--source", default="ipwebcam", choices=["ipwebcam", "termux"])
    ap.add_argument("--frame", default="frame.png", help="キャプチャ保存先")
    a = ap.parse_args()

    # 1) capture
    cmd = [sys.executable, os.path.join(HERE, "capture.py"),
           "--source", a.source, "-o", a.frame]
    if a.homography:
        cmd += ["--homography", a.homography]
    subprocess.run(cmd, check=True)

    # 2) prompt
    import anthropic

    with open(a.checklist) as f:
        checklist = f.read()

    content = [{"type": "text", "text":
                "あなたは車載デジタルメーターの物理LCD(800x256)を検証しています。\n"
                "CAPTURED フレームをチェックリスト"
                + ("とGOLDEN参照" if a.golden else "")
                + "と厳密に突き合わせ、判定してください。\n\n"
                "CHECKLIST:\n" + checklist +
                '\n\n出力はJSONのみ: {"pass": bool, "issues": [string], "notes": string}'}]
    if a.golden:
        content += [{"type": "text", "text": "GOLDEN 参照:"}, img_block(a.golden)]
    content += [{"type": "text", "text": "CAPTURED フレーム:"}, img_block(a.frame)]

    client = anthropic.Anthropic()
    msg = client.messages.create(
        model=MODEL, max_tokens=1024,
        messages=[{"role": "user", "content": content}])
    print(msg.content[0].text)


if __name__ == "__main__":
    main()
