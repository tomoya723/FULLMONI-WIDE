"""DBC-driven CAN test-pattern playback over a PCAN USB adapter.

Signal values in scenarios.yaml are physical engineering values (e.g. RPM,
degC, kPa, AFR) exactly as defined in debug_env/fullmoni_MX5_can.dbc. cantools
encodes them to raw CAN bytes; the firmware's own multiplier/divisor/
decimal_shift happens to reproduce the same physical value on screen (verified
against Firmware/src/param_storage.c's default field table), so "set
Lambda1=14.7" in a scenario really does mean "AFR label should read 14.7".
"""
from __future__ import annotations

import threading
import time

import can
import cantools

from scenario import CanConfig


class PatternPlayer:
    def __init__(self, can_cfg: CanConfig):
        self.db = cantools.database.load_file(can_cfg.dbc_path)
        self.bus = can.interface.Bus(
            interface=can_cfg.interface,
            channel=can_cfg.channel,
            bitrate=can_cfg.bitrate,
        )
        self.frame_period_s = can_cfg.frame_period_s
        self._stop = threading.Event()
        self._thread: threading.Thread | None = None

    def _frames_for(self, signals: dict[str, float]) -> dict[int, dict]:
        """Group 'MessageName.SignalName: value' entries into per-message signal dicts."""
        per_message: dict[str, dict[str, float]] = {}
        for key, value in signals.items():
            msg_name, sig_name = key.split(".", 1)
            per_message.setdefault(msg_name, {})[sig_name] = value

        frames = {}
        for msg_name, sig_values in per_message.items():
            message = self.db.get_message_by_name(msg_name)
            # unspecified signals in this message default to 0
            full = {s.name: sig_values.get(s.name, 0) for s in message.signals}
            data = message.encode(full)
            frames[message.frame_id] = {"data": data, "dlc": message.length}
        return frames

    def _sender_loop(self, frames: dict[int, dict]):
        msgs = [can.Message(arbitration_id=fid, data=f["data"], is_extended_id=False)
                for fid, f in frames.items()]
        while not self._stop.is_set():
            for m in msgs:
                self.bus.send(m)
            time.sleep(self.frame_period_s)

    def start(self, signals: dict[str, float]):
        """Start continuously broadcasting the given scenario's signal values."""
        self.stop()
        frames = self._frames_for(signals)
        self._stop.clear()
        self._thread = threading.Thread(target=self._sender_loop, args=(frames,), daemon=True)
        self._thread.start()

    def stop(self):
        if self._thread is not None:
            self._stop.set()
            self._thread.join(timeout=2.0)
            self._thread = None

    def close(self):
        self.stop()
        self.bus.shutdown()
