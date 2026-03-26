"""Rebuild eez003.eez-project - v3 faithful to reference image.

Uses CENTER align coordinate system (same as eez002):
  Parent: ContainerDashboard 765x256, align=CENTER
  (0,0) = center of dashboard = approx pixel (382, 128) in absolute coords
  Positive x = right, positive y = down

Reference image layout:
  LEFT: 6 sensor icons + bars + values + units (stacked vertically)
  CENTER: Fan tachometer (large) with number ring 0-9
  RIGHT OF TACHO: Gear letter (large "N")
  BELOW TACHO: RPM value "3000"
  TOP CENTER: Time "00:00"
  RIGHT: Speed "180" + "km/h"
  BOTTOM CENTER: AFR box "AFR 14.7"
  BOTTOM RIGHT: "Trip: 0.0 km" / "Total: 999999 km" + Fuel gauge E-F
"""
import json
import uuid
import base64
import os

PROJ = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003\eez003.eez-project"
ASSET_DIR = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003"

def uid(): return str(uuid.uuid4())

def style_obj():
    return {"objID": uid(), "useStyle": "default", "conditionalStyles": [], "childStyles": []}

def lsty(defn):
    return {"objID": uid(), "definition": defn}

def w_base(wtype, left, top, width, height, ident=None, hidden=False, children=None):
    w = {
        "objID": uid(), "type": wtype,
        "left": left, "top": top, "width": width, "height": height,
        "customInputs": [], "customOutputs": [],
        "style": style_obj(), "timeline": [], "eventHandlers": [],
        "leftUnit": "px", "topUnit": "px", "widthUnit": "px", "heightUnit": "px",
    }
    if ident: w["identifier"] = ident
    if hidden: w["hiddenFlag"] = "HIDDEN"
    if children is not None: w["children"] = children
    elif wtype == "LVGLPanelWidget": w["children"] = []
    if wtype == "LVGLPanelWidget": w["hiddenInEditor"] = False
    return w

# ----- Widget constructors -----
# All use CENTER align like eez002

def _panel(left, top, w, h, ident, children, ls=None, hidden=False):
    o = w_base("LVGLPanelWidget", left, top, w, h, ident, hidden, children)
    if ls: o["localStyles"] = ls
    return o

def _lbl(left, top, w, h, text, ident, font, color, align="CENTER", ta=None, hidden=False, lm=None):
    o = w_base("LVGLLabelWidget", left, top, w, h, ident, hidden)
    o["text"] = text
    if lm: o["longMode"] = lm
    sd = {"MAIN": {"DEFAULT": {"align": align, "text_font": font, "text_color": color}}}
    if ta: sd["MAIN"]["DEFAULT"]["text_align"] = ta
    o["localStyles"] = lsty(sd)
    return o

def _bar(left, top, w, h, ident, minv=0, maxv=100, val=0, bg="#1A2530", ind="#00E0FF", bg_img=None, ind_img=None):
    o = w_base("LVGLBarWidget", left, top, w, h, ident)
    o["min"] = minv; o["max"] = maxv; o["value"] = val; o["mode"] = "NORMAL"; o["enableAnimation"] = False
    sd = {
        "MAIN": {"DEFAULT": {"align": "CENTER", "radius": 0, "bg_color": bg, "bg_opa": 255}},
        "INDICATOR": {"DEFAULT": {"radius": 0, "bg_color": ind, "bg_opa": 255}},
    }
    if bg_img: sd["MAIN"]["DEFAULT"]["bg_img_src"] = bg_img
    if ind_img: sd["INDICATOR"]["DEFAULT"]["bg_img_src"] = ind_img
    o["localStyles"] = lsty(sd)
    return o

def _img(left, top, w, h, img_name, ident, hidden=False):
    o = w_base("LVGLImageWidget", left, top, w, h, ident, hidden)
    o["image"] = img_name; o["sizeMode"] = "VIRTUAL"
    o["localStyles"] = lsty({"MAIN": {"DEFAULT": {"align": "CENTER"}}})
    return o

# Colors & Fonts
C = "#00E0FF"; W = "#FFFFFF"; D = "#C0C0C0"; R = "#FF0000"; BK = "#000000"
F1 = "ui_font_Font1"; FM = "ui_font_FontHUDmid"; FS = "ui_font_FontHUDsmall"
FL = "ui_font_FontLARGR"; F36 = "ui_font_FontHUD36"; F30 = "ui_font_FontHUD30"

