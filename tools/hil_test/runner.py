"""CLI entrypoint for the HIL (CAN -> camera -> VLM) regression test framework.

Usage:
    python runner.py --design eez002
    python runner.py --design eez002 --scenario oil_press_low_regression master_warning_no_brake
    python runner.py --design eez002 --list

Requires: PCAN USB adapter + USB camera connected, firmware flashed and
running, ANTHROPIC_API_KEY set. See README.md.
"""
from __future__ import annotations

import argparse
import datetime
import os
import sys
import time

import camera
import dbc_patterns
import report
import scenario
import vlm_check

REPO_ROOT = scenario.REPO_ROOT


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--design", required=True, help="e.g. eez002 (Firmware/eez/<design>/hil_test/ must exist)")
    ap.add_argument("--scenario", nargs="*", help="run only these scenario ids (default: all)")
    ap.add_argument("--list", action="store_true", help="list available scenarios and exit")
    ap.add_argument("--out-dir", help="override output dir (default: test-results/<design>/<timestamp>/)")
    args = ap.parse_args()

    scenarios = scenario.load_scenarios(args.design, only=args.scenario)
    if args.list:
        for s in scenarios:
            print(f"{s.id}: {s.description}")
        return

    if not scenarios:
        print("No matching scenarios.", file=sys.stderr)
        sys.exit(1)

    can_cfg = scenario.load_can_config(args.design)
    cam_cfg = scenario.load_camera_config(args.design)

    out_dir = args.out_dir or os.path.join(
        REPO_ROOT, "test-results", args.design,
        datetime.datetime.now().strftime("%Y%m%d_%H%M%S"),
    )

    player = dbc_patterns.PatternPlayer(can_cfg)
    cam = camera.Camera(cam_cfg)
    results = []
    try:
        for s in scenarios:
            print(f"[{s.id}] sending CAN pattern...")
            player.start(s.signals)
            time.sleep(can_cfg.settle_s)

            img_path = os.path.join(out_dir, f"{s.id}.png")
            print(f"[{s.id}] capturing...")
            cam.capture(img_path)

            print(f"[{s.id}] asking VLM...")
            verdict = vlm_check.check(img_path, s.expect)

            status = "PASS" if verdict.passed else "FAIL"
            print(f"[{s.id}] {status}: {verdict.observed}")
            if verdict.mismatches:
                for m in verdict.mismatches:
                    print(f"    - {m}")

            results.append({
                "id": s.id,
                "description": s.description,
                "expect": s.expect,
                "passed": verdict.passed,
                "observed": verdict.observed,
                "mismatches": verdict.mismatches,
                "image_path": img_path,
            })
    finally:
        player.close()
        cam.close()

    report_path = report.write(results, out_dir)
    n_pass = sum(1 for r in results if r["passed"])
    print(f"\n{n_pass}/{len(results)} passed. Report: {report_path}")
    sys.exit(0 if n_pass == len(results) else 1)


if __name__ == "__main__":
    main()
