#!/usr/bin/env python3
"""キャプチャ画像上のパネル四隅から homography.json を作る。

手順:
  1) 基準フレームを撮る:  python capture.py -o ref.png
  2) ref.png を開き、LCD の四隅のピクセル座標を読む
     （順番: 左上 TL, 右上 TR, 右下 BR, 左下 BL）
  3) python calibrate.py --corners "TLx,TLy TRx,TRy BRx,BRy BLx,BLy" [--out-size 800x256]

以降 capture.py --homography homography.json で正準 800x256 に矯正される。
"""
import argparse
import json


def parse_pts(s):
    pts = []
    for tok in s.split():
        x, y = tok.split(",")
        pts.append([float(x), float(y)])
    if len(pts) != 4:
        raise SystemExit("四隅ちょうど4点を指定してください (TL TR BR BL)")
    return pts


def main():
    ap = argparse.ArgumentParser(description="Build homography.json from 4 panel corners.")
    ap.add_argument("--corners", required=True,
                    help='"TLx,TLy TRx,TRy BRx,BRy BLx,BLy"')
    ap.add_argument("--out-size", default="800x256", help="正準サイズ WxH")
    ap.add_argument("-o", "--out", default="homography.json")
    a = ap.parse_args()

    w, h = (int(v) for v in a.out_size.lower().split("x"))
    cfg = {"panel_corners_in_capture": parse_pts(a.corners), "out_w": w, "out_h": h}
    with open(a.out, "w") as f:
        json.dump(cfg, f, indent=2)
    print(a.out)


if __name__ == "__main__":
    main()
