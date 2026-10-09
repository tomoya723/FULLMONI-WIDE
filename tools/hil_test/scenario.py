"""Scenario config loading for the HIL (CAN -> camera -> VLM) test framework.

See tools/hil_test/README.md for the YAML schema. Each design keeps its own
scenarios.yaml / camera.yaml under Firmware/eez/<design>/hil_test/.
"""
from __future__ import annotations

import copy
import os
from dataclasses import dataclass, field
from typing import Any

import yaml

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


@dataclass
class CanConfig:
    dbc_path: str
    interface: str = "pcan"
    channel: str = "PCAN_USBBUS1"
    bitrate: int = 500000
    frame_period_s: float = 0.02
    settle_s: float = 2.0


@dataclass
class CameraConfig:
    device_index: int = 0
    width: int = 1920
    height: int = 1080
    warmup_frames: int = 10
    rotate: int = 0  # 0/90/180/270, applied before crop (camera mounted upside down etc.)
    crop: tuple[int, int, int, int] | None = None  # (x, y, w, h) in the *rotated* frame, None = full frame


@dataclass
class Scenario:
    id: str
    description: str
    signals: dict[str, float]
    expect: str


def _design_dir(design: str) -> str:
    return os.path.join(REPO_ROOT, "Firmware", "eez", design, "hil_test")


def load_can_config(design: str) -> CanConfig:
    path = os.path.join(_design_dir(design), "scenarios.yaml")
    with open(path, encoding="utf-8") as f:
        doc = yaml.safe_load(f)
    can_doc = doc.get("can", {})
    dbc_path = os.path.normpath(os.path.join(_design_dir(design), can_doc["dbc"]))
    return CanConfig(
        dbc_path=dbc_path,
        interface=can_doc.get("interface", "pcan"),
        channel=can_doc.get("channel", "PCAN_USBBUS1"),
        bitrate=int(can_doc.get("bitrate", 500000)),
        frame_period_s=float(can_doc.get("frame_period_s", 0.02)),
        settle_s=float(can_doc.get("settle_s", 2.0)),
    )


def load_camera_config(design: str) -> CameraConfig:
    path = os.path.join(_design_dir(design), "camera.yaml")
    with open(path, encoding="utf-8") as f:
        doc = yaml.safe_load(f) or {}
    crop = doc.get("crop")
    return CameraConfig(
        device_index=int(doc.get("device_index", 0)),
        width=int(doc.get("width", 1920)),
        height=int(doc.get("height", 1080)),
        warmup_frames=int(doc.get("warmup_frames", 10)),
        rotate=int(doc.get("rotate", 0)),
        crop=tuple(crop) if crop else None,
    )


def load_scenarios(design: str, only: list[str] | None = None) -> list[Scenario]:
    path = os.path.join(_design_dir(design), "scenarios.yaml")
    with open(path, encoding="utf-8") as f:
        doc = yaml.safe_load(f)

    raw_scenarios = {s["id"]: s for s in doc["scenarios"]}
    resolved: list[Scenario] = []
    for sid, raw in raw_scenarios.items():
        if only and sid not in only:
            continue
        signals: dict[str, float] = {}
        if "base" in raw:
            base = raw_scenarios[raw["base"]]
            signals.update(copy.deepcopy(base.get("signals", {})))
        signals.update(raw.get("signals", {}))
        resolved.append(Scenario(
            id=sid,
            description=raw.get("description", ""),
            signals=signals,
            expect=raw.get("expect", "").strip(),
        ))
    return resolved
