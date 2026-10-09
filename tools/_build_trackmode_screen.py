"""
Build Screen1_1 (TRACK mode) layout inside Firmware/eez/eez002/eez002.eez-project.

Screen1_1 already exists as a straight duplicate of Screen1 (made by hand in EEZ
Studio). This script repositions/restyles its existing widgets to the TRACK
layout described in Firmware/eez/eez002/trackmode_screen_design/, reusing the
project's existing B612 fonts (no new font generation) and deletes/adds only
the minimum widgets needed.

Design source canvas is 1198x381 (Firmware/eez/eez002/trackmode_screen_design/layout.json).
Actual target is the existing ui_ContainerDashboard_1 box: 765x256, offset (18,0)
within the 800x256 screen (same as Screen1). The design's own "dashboard" box
(x=35, width=1163, full height=381) is the reference frame we scale from, so the
existing (already correct) 35px telltale reservation on the real screen is left
untouched -- only the content *inside* the dashboard container is rescaled.

Run once; re-run is idempotent against a clean checkout (it finds widgets by
identifier and will fail loudly if something unexpected is missing).
"""
import json
import uuid

PROJ = r"C:\Users\chake\Documents\GitHub\FULLMONI-WIDE\Firmware\eez\eez002\eez002.eez-project"

# --- scale: design(1198x381, dashboard sub-box x=35..1198,y=0..381) -> actual container (765x256) ---
DASH_X = 35
DASH_W = 1163
DESIGN_H = 381
PW, PH = 765, 256
CX, CY = PW / 2, PH / 2
XSCALE = PW / DASH_W
YSCALE = PH / DESIGN_H

COLORS = {
    "fg": "#FFFFFF",
    "secondary": "#D3D3D3",
    "track": "#252525",
    "red": "#FF4149",
}


def uid():
    return str(uuid.uuid4())


def d2c(x, y, w, h):
    """design LCD box -> container-relative px box (float)."""
    return (x - DASH_X) * XSCALE, y * YSCALE, w * XSCALE, h * YSCALE


ANCHORS = {
    "RIGHT_MID": ((1.0, 0.5), (PW, CY)),
    "LEFT_MID": ((0.0, 0.5), (0.0, CY)),
    "CENTER": ((0.5, 0.5), (CX, CY)),
    "TOP_LEFT": ((0.0, 0.0), (0.0, 0.0)),
}


def offset_for(box, align):
    cx, cy, cw, ch = box
    (ox, oy), (px, py) = ANCHORS[align]
    left = round((cx + cw * ox) - px)
    top = round((cy + ch * oy) - py)
    return left, top


def find(node, ident):
    if node.get("identifier") == ident:
        return node
    for c in node.get("children", []):
        r = find(c, ident)
        if r:
            return r
    return None


def remove(children, ident):
    for i, c in enumerate(children):
        if c.get("identifier") == ident:
            del children[i]
            return True
    return False


def set_pos(node, align, box, keep_size_unit=True):
    left, top = offset_for(box, align)
    node["left"] = left
    node["top"] = top
    defn = node["localStyles"]["definition"].setdefault("MAIN", {}).setdefault("DEFAULT", {})
    defn["align"] = align


def label_widget(identifier, align, text_align, font, color, text, box, width_px=None):
    left, top = offset_for(box, align)
    main_default = {
        "align": align,
        "text_align": text_align,
        "text_font": font,
        "text_color": color,
    }
    if width_px is not None:
        main_default["width"] = width_px
    return {
        "objID": uid(), "type": "LVGLLabelWidget",
        "left": left, "top": top, "width": 100, "height": 28,
        "customInputs": [], "customOutputs": [],
        "style": {"objID": uid(), "useStyle": "default", "conditionalStyles": [], "childStyles": []},
        "hiddenInEditor": False, "timeline": [], "eventHandlers": [],
        "identifier": identifier,
        "leftUnit": "px", "topUnit": "px", "widthUnit": "content", "heightUnit": "content",
        "children": [], "widgetFlags": "", "hiddenFlagType": "literal",
        "clickableFlagType": "literal", "flagScrollbarMode": "", "flagScrollDirection": "",
        "scrollSnapX": "", "scrollSnapY": "", "checkedStateType": "literal",
        "disabledStateType": "literal", "states": "",
        "localStyles": {"objID": uid(), "definition": {"MAIN": {"DEFAULT": main_default}}},
        "group": "", "groupIndex": 0, "text": text, "textType": "literal",
        "longMode": "CLIP", "recolor": False,
    }


def bar_widget(identifier, box, vmin, vmax, value, fg_color, bg_color):
    left, top = offset_for(box, "CENTER")
    _, _, cw, ch = box
    return {
        "objID": uid(), "type": "LVGLBarWidget",
        "left": left, "top": top, "width": round(cw), "height": round(ch),
        "customInputs": [], "customOutputs": [],
        "style": {"objID": uid(), "useStyle": "default", "conditionalStyles": [], "childStyles": []},
        "hiddenInEditor": False, "timeline": [], "eventHandlers": [],
        "identifier": identifier,
        "leftUnit": "px", "topUnit": "px", "widthUnit": "px", "heightUnit": "px",
        "children": [], "widgetFlags": "", "hiddenFlagType": "literal",
        "clickableFlagType": "literal", "flagScrollbarMode": "", "flagScrollDirection": "",
        "scrollSnapX": "", "scrollSnapY": "", "checkedStateType": "literal",
        "disabledStateType": "literal", "states": "",
        "localStyles": {"objID": uid(), "definition": {
            "MAIN": {"DEFAULT": {"align": "CENTER", "radius": 0, "bg_color": bg_color, "bg_opa": 255}},
            "INDICATOR": {"DEFAULT": {"radius": 0, "bg_color": fg_color, "bg_opa": 255}},
        }},
        "group": "", "groupIndex": 0, "min": vmin, "max": vmax, "mode": "NORMAL",
        "value": value, "valueType": "literal", "enableAnimation": False,
    }