def center_panel_ls():
    return lsty({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_opa": 0}}})

def dark_panel_ls():
    return lsty({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_color": BK, "bg_opa": 255}}})


# ======== Dashboard children ========
# Coordinate reference (CENTER-based, parent 765x256):
#   eez002 bars at x=-300 with 43px vertical spacing starting at y=-102
#   eez002 labels (values) at x=-570, RIGHT_MID aligned
#   eez002 gear at x=164, y=-6
#   eez002 time at x=-190, y=-113

def build_dashboard():
    kids = []

    # ===== LEFT: 6 sensor rows =====
    # Each row: icon(24x24) at x=-350 | bar(80x10) at x=-300 | value at x=-230 | unit at x=-165
    # Y positions: -102, -59, -16, 23, 62, 101 (same as eez002 bar positions)

    sensors = [
        # (y, icon_bitmap, bar_ident, val_ident, val_text, unit_ident, unit_text, icon_ident)
        (-102, "ui_img_ico_water_temp_png", "ui_BarWaterTemp", "ui_LblWaterTemp", "100", "ui_LblWaterTempUnit", "deg", "ui_IcoWaterTemp"),
        (-59,  "ui_img_ico_oil_temp_png",   "ui_BarOilTemp",   "ui_LblOilTemp",   "40",  "ui_LblOilTempUnit",  "deg", "ui_IcoOilTemp"),
        (-16,  "ui_img_ico_exhaust_temp_png","ui_BarExhaustTemp","ui_LblExhaustTemp","100","ui_LblExhaustTempUnit","deg","ui_IcoExhaustTemp"),
        (23,   "ui_img_ico_map_png",         "ui_BarMAP",       "ui_LblMAP",       "99",  "ui_LblMAPUnit",     "kPa", "ui_IcoMAP"),
        (62,   "ui_img_ico_fuel_press_png",  "ui_BarFuelPress", "ui_LblFuelPress", "4.0", "ui_LblFuelPressUnit","x100\nkPa","ui_IcoFuelPress"),
        (101,  "ui_img_ico_battery_png",     "ui_BarBattery",   "ui_LblBattery",   "14.5","ui_LblBatteryUnit", "v",   "ui_IcoBattery"),
    ]

    for y, ico_bmp, bar_id, val_id, val_txt, unit_id, unit_txt, ico_id in sensors:
        # Icon at x=-350 (far left)
        kids.append(_img(-350, y - 12, 24, 24, ico_bmp, ico_id))
        # Bar at x=-300 (just like eez002)
        kids.append(_bar(-300, y, 80, 10, bar_id))
        # Value label at x=-210, RIGHT_MID (like eez002's -570 but we're closer to center due to icon)
        kids.append(_lbl(-210, y - 5, 65, 28, val_txt, val_id, F1, W, align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
        # Unit label at x=-150
        kids.append(_lbl(-150, y - 5, 45, 28, unit_txt, unit_id, FS, D, align="CENTER"))

    # ===== CENTER: Fan Tachometer container =====
    # eez002 had ImgTacho at (27,0) 340x256 and ArcRPM at (-18,0) 240x240
    # We use a transparent panel for custom C drawing
    kids.append(_panel(0, -20, 320, 200, "ui_ContainerTacho", [],
                       lsty({"MAIN": {"DEFAULT": {"align": "CENTER", "border_width": 0, "radius": 0,
                                                   "bg_opa": 0, "pad_top": 0, "pad_bottom": 0,
                                                   "pad_left": 0, "pad_right": 0}}})))

    # Tachometer number background overlay
    kids.append(_img(0, -20, 320, 200, "ui_img_tacho_bg_png", "ui_ImgTachoBg"))

    # ===== GEAR (right of tachometer, like eez002 x=164) =====
    kids.append(_lbl(200, -6, 80, 82, "N", "ui_LblGear", FL, W, align="CENTER"))

    # ===== RPM value (below tachometer, like eez002 x=-300, y=31 but we put center) =====
    kids.append(_lbl(-40, 55, 120, 34, "3000", "ui_LblRPM", F30, W, align="CENTER", ta="CENTER"))

    # ===== TIME (top center, eez002: x=-190, y=-113) =====
    kids.append(_lbl(50, -113, 80, 22, "00:00", "ui_LblTime", FM, W, align="CENTER", ta="CENTER"))

    # ===== SPEED (right side, eez002: x=-65 y=36 meaning right area) =====
    kids.append(_lbl(290, 18, 120, 42, "180", "ui_LblSPD", F36, W, align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    kids.append(_lbl(330, 40, 50, 20, "km/h", "ui_LblSpdUnit", FS, D, align="CENTER"))

    # ===== AFR box (bottom center) =====
    kids.append(_img(-40, 82, 140, 36, "ui_img_afr_box_bg_png", "ui_ImgAfrBox"))
    kids.append(_lbl(-85, 82, 40, 32, "AFR", "ui_LblAfrName", FS, W, align="CENTER"))
    kids.append(_lbl(-10, 79, 80, 38, "14.7", "ui_LblAFR", F30, W, align="CENTER", ta="CENTER"))

    # ===== Trip / Total (bottom right) =====
    kids.append(_lbl(210, 68, 50, 18, "Trip:", "ui_LblTripName", FS, D, align="CENTER"))
    kids.append(_lbl(310, 68, 70, 18, "0.0", "ui_LblTrip", FS, W, align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    kids.append(_lbl(350, 68, 30, 18, "km", "ui_LblTripUnit", FS, D, align="CENTER"))

    kids.append(_lbl(200, 86, 60, 18, "Total:", "ui_LblOdoName", FS, D, align="CENTER"))
    kids.append(_lbl(310, 86, 70, 18, "999999", "ui_LblOdo", FS, W, align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    kids.append(_lbl(350, 86, 30, 18, "km", "ui_LblOdoUnit", FS, D, align="CENTER"))

    # ===== Fuel gauge (bottom right, E-F bar) =====
    kids.append(_lbl(215, 104, 14, 16, "E", "ui_LblFuelE", FS, D, align="CENTER"))
    kids.append(_bar(280, 107, 100, 15, "ui_BarFuel", val=70,
                     bg_img="ui_img_fuel2_png", ind_img="ui_img_fuel1_png"))
    kids.append(_lbl(345, 104, 14, 16, "F", "ui_LblFuelF", FS, D, align="CENTER"))

    return kids


def build_telltale():
    icons = [
        (-106, "ui_img_ws_masterwarning_png", "ui_ImgWarnMaster"),
        (-76, "ui_img_ws_oilpresswarning_png", "ui_ImgWarnOilPress"),
        (-46, "ui_img_ws_watarcool_png", "ui_ImgWarnWaterCold"),
        (-46, "ui_img_ws_waterwarning_png", "ui_ImgWarnWaterHot"),
        (-16, "ui_img_ws_exhaustwarning_png", "ui_ImgWarnExhaust"),
        (14, "ui_img_ws_batterywarning_png", "ui_ImgWarnBattery"),
        (44, "ui_img_ws_breakwarning_png", "ui_ImgWarnBrake"),
        (74, "ui_img_ws_beltwarning_png", "ui_ImgWarnBelt"),
        (104, "ui_img_ws_fuelcheck_png", "ui_ImgWarnFuel"),
    ]
    return [_img(0, y, 25, 25, im, ident) for y, im, ident in icons]


def build_screen_children():
    ch = []
    # Dashboard
    ch.append(_panel(18, 0, 765, 256, "ui_ContainerDashboard",
                     build_dashboard(), dark_panel_ls()))
    # Opening (hidden)
    ch.append(_panel(18, 0, 765, 256, "ui_ContainerOpening",
                     [_img(0, 0, 765, 256, "ui_img_fmw_op0_png", "ui_ImgOpening")],
                     dark_panel_ls(), hidden=True))
    # Telltale
    ch.append(_panel(-382, 0, 35, 256, "ui_ContainerTelltale",
                     build_telltale(), center_panel_ls()))
    # NotifyBox (hidden)
    ch.append(_panel(0, -100, 696, 50, "ui_NotifyBox",
                     [_lbl(0, 0, 680, 42, "", "ui_NotifyLabel", FL, W, align="CENTER", ta="CENTER")],
                     lsty({"MAIN": {"DEFAULT": {"align": "CENTER", "border_width": 0, "radius": 0,
                                                 "pad_top": 4, "pad_bottom": 4, "pad_left": 4, "pad_right": 4,
                                                 "bg_color": R, "bg_opa": 255}}}), hidden=True))
    return ch


def png_to_data_uri(path):
    with open(path, "rb") as f:
        return "data:image/png;base64," + base64.b64encode(f.read()).decode("ascii")


def main():
    with open(PROJ, "r", encoding="utf-8") as f:
        proj = json.load(f)

    proj["settings"]["general"]["description"] = "FULLMONI-WIDE Prodrive Racing Dashboard"

    # Add new bitmaps if not present
    new_imgs = {
        "ui_img_ico_water_temp_png": ("ico_water_temp.png", 32),
        "ui_img_ico_oil_temp_png": ("ico_oil_temp.png", 32),
        "ui_img_ico_exhaust_temp_png": ("ico_exhaust_temp.png", 32),
        "ui_img_ico_map_png": ("ico_map.png", 32),
        "ui_img_ico_fuel_press_png": ("ico_fuel_press.png", 32),
        "ui_img_ico_battery_png": ("ico_battery.png", 32),
        "ui_img_tacho_bg_png": ("tacho_bg.png", 32),
        "ui_img_afr_box_bg_png": ("afr_box_bg.png", 32),
    }
    existing = {b["name"] for b in proj.get("bitmaps", [])}
    for name, (fn, bpp) in new_imgs.items():
        if name not in existing:
            path = os.path.join(ASSET_DIR, fn)
            if os.path.exists(path):
                proj["bitmaps"].append({
                    "objID": uid(), "name": name,
                    "image": png_to_data_uri(path),
                    "bpp": bpp, "lvglBinaryOutputFormat": 4, "lvglDither": False,
                })
                print(f"  +bitmap: {name}")

    # Replace screen children
    screen = proj["userPages"][0]["components"][0]
    screen["localStyles"] = lsty({"MAIN": {"DEFAULT": {"bg_color": BK, "bg_opa": 255}}})
    screen["children"] = build_screen_children()

    with open(PROJ, "w", encoding="utf-8", newline="\n") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)

    def cnt(n):
        return 1 + sum(cnt(c) for c in n.get("children", []))
    total = sum(cnt(c) for c in screen["children"])
    print(f"Total widgets: {total + 1}")
    print(f"Wrote: {PROJ}")


if __name__ == "__main__":
    main()
