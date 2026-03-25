#!/usr/bin/env python3
"""Update fullmoni.eez-project fonts to match aw002 -- V2 (safe).

Changes:
1. Font size adjustments:
   - ui_font_Font1:      24 -> 27  (HeadUpDaisy, 6ch sensor values)
   - ui_font_FontLARGR:  54 -> 72  (HeadUpDaisy, GEAR)
   - ui_font_FontHUDsmall: 12 -> 16 (DenkiChip, TRIP/ODO/units)

2. New font definitions:
   - ui_font_FontHUD36: HeadUpDaisy 36px (SPEED, AFR)
   - ui_font_FontHUD30: HeadUpDaisy 30px (RPM value)

3. Update text_font on widgets that need a DIFFERENT font than current:
   - ui_LblSPD:  ui_font_FontLARGR -> ui_font_FontHUD36
   - ui_LblAFR:  (whatever) -> ui_font_FontHUD36
   - ui_LblRPM:  ui_font_Font1 -> ui_font_FontHUD30
   - ui_LblGEAR: (whatever) -> ui_font_FontLARGR (keep/set)

Correct localStyles path: definition.MAIN.DEFAULT.text_font
"""
import json
import uuid
import shutil
from pathlib import Path

PROJ_PATH = Path(__file__).resolve().parent.parent / "Firmware" / "eez" / "fullmoni.eez-project"


def make_font(name, ttf_file, size, bpp=1):
    return {
        "objID": str(uuid.uuid4()),
        "name": name,
        "renderingEngine": "LVGL",
        "source": {
            "objID": str(uuid.uuid4()),
            "filePath": ttf_file,
            "size": size,
        },
        "bpp": bpp,
        "threshold": 128,
        "glyphs": [],
        "lvglRanges": "32-127",
        "lvglSymbols": "",
    }


def find_widget(children, identifier):
    for w in children:
        if w.get("identifier") == identifier:
            return w
        if "children" in w:
            r = find_widget(w["children"], identifier)
            if r:
                return r
    return None


def set_widget_font(children, identifier, font_name):
    """Set text_font at the CORRECT path: localStyles.definition.MAIN.DEFAULT.text_font"""
    w = find_widget(children, identifier)
    if not w:
        print(f"  WARNING: widget '{identifier}' not found")
        return

    ls = w.get("localStyles", {})
    defn = ls.get("definition", {})
    main = defn.get("MAIN", {})
    default = main.get("DEFAULT", {})

    old_font = default.get("text_font", "(none)")

    # Only update if different
    if old_font == font_name:
        print(f"  {identifier}: already {font_name}")
        return

    default["text_font"] = font_name

    # Rebuild path if needed
    main["DEFAULT"] = default
    defn["MAIN"] = main

    if "objID" not in ls:
        ls["objID"] = str(uuid.uuid4())
    ls["definition"] = defn
    w["localStyles"] = ls

    print(f"  {identifier}: {old_font} -> {font_name}")


def main():
    with open(PROJ_PATH, "r", encoding="utf-8") as f:
        proj = json.load(f)

    fonts = proj.get("fonts", [])
    font_names = {f["name"] for f in fonts}

    # ─── 1. Resize existing fonts ────────────────────────────────
    print("=== Resizing existing fonts ===")
    size_changes = {
        "ui_font_Font1": 27,       # was 24
        "ui_font_FontLARGR": 72,   # was 54
        "ui_font_FontHUDsmall": 16, # was 12
    }
    for f in fonts:
        name = f["name"]
        if name in size_changes:
            src = f.get("source", {})
            old = src.get("size")
            new = size_changes[name]
            src["size"] = new
            print(f"  {name}: {old} -> {new}")

    # ─── 2. Add new fonts ────────────────────────────────────────
    print("\n=== Adding new fonts ===")
    new_fonts = [
        ("ui_font_FontHUD36", "x14y24pxHeadUpDaisy.ttf", 36),
        ("ui_font_FontHUD30", "x14y24pxHeadUpDaisy.ttf", 30),
    ]
    for name, ttf, size in new_fonts:
        if name not in font_names:
            fonts.append(make_font(name, ttf, size))
            print(f"  Added: {name} ({ttf} @ {size}px)")
        else:
            print(f"  Exists: {name}")
    proj["fonts"] = fonts

    # ─── 3. Update widget font assignments (only where needed) ──
    print("\n=== Updating widget text_font ===")
    screen = proj["userPages"][0]["components"][0]
    children = screen["children"]

    # Widgets that need a different font than currently assigned
    font_assignments = {
        "ui_LblGEAR":  "ui_font_FontLARGR",   # 72px
        "ui_LblSPD":   "ui_font_FontHUD36",    # 36px (was FontLARGR=54)
        "ui_LblAFR":   "ui_font_FontHUD36",    # 36px
        "ui_LblRPM":   "ui_font_FontHUD30",    # 30px (was Font1=24)
        # 6ch sensor labels: keep ui_font_Font1 (now 27px) - no change needed
        # TIME: keep ui_font_FontHUDmid (18px) - no change needed
        # TRIP, ODO: keep ui_font_FontHUDsmall (now 16px) - check if already set
        "ui_LblTrip":  "ui_font_FontHUDsmall",
        "ui_LblODO":   "ui_font_FontHUDsmall",
    }

    for widget_id, font_name in font_assignments.items():
        set_widget_font(children, widget_id, font_name)

    # ─── Save ────────────────────────────────────────────────────
    backup = PROJ_PATH.with_suffix(".eez-project.font-bak2")
    shutil.copy2(PROJ_PATH, backup)
    print(f"\nBackup: {backup}")

    with open(PROJ_PATH, "w", encoding="utf-8") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)
    print(f"Saved: {PROJ_PATH}")


if __name__ == "__main__":
    main()