def main():
    with open(PROJ, encoding="utf-8") as f:
        proj = json.load(f)

    page = next(p for p in proj["userPages"] if p["name"] == "Screen1_1")
    screen = page["components"][0]
    dash = find(screen, "ui_ContainerDashboard_1")
    assert dash is not None
    dash_children = dash["children"]

    # --- 1. delete widgets not used by the TRACK layout -----------------
    for ident in ("ui_ArcRPM_1", "ui_ImgTacho_1", "ui_imageRPM_1", "ui_ImagePeakRPM_1",
                  "ui_LblIAT_1", "ui_LblMAP_1", "ui_LblGEAR_1"):
        ok = remove(dash_children, ident)
        assert ok, f"expected to find+remove {ident}"

    # --- 2. reposition reused widgets (keep font/color, just move) ------
    RIGHT_MID = "RIGHT_MID"

    rpm_lbl = find(dash, "ui_LblRPM_1")
    set_pos(rpm_lbl, RIGHT_MID, d2c(65, 130, 125.266, 40))

    spd_lbl = find(dash, "ui_LblSPD_1")
    set_pos(spd_lbl, RIGHT_MID, d2c(366, 106, 501, 112))

    time_lbl = find(dash, "ui_LblTIME_1")
    set_pos(time_lbl, RIGHT_MID, d2c(1076.406, 130, 92.594, 41))

    afr_lbl = find(dash, "ui_LblAFR_1")
    set_pos(afr_lbl, RIGHT_MID, d2c(605.438, 334.5, 276.25, 39))

    volt_lbl = find(dash, "ui_LblBattery_1")
    set_pos(volt_lbl, RIGHT_MID, d2c(881.688, 334.5, 287.297, 39))

    # health row: big numbers, bottom 2/3 of each cell, CENTER aligned
    def health_number_box(x, w):
        cx, cy, cw, ch = d2c(x, 228, w, 98)
        caption_h = 22 * YSCALE
        return (cx, cy + caption_h, cw, ch - caption_h)

    oilpress_lbl = find(dash, "ui_LblOilPress_1")
    oilpress_lbl["localStyles"]["definition"]["MAIN"]["DEFAULT"]["text_align"] = "CENTER"
    set_pos(oilpress_lbl, "CENTER", health_number_box(59, 371.656))

    oiltemp_lbl = find(dash, "ui_LblOilTemp_1")
    oiltemp_lbl["localStyles"]["definition"]["MAIN"]["DEFAULT"]["text_align"] = "CENTER"
    set_pos(oiltemp_lbl, "CENTER", health_number_box(430.656, 371.672))

    watertemp_lbl = find(dash, "ui_LblWaterTemp_1")
    watertemp_lbl["localStyles"]["definition"]["MAIN"]["DEFAULT"]["text_align"] = "CENTER"
    set_pos(watertemp_lbl, "CENTER", health_number_box(802.328, 371.656))

    # fuel bar: keep native 100x15 (bg_img_src gauge graphic), just move its center
    fuel_bar = find(dash, "ui_BarFUEL_1")
    fbx, fby, fbw, fbh = d2c(159.141, 344.5, 381.688, 19)
    fuel_center_box = (fbx + fbw / 2 - 50, fby + fbh / 2 - 7.5, 100, 15)
    set_pos(fuel_bar, "CENTER", fuel_center_box)

    # --- 3. new static caption / heading labels --------------------------
    new_labels = []

    def cap(ident, text, box, align="LEFT_MID", text_align="LEFT"):
        new_labels.append(label_widget(ident, align, text_align, "ui_font_612_Small",
                                        COLORS["secondary"], text, box))

    cap("ui_LblTrackMode_1", "TRACK", d2c(59, 13, 400, 25))
    cap("ui_LblTachUnit_1", "x1000rpm", d2c(59, 13, 1115, 25), align="RIGHT_MID", text_align="RIGHT")

    def health_caption_box(x, w):
        cx, cy, cw, ch = d2c(x, 228, w, 22)
        return (cx, cy, cw, ch)

    cap("ui_LblOilPressCap_1", "OIL PRESS", health_caption_box(59, 371.656))
    cap("ui_LblOilTempCap_1", "OIL TEMP", health_caption_box(430.656, 371.672))
    cap("ui_LblWaterTempCap_1", "WATER TEMP", health_caption_box(802.328, 371.656))

    cap("ui_LblFuelCap_1", "FUEL", d2c(64, 341.5, 90, 25))
    cap("ui_LblFuelE_1", "E", d2c(159.141, 344.5, 0, 19))
    cap("ui_LblFuelF_1", "F", d2c(159.141, 344.5, 381.688, 19), align="RIGHT_MID", text_align="RIGHT")

    cap("ui_LblAfrCap_1", "AFR", d2c(605.438, 334.5, 90, 39))
    cap("ui_LblVoltCap_1", "VOLT", d2c(881.688, 334.5, 90, 39))

    dash_children.extend(new_labels)

    # --- 4. new RPM bar (replaces arc gauge) ------------------------------
    rpm_bar = bar_widget("ui_BarRPM_1", d2c(59, 82, 1115, 30), 0, 9000, 3000,
                          COLORS["fg"], COLORS["track"])
    dash_children.append(rpm_bar)

    with open(PROJ, "w", encoding="utf-8", newline="\n") as f:
        json.dump(proj, f, ensure_ascii=False, indent=2)

    print("Screen1_1 updated:", len(dash_children), "children in ui_ContainerDashboard_1")


if __name__ == "__main__":
    main()
