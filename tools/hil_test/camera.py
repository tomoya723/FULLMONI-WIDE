"""USB camera capture for the HIL test framework.

The LCD is an RGB565 panel with its own refresh/backlight PWM, so a single
grab right after a scenario switch can catch a partially-updated or
dim/flickering frame. capture() discards warmup_frames and grabs the next one.
"""
from __future__ import annotations

import os
import time

import cv2

from scenario import CameraConfig

_ROTATE_CODES = {
    90: cv2.ROTATE_90_CLOCKWISE,
    180: cv2.ROTATE_180,
    270: cv2.ROTATE_90_COUNTERCLOCKWISE,
}


def apply_rotation(frame, rotate: int):
    """Rotate a frame by 0/90/180/270 degrees (camera mounted sideways/upside down)."""
    if rotate % 360 == 0:
        return frame
    code = _ROTATE_CODES.get(rotate % 360)
    if code is None:
        raise ValueError(f"rotate must be 0/90/180/270, got {rotate}")
    return cv2.rotate(frame, code)


class Camera:
    def __init__(self, cfg: CameraConfig):
        self.cfg = cfg
        self.cap = cv2.VideoCapture(cfg.device_index, cv2.CAP_DSHOW)
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, cfg.width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, cfg.height)
        if not self.cap.isOpened():
            raise RuntimeError(f"Could not open camera index {cfg.device_index}")

    def capture(self, out_path: str):
        for _ in range(self.cfg.warmup_frames):
            self.cap.read()
            time.sleep(1 / 30)
        ok, frame = self.cap.read()
        if not ok:
            raise RuntimeError("Camera read failed")
        frame = apply_rotation(frame, self.cfg.rotate)
        if self.cfg.crop:
            x, y, w, h = self.cfg.crop
            frame = frame[y:y + h, x:x + w]
        os.makedirs(os.path.dirname(out_path), exist_ok=True)
        cv2.imwrite(out_path, frame)
        return out_path

    def close(self):
        self.cap.release()
