#!/usr/bin/env python3
"""Update fullmoni.eez-project fonts to match aw002 AppWizard font assignments.

aw002 widget -> font mapping:
  ID_NUM_GEAR  -> HeadUpDaisy 72px Bold
  ID_NUM_AF    -> HeadUpDaisy 36px Bold  (AFR)
  ID_NUM_SPEED -> HeadUpDaisy 36px Bold
  ID_NUM_RV    -> HeadUpDaisy 30px Bold  (RPM value)
  ID_NUM_01-06 -> HeadUpDaisy 27px Bold  (6ch sensor values)
  ID_NUM_TIME  -> HeadUpDaisy 18px Normal (already correct)
  ID_NUM_TRIP  -> Arial 16px Bold
  ID_NUM_ODO   -> Arial 16px Bold
  ID_NUM_FPS   -> Arial 19px Normal (no eez widget)
  ID_TEXT_ACC  -> 51_20 Normal (no eez widget)

Current EEZ fonts:
  ui_font_Font1      -> HeadUpDaisy 24px  (used by 6ch labels)
  ui_font_FontHUDmid -> HeadUpDaisy 18px  (TIME - OK)
  ui_font_FontHUDsmall -> DenkiChip 12px  (small labels)
  ui_font_FontLARGR  -> HeadUpDaisy 54px  (GEAR etc)
"""
import json
import uuid
import shutil
from pathlib import Path

PROJ_PATH = Path(__file__).resolve().parent.parent / "Firmware" / "eez" / "fullmoni.eez-project"


def make_font(name, ttf_file, size, bpp=1):
    """Create an EEZ font definition dict."""
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
    """Set localStyles text_font for a widget."""
    w = find_widget(children, identifier)
    if not w:
        print(f"  WARNING: widget '{identifier}' not found")
        return
    # Ensure localStyles structure exists
    if "localStyles" not in w:
        w["localStyles"] = {}
    ls = w["localStyles"]
    if "definition" not in ls:
        ls["definition"] = {}
    defn = ls["definition"]
    if "MAIN" not in defn:
        defn["MAIN"] = {}
    defn["MAIN"]["text_font"] = font_name
    print(f"  {identifier}: text_font -> {font_name}")


def main():
    with open(PROJ_PATH, "r", encoding="utf-8") as f:
        proj = json.load(f)

    fonts = proj.get("fonts", [])
    font_names = {f["name"] for f in fonts}

    # ─── 1. Update existing font sizes ───────────────────────────
    print("=== Updating existing font sizes ===")
    for f in fonts:
        name = f["name"]
        src = f.get("source", {})
        old_size = src.get("size")

        if name == "ui_font_Font1":
            # 24 -> 27 (HeadUpDaisy, for 6ch sensor values)
            src["size"] = 27
            print(f"  {name}: size {old_size} -> 27")

        elif name == "ui_font_FontLARGR":
            # 54 -> 72 (HeadUpDaisy, for GEAR)
            src["size"] = 72
            print(f"  {name}: size {old_size} -> 72")

        elif name == "ui_font_FontHUDsmall":
            # 12 -> 16 (DenkiChip, for TRIP/ODO - aw002 uses Arial 16 Bold)
            src["size"] = 16
            print(f"  {name}: size {old_size} -> 16")

    # ─── 2. Add missing font definitions ─────────────────────────
    print("\n=== Adding new font definitions ===")

    new_fonts = [
        ("ui_font_FontHUD36", "x14y24pxHeadUpDaisy.ttf", 36),   # SPEED, AFR
        ("ui_font_FontHUD30", "x14y24pxHeadUpDaisy.ttf", 30),   # RPM value
    ]
    for name, ttf, size in new_fonts:
        if name not in font_names:
            fonts.append(make_font(name, ttf, size))
            print(f"  Added: {name} ({ttf} @ {size}px)")
        else:
            print(f"  Already exists: {name}")

    proj["fonts"] = fonts

    # ─── 3. Assign fonts to widgets ──────────────────────────────
    print("\n=== Assigning fonts to widgets ===")
    screen = proj["userPages"][0]["components"][0]
    children = screen["children"]

    # GEAR -> FontLARGR (72px, already existed, just resized)
    set_widget_font(children, "ui_LblGEAR", "ui_font_FontLARGR")

    # SPEED, AFR -> FontHUD36 (36px, new)
    set_widget_font(children, "ui_LblSPD", "ui_font_FontHUD36")
    set_widget_font(children, "ui_LblAFR", "ui_font_FontHUD36")

    # RPM value -> FontHUD30 (30px, new)
    set_widget_font(children, "ui_LblRPM", "ui_font_FontHUD30")

    # 6ch sensor values -> Font1 (27px, resized from 24)
    for lbl in ["ui_LblWaterTemp", "ui_LblIAT", "ui_LblOilTemp",
                "ui_LblMAP", "ui_LblOilPress", "ui_LblBattery"]:
        set_widget_font(children, lbl, "ui_font_Font1")

    # TIME -> FontHUDmid (18px, already correct)
    set_widget_font(children, "ui_LblTIME", "ui_font_FontHUDmid")

    # TRIP, ODO -> FontHUDsmall (DenkiChip 16px, aw002 uses Arial 16 Bold)
    set_widget_font(children, "ui_LblTrip", "ui_font_FontHUDsmall")
    set_widget_font(children, "ui_LblODO", "ui_font_FontHUDsmall")

    # Small utility labels keep FontHUDsmall (12px DenkiChip)
    for lbl in ["ui_LblRPMUnit", "ui_LblSPDUnit",
                "ui_LblTripName", "ui_LblTripUnit",
                "ui_LblODOName", "ui_LblODOUnit",
                "ui_LblEmpty", "ui_LblFull"]:
        set_widget_font(children, lbl, "ui_font_FontHUDsmall")

    # ─── Save ────────────────────────────────────────────────────
    backup = PROJ_PATH.with_suffix(".eez-project.font-bak")
    shutil.copy2(PROJ_PATH, backup)
    print(f"\nBackup: {backup}")

    with open(PROJ_PATH, "w", encoding="utf-8") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)
    print(f"Saved: {PROJ_PATH}")


if __name__ == "__main__":
    main()
