#!/usr/bin/env python3
"""スマホ単体で実機LCDを1フレーム撮る → PNG（任意で 800x256 に矯正）。

取得ソース (--source):
  ipwebcam : IP Webcam アプリの HTTP JPEG (既定 http://127.0.0.1:8080/shot.jpg)
             露出/WB/フォーカスをアプリ側でロックできるので差分検証向き（推奨）
  termux   : termux-camera-photo（追加アプリ不要・粗い・1枚あたり約1〜2秒）

矯正 (--homography):
  calibrate.py が作る homography.json を渡すと、パネル部分を正準 WxH へ透視補正し、
  毎回ピクセル比較可能なフレームにする。未指定なら生フレームをそのまま保存。

依存: 生保存のみなら標準ライブラリだけで動く。矯正には Pillow + numpy が必要。
"""
import argparse
import io
import os
import subprocess
import sys
import tempfile
import urllib.request


def grab_ipwebcam(url, timeout=5):
    with urllib.request.urlopen(url, timeout=timeout) as r:
        return r.read()


def grab_termux(cam="0"):
    path = tempfile.mktemp(suffix=".jpg")
    try:
        subprocess.run(["termux-camera-photo", "-c", cam, path], check=True)
        with open(path, "rb") as f:
            return f.read()
    finally:
        if os.path.exists(path):
            os.unlink(path)


def _find_coeffs(dst, src):
    """出力座標(dst) -> 入力座標(src) へのPillow PERSPECTIVE係数(8)を解く。"""
    import numpy

    m = []
    for (dx, dy), (sx, sy) in zip(dst, src):
        m.append([dx, dy, 1, 0, 0, 0, -sx * dx, -sx * dy])
        m.append([0, 0, 0, dx, dy, 1, -sy * dx, -sy * dy])
    a = numpy.array(m, dtype=float)
    b = numpy.array(src, dtype=float).reshape(8)
    return numpy.linalg.solve(a, b)


def rectify(jpeg_bytes, homography_path):
    import json

    from PIL import Image

    cfg = json.load(open(homography_path))
    # TL, TR, BR, BL（キャプチャ画像上のパネル四隅・ピクセル）
    src = cfg["panel_corners_in_capture"]
    w, h = int(cfg.get("out_w", 800)), int(cfg.get("out_h", 256))
    dst = [(0, 0), (w, 0), (w, h), (0, h)]
    coeffs = _find_coeffs(dst, src)
    img = Image.open(io.BytesIO(jpeg_bytes)).convert("RGB")
    return img.transform((w, h), Image.PERSPECTIVE, coeffs, Image.BICUBIC)


def main():
    ap = argparse.ArgumentParser(description="Capture one frame from the phone camera.")
    ap.add_argument("--source", choices=["ipwebcam", "termux"], default="ipwebcam")
    ap.add_argument("--url", default="http://127.0.0.1:8080/shot.jpg",
                    help="IP Webcam の静止画エンドポイント")
    ap.add_argument("--cam", default="0", help="termux-camera-photo のカメラID (0=背面)")
    ap.add_argument("--homography", help="homography.json（矯正する場合）")
    ap.add_argument("-o", "--out", default="frame.png")
    a = ap.parse_args()

    data = grab_ipwebcam(a.url) if a.source == "ipwebcam" else grab_termux(a.cam)

    if a.homography:
        rectify(data, a.homography).save(a.out)
        print(a.out)
        return

    # 生保存: Pillow があれば PNG 化、無ければ JPEG のまま書き出す
    try:
        from PIL import Image
        Image.open(io.BytesIO(data)).convert("RGB").save(a.out)
        print(a.out)
    except ImportError:
        out = a.out
        if out.lower().endswith(".png"):
            out = out[:-4] + ".jpg"
        with open(out, "wb") as f:
            f.write(data)
        print(out)


if __name__ == "__main__":
    main()
