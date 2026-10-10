"""Demo: simulate a lap of circuit driving over CAN, for showing off the dashboard.

Not a regression test -- no camera/VLM involved, just a continuous, smoothly
animated CAN stream (RPM/water/oil temp/oil pressure/AFR) shaped like a lap:
straights with gear-shift RPM sawtooth, braking zones, corners held at
partial throttle. Loops until Ctrl+C (or --laps N).

Only drives the signals that are actually bound to something on-screen
(see Firmware/src/param_storage.c's default field table / dataregister.c):
RPM, EngineTemp (WATER), OilTemp, OilPressure, Lambda1 (A/F). ManifolePressure/
IntakeAirTemperature/GroundSpeed/BatteryVoltage are either not displayed on
the TRACK screen or (VOLT/FUEL) are driven by analog inputs, not CAN -- see
Firmware/eez/eez002/hil_test/scenarios.yaml's header comment -- so there's no
point animating them here.

Usage:
    python demo_lap.py [--design eez002] [--laps N] [--lap-time SECONDS]
"""
from __future__ import annotations

import argparse
import random
import time

from scenario import load_can_config
from dbc_patterns import PatternPlayer

# Each lap is this sequence of (duration_s, kind) segments. kind in
# {"accel", "brake", "corner"} shapes how RPM/throttle move during it.
# Loosely: start/finish straight -> turn1 -> short straight -> turn2 ->
# back straight (long) -> chicane -> exit onto start/finish straight again.
LAP_SEGMENTS = [
    (4.0, "accel"),   # start/finish straight
    (2.2, "brake"),   # turn 1 braking
    (1.4, "corner"),  # turn 1 apex
    (2.3, "accel"),   # turn 1 exit -> short straight
    (1.8, "brake"),   # turn 2 braking
    (1.4, "corner"),  # turn 2 apex
    (5.0, "accel"),   # back straight (long)
    (2.3, "brake"),   # chicane braking
    (1.0, "corner"),  # chicane entry
    (1.0, "corner"),  # chicane exit
    (3.0, "accel"),   # exit onto start/finish straight
]

TICK_S = 0.05  # 20Hz simulation update

RPM_IDLE = 1200.0
RPM_REDLINE = 8500.0
RPM_SHIFT_DROP = 4300.0  # RPM right after an upshift

WATER_TARGET = 88.0
OIL_TARGET = 98.0


class EngineSim:
    def __init__(self):
        self.rpm = RPM_IDLE
        self.throttle = 0.0
        self.water = 70.0  # starts cold, warms up toward WATER_TARGET
        self.oil = 65.0

    def step(self, dt: float, kind: str, corner_target: float):
        # --- throttle: ease toward the target for this segment kind ---
        throttle_target = {"accel": 0.95, "brake": 0.05, "corner": 0.35}[kind]
        self.throttle += (throttle_target - self.throttle) * min(1.0, 3.0 * dt)

        # --- RPM: shaped per segment kind ---
        if kind == "accel":
            self.rpm += 2600.0 * dt
            if self.rpm >= RPM_REDLINE:
                self.rpm = RPM_SHIFT_DROP  # upshift
        elif kind == "brake":
            self.rpm -= 2200.0 * dt
            if self.rpm < RPM_IDLE:
                self.rpm = RPM_IDLE
        else:  # corner
            self.rpm += (corner_target - self.rpm) * min(1.0, 2.5 * dt)

        # --- derived signals (function of rpm/throttle, matches real engine
        # behavior loosely enough for a visual demo) ---
        oil_press = 180.0 + (self.rpm - RPM_IDLE) / (RPM_REDLINE - RPM_IDLE) * 380.0
        oil_press += random.uniform(-8.0, 8.0)
        oil_press = max(150.0, oil_press)

        afr = 14.7 - self.throttle * 2.5 + random.uniform(-0.1, 0.1)

        # water/oil temp: slow first-order warm-up toward target, with a
        # little load-dependent wobble so it's not perfectly flat
        water_target = WATER_TARGET + self.throttle * 3.0
        oil_target = OIL_TARGET + self.throttle * 5.0
        self.water += (water_target - self.water) * min(1.0, 0.15 * dt)
        self.oil += (oil_target - self.oil) * min(1.0, 0.10 * dt)

        return {
            "DataFrame1.RPM": max(0, int(self.rpm)),
            "DataFrame2.EngineTemp": round(self.water, 1),
            "DataFrame2.Lambda1": round(afr, 2),
            "DataFrame3.OilTemp": round(self.oil, 1),
            "DataFrame4.OilPressure": round(oil_press, 1),
        }


def run(design: str, laps: int | None, lap_time_scale: float):
    can_cfg = load_can_config(design)
    player = PatternPlayer(can_cfg)
    sim = EngineSim()

    lap_no = 0
    try:
        while laps is None or lap_no < laps:
            lap_no += 1
            print(f"--- Lap {lap_no}{'' if laps is None else f'/{laps}'} ---")
            for seg_i, (duration, kind) in enumerate(LAP_SEGMENTS):
                duration *= lap_time_scale
                corner_target = random.uniform(3200.0, 4600.0)
                elapsed = 0.0
                while elapsed < duration:
                    signals = sim.step(TICK_S, kind, corner_target)
                    player.start(signals)
                    print(f"\r  seg {seg_i+1}/{len(LAP_SEGMENTS)} {kind:7s} "
                          f"RPM={signals['DataFrame1.RPM']:5d}  "
                          f"OIL-P={signals['DataFrame4.OilPressure']:6.1f}  "
                          f"A/F={signals['DataFrame2.Lambda1']:5.2f}  "
                          f"WATER={signals['DataFrame2.EngineTemp']:5.1f}  "
                          f"OIL-T={signals['DataFrame3.OilTemp']:5.1f}", end="", flush=True)
                    time.sleep(TICK_S)
                    elapsed += TICK_S
            print()
    except KeyboardInterrupt:
        print("\nStopped.")
    finally:
        player.close()


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--design", default="eez002", help="e.g. eez002 (Firmware/eez/<design>/hil_test/ must exist)")
    ap.add_argument("--laps", type=int, default=None, help="number of laps (default: loop forever, Ctrl+C to stop)")
    ap.add_argument("--lap-time", type=float, default=1.0,
                     help="scale factor for segment durations (e.g. 0.5 = twice as fast)")
    args = ap.parse_args()
    run(args.design, args.laps, args.lap_time)


if __name__ == "__main__":
    main()
