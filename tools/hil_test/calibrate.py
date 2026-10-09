"""One-shot helper to pick a camera device index and find the LCD crop box.

Usage:
    python calibrate.py --design eez002 [--device-index 0]

Saves a full, uncropped frame to test-results/<design>/calibration.png so you
can open it, read off the LCD's pixel rectangle, and fill in
Firmware/eez/<design>/hil_test/camera.yaml's `crop: [x, y, w, h]`.
"""
from __future__ import annotations

import argparse
import os

import cv2

import camera as camera_mod
import scenario


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--design", required=True)
    ap.add_argument("--device-index", type=int, default=None)
    ap.add_argument("--rotate", type=int, default=None, choices=[0, 90, 180, 270],
                     help="override camera.yaml's rotate for this preview")
    args = ap.parse_args()

    cfg = scenario.load_camera_config(args.design)
    if args.device_index is not None:
        cfg.device_index = args.device_index
    if args.rotate is not None:
        cfg.rotate = args.rotate

    cap = cv2.VideoCapture(cfg.device_index, cv2.CAP_DSHOW)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, cfg.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, cfg.height)
    if not cap.isOpened():
        raise SystemExit(f"Could not open camera index {cfg.device_index}. "
                          f"Try --device-index 1, 2, ... to find the USB camera.")

    for _ in range(cfg.warmup_frames):
        cap.read()
    ok, frame = cap.read()
    cap.release()
    if not ok:
        raise SystemExit("Camera read failed")
    frame = camera_mod.apply_rotation(frame, cfg.rotate)

    out_dir = os.path.join(scenario.REPO_ROOT, "test-results", args.design)
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "calibration.png")
    cv2.imwrite(out_path, frame)
    print(f"Saved {out_path} ({frame.shape[1]}x{frame.shape[0]}, rotate={cfg.rotate}).")
    print("Open it, read off the LCD rectangle (coordinates are in this rotated "
          "image), and set crop: [x, y, w, h] "
          f"in Firmware/eez/{args.design}/hil_test/camera.yaml.")
    if args.rotate is not None and args.rotate != scenario.load_camera_config(args.design).rotate:
        print(f"NOTE: --rotate {args.rotate} was used for this preview only; "
              f"set rotate: {args.rotate} in camera.yaml to make it permanent.")


if __name__ == "__main__":
    main()
