#!/usr/bin/env python3
"""Fix remaining font assignment issues in eez-project."""
import json
import shutil
from pathlib import Path

PROJ_PATH = Path(__file__).resolve().parent.parent / "Firmware" / "eez" / "fullmoni.eez-project"


def find_widget(children, identifier):
    for w in children:
        if w.get("identifier") == identifier:
            return w
        if "children" in w:
            r = find_widget(w["children"], identifier)
            if r:
                return r
    return None


def get_font(w):
    ls = w.get("localStyles", {})
    return ls.get("definition", {}).get("MAIN", {}).get("DEFAULT", {}).get("text_font", "(none)")


def set_font(w, font_name):
    ls = w.setdefault("localStyles", {})
    defn = ls.setdefault("definition", {})
    main = defn.setdefault("MAIN", {})
    default = main.setdefault("DEFAULT", {})
    old = default.get("text_font", "(none)")
    default["text_font"] = font_name
    return old


def main():
    with open(PROJ_PATH, "r", encoding="utf-8") as f:
        proj = json.load(f)

    screen = proj["userPages"][0]["components"][0]
    children = screen["children"]

    # Complete aw002 font mapping
    # aw002 widget -> font -> EEZ font name
    assignments = {
        # 01a area
        "ui_LblGEAR":      "ui_font_FontLARGR",    # HeadUpDaisy 72
        "ui_LblRPM":       "ui_font_FontHUD30",     # HeadUpDaisy 30
        "ui_LblRPMUnit":   "ui_font_FontHUDsmall",  # small label
        "ui_LblAFR":       "ui_font_FontHUD36",     # HeadUpDaisy 36
        "ui_LblTIME":      "ui_font_FontHUDmid",    # HeadUpDaisy 18 ← FIX

        # 01b area (6ch sensor values)
        "ui_LblWaterTemp":  "ui_font_Font1",        # HeadUpDaisy 27
        "ui_LblIAT":        "ui_font_Font1",
        "ui_LblOilTemp":    "ui_font_Font1",
        "ui_LblMAP":        "ui_font_Font1",
        "ui_LblOilPress":   "ui_font_Font1",
        "ui_LblBattery":    "ui_font_Font1",

        # 01c area
        "ui_LblSPD":       "ui_font_FontHUD36",     # HeadUpDaisy 36
        "ui_LblSPDUnit":   "ui_font_FontHUDsmall",  # small label ← FIX
        "ui_LblTrip":      "ui_font_FontHUDsmall",  # Arial 16 → DenkiChip 16
        "ui_LblTripName":  "ui_font_FontHUDsmall",
        "ui_LblTripUnit":  "ui_font_FontHUDsmall",
        "ui_LblODO":       "ui_font_FontHUDsmall",  # Arial 16 → DenkiChip 16
        "ui_LblODOName":   "ui_font_FontHUDsmall",
        "ui_LblODOUnit":   "ui_font_FontHUDsmall",
        "ui_LblEmpty":     "ui_font_FontHUDsmall",
        "ui_LblFull":      "ui_font_FontHUDsmall",
    }

    print("=== Checking/fixing font assignments ===")
    changed = 0
    for widget_id, expected_font in assignments.items():
        w = find_widget(children, widget_id)
        if not w:
            print(f"  WARNING: {widget_id} not found")
            continue
        current = get_font(w)
        if current != expected_font:
            set_font(w, expected_font)
            print(f"  FIXED: {widget_id}: {current} -> {expected_font}")
            changed += 1
        else:
            print(f"  OK:    {widget_id}: {expected_font}")

    if changed == 0:
        print("\nNo changes needed.")
        return

    backup = PROJ_PATH.with_suffix(".eez-project.font-fix")
    shutil.copy2(PROJ_PATH, backup)
    print(f"\nFixed {changed} widgets. Backup: {backup}")

    with open(PROJ_PATH, "w", encoding="utf-8") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)
    print(f"Saved: {PROJ_PATH}")


if __name__ == "__main__":
    main()
