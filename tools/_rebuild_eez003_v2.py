"""Rebuild eez003.eez-project to match original Prodrive reference image.

Layout (800x256):
  LEFT (x:18-180):  6 sensor rows, each = icon + bar + value + unit
  CENTER (x:180-530): Fan tachometer container (custom C draw) + tacho_bg image
  Tacho RIGHT:      Gear letter (large)
  Tacho BOTTOM:     RPM number
  TOP CENTER:       Time "00:00"
  RIGHT (x:560-760): Speed (large) + km/h unit
  BOTTOM CENTER:    AFR box with border + value
  BOTTOM RIGHT:     Trip / Total + Fuel gauge (E-F)
  LEFT STRIP:       Telltale warning icons (unchanged)
  NOTIFY:           Red notification overlay (unchanged)
"""
import json
import uuid
import base64
import os

PROJ = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003\eez003.eez-project"
ASSET_DIR = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003"


def uid():
    return str(uuid.uuid4())


def style_obj():
    return {"objID": uid(), "useStyle": "default", "conditionalStyles": [], "childStyles": []}


def local_styles(definition):
    return {"objID": uid(), "definition": definition}


def base(wtype, left, top, width, height, identifier=None, hidden=False, children=None):
    w = {
        "objID": uid(), "type": wtype,
        "left": left, "top": top, "width": width, "height": height,
        "customInputs": [], "customOutputs": [],
        "style": style_obj(), "timeline": [], "eventHandlers": [],
        "leftUnit": "px", "topUnit": "px", "widthUnit": "px", "heightUnit": "px",
    }
    if identifier:
        w["identifier"] = identifier
    if hidden:
        w["hiddenFlag"] = "HIDDEN"
    if children is not None:
        w["children"] = children
    elif wtype == "LVGLPanelWidget":
        w["children"] = []
    if wtype == "LVGLPanelWidget":
        w["hiddenInEditor"] = False
    return w


# ---- Widget helpers ----

def panel(left, top, w, h, ident, children, lstyle=None, hidden=False):
    obj = base("LVGLPanelWidget", left, top, w, h, ident, hidden, children)
    if lstyle:
        obj["localStyles"] = lstyle
    return obj


def lbl(left, top, w, h, text, ident, font, color, align="CENTER", ta=None, hidden=False, lm=None):
    obj = base("LVGLLabelWidget", left, top, w, h, ident, hidden)
    obj["text"] = text
    if lm:
        obj["longMode"] = lm
    sd = {"MAIN": {"DEFAULT": {"align": align, "text_font": font, "text_color": color}}}
    if ta:
        sd["MAIN"]["DEFAULT"]["text_align"] = ta
    obj["localStyles"] = local_styles(sd)
    return obj


def bar_w(left, top, w, h, ident, minv=0, maxv=100, val=0,
          bg="#1A2530", ind="#00E0FF", bg_img=None, ind_img=None):
    obj = base("LVGLBarWidget", left, top, w, h, ident)
    obj["min"] = minv
    obj["max"] = maxv
    obj["value"] = val
    obj["mode"] = "NORMAL"
    obj["enableAnimation"] = False
    sd = {
        "MAIN": {"DEFAULT": {"align": "CENTER", "radius": 0, "bg_color": bg, "bg_opa": 255}},
        "INDICATOR": {"DEFAULT": {"radius": 0, "bg_color": ind, "bg_opa": 255}},
    }
    if bg_img:
        sd["MAIN"]["DEFAULT"]["bg_img_src"] = bg_img
    if ind_img:
        sd["INDICATOR"]["DEFAULT"]["bg_img_src"] = ind_img
    obj["localStyles"] = local_styles(sd)
    return obj


def img_w(left, top, w, h, img_name, ident, hidden=False):
    obj = base("LVGLImageWidget", left, top, w, h, ident, hidden)
    obj["image"] = img_name
    obj["sizeMode"] = "VIRTUAL"
    obj["localStyles"] = local_styles({"MAIN": {"DEFAULT": {"align": "CENTER"}}})
    return obj


# ---- Colors & Fonts ----
C = "#00E0FF"   # cyan
W = "#FFFFFF"   # white
D = "#C0C0C0"   # dim gray
R = "#FF0000"   # red
BK = "#000000"  # black
BG = "#1A2530"  # bar background

F1 = "ui_font_Font1"
FM = "ui_font_FontHUDmid"
FS = "ui_font_FontHUDsmall"
FL = "ui_font_FontLARGR"
F36 = "ui_font_FontHUD36"
F30 = "ui_font_FontHUD30"


def transparent_panel(left, top, w, h, ident, children):
    return panel(left, top, w, h, ident, children,
                 local_styles({"MAIN": {"DEFAULT": {
                     "align": "CENTER", "border_width": 0, "radius": 0, "bg_opa": 0,
                     "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0}}}))


def dark_panel(left, top, w, h, ident, children, hidden=False):
    return panel(left, top, w, h, ident, children,
                 local_styles({"MAIN": {"DEFAULT": {
                     "align": "CENTER", "border_width": 0, "radius": 0,
                     "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
                     "bg_color": BK, "bg_opa": 255}}}), hidden)


# ===== Sensor row builder =====
# Each row: icon(24x24) + bar(80x8) + value + unit text
# Positioned RELATIVE to ContainerDashboard (765x256), align=CENTER

def sensor_row(y_center, icon_img, bar_ident, val_ident, val_text, unit_text, unit_ident, icon_ident):
    """Build one sensor row. All x relative to dashboard center (align=CENTER)."""
    kids = []
    # Icon at far left of dashboard
    kids.append(img_w(-350, y_center - 12, 24, 24, icon_img, icon_ident))
    # Bar gauge
    kids.append(bar_w(-270, y_center - 4, 80, 8, bar_ident))
    # Value (right-aligned)
    kids.append(lbl(-180, y_center - 12, 55, 28, val_text, val_ident, F1, W,
                    align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    # Unit label
    kids.append(lbl(-130, y_center - 8, 40, 20, unit_text, unit_ident, FS, D))
    return kids


# ===== Build all Screen1 children =====

def build_dashboard():
    kids = []

    # === 6 sensor rows on the LEFT ===
    # Row spacing ~32px, starting y=-96 (top area of 256px dashboard)
    rows = [
        (-96, "ui_img_ico_water_temp_png", "ui_BarWaterTemp", "ui_LblWaterTemp", "100", "deg", "ui_LblWaterTempUnit", "ui_IcoWaterTemp"),
        (-64, "ui_img_ico_oil_temp_png", "ui_BarOilTemp", "ui_LblOilTemp", "40", "deg", "ui_LblOilTempUnit", "ui_IcoOilTemp"),
        (-32, "ui_img_ico_exhaust_temp_png", "ui_BarExhaustTemp", "ui_LblExhaustTemp", "100", "deg", "ui_LblExhaustTempUnit", "ui_IcoExhaustTemp"),
        (0,   "ui_img_ico_map_png", "ui_BarMAP", "ui_LblMAP", "99", "kPa", "ui_LblMAPUnit", "ui_IcoMAP"),
        (32,  "ui_img_ico_fuel_press_png", "ui_BarFuelPress", "ui_LblFuelPress", "4.0", "x100\nkPa", "ui_LblFuelPressUnit", "ui_IcoFuelPress"),
        (64,  "ui_img_ico_battery_png", "ui_BarBattery", "ui_LblBattery", "14.5", "v", "ui_LblBatteryUnit", "ui_IcoBattery"),
    ]
    for r in rows:
        kids.extend(sensor_row(*r))

    # === Fan Tachometer container (center, custom drawn in C) ===
    kids.append(transparent_panel(-20, -48, 320, 155, "ui_ContainerTacho", []))

    # Tachometer background image (numbers 0-9 ring)
    kids.append(img_w(-20, -48, 320, 155, "ui_img_tacho_bg_png", "ui_ImgTachoBg"))

    # === Gear display (right of tachometer) ===
    kids.append(lbl(200, -20, 80, 82, "N", "ui_LblGear", FL, W))

    # === RPM number (below tachometer center) ===
    kids.append(lbl(-20, 65, 120, 38, "3000", "ui_LblRPM", F30, W,
                    align="CENTER", ta="CENTER"))

    # === Time display (top center) ===
    kids.append(lbl(60, -120, 80, 24, "00:00", "ui_LblTime", FM, W,
                    align="CENTER", ta="CENTER"))

    # === Speed display (right side, large) ===
    kids.append(lbl(250, 20, 120, 46, "180", "ui_LblSPD", F36, W,
                    align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    kids.append(lbl(310, 50, 50, 20, "km/h", "ui_LblSpdUnit", FS, D))

    # === AFR box (bottom center) ===
    kids.append(img_w(-20, 88, 140, 36, "ui_img_afr_box_bg_png", "ui_ImgAfrBox"))
    kids.append(lbl(-65, 82, 40, 24, "AFR", "ui_LblAfrName", FS, W))
    kids.append(lbl(10, 80, 70, 36, "14.7", "ui_LblAFR", F30, W,
                    align="CENTER", ta="CENTER"))

    # === Trip / Total (bottom right) ===
    kids.append(lbl(220, 75, 50, 18, "Trip:", "ui_LblTripName", FS, D))
    kids.append(lbl(280, 75, 80, 18, "0.0", "ui_LblTrip", FS, W,
                    align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    kids.append(lbl(330, 75, 30, 18, "km", "ui_LblTripUnit", FS, D))
    kids.append(lbl(200, 95, 70, 18, "Total:", "ui_LblOdoName", FS, D))
    kids.append(lbl(280, 95, 80, 18, "999999", "ui_LblOdo", FS, W,
                    align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    kids.append(lbl(330, 95, 30, 18, "km", "ui_LblOdoUnit", FS, D))

    # === Fuel gauge (bottom right, horizontal E-F) ===
    kids.append(lbl(225, 112, 14, 16, "E", "ui_LblFuelE", FS, D))
    kids.append(bar_w(250, 115, 80, 10, "ui_BarFuel", val=70,
                      bg_img="ui_img_fuel2_png", ind_img="ui_img_fuel1_png"))
    kids.append(lbl(340, 112, 14, 16, "F", "ui_LblFuelF", FS, D))
    # Fuel pump icon
    kids.append(img_w(350, 112, 16, 16, "ui_img_ws_fuelcheck_png", "ui_IcoFuelGauge"))

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
    return [img_w(0, y, 24, 24, im, ident) for y, im, ident in icons]


def build_screen_children():
    ch = []
    # 1. Dashboard
    ch.append(dark_panel(18, 0, 765, 256, "ui_ContainerDashboard", build_dashboard()))
    # 2. Opening (hidden)
    ch.append(dark_panel(18, 0, 765, 256, "ui_ContainerOpening",
                         [img_w(0, 0, 765, 256, "ui_img_fmw_op0_png", "ui_ImgOpening")], hidden=True))
    # 3. Telltale strip
    ch.append(transparent_panel(-382, 0, 35, 256, "ui_ContainerTelltale", build_telltale()))
    # 4. Notify box (hidden)
    notify_lbl = lbl(0, 0, 696, 42, "", "ui_NotifyLabel", FL, W, align="CENTER", ta="CENTER")
    ch.append(panel(0, -100, 696, 50, "ui_NotifyBox",
                    [notify_lbl],
                    local_styles({"MAIN": {"DEFAULT": {
                        "align": "CENTER", "border_width": 0, "radius": 0,
                        "pad_top": 4, "pad_bottom": 4, "pad_left": 4, "pad_right": 4,
                        "bg_color": R, "bg_opa": 255}}}), hidden=True))
    return ch


def png_to_data_uri(path):
    with open(path, "rb") as f:
        data = f.read()
    b64 = base64.b64encode(data).decode("ascii")
    return f"data:image/png;base64,{b64}"


def main():
    with open(PROJ, "r", encoding="utf-8") as f:
        proj = json.load(f)

    # Update description
    proj["settings"]["general"]["description"] = "FULLMONI-WIDE Prodrive Racing Dashboard"

    # --- Add new bitmap entries for generated icons ---
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

    # Remove any old entries with these names
    existing_names = {b["name"] for b in proj.get("bitmaps", [])}
    for name, (filename, bpp) in new_imgs.items():
        if name not in existing_names:
            png_path = os.path.join(ASSET_DIR, filename)
            if os.path.exists(png_path):
                proj["bitmaps"].append({
                    "objID": uid(),
                    "name": name,
                    "image": png_to_data_uri(png_path),
                    "bpp": bpp,
                    "lvglBinaryOutputFormat": 4,
                    "lvglDither": False,
                })
                print(f"  Added bitmap: {name} <- {filename}")
            else:
                print(f"  WARNING: {png_path} not found!")

    # --- Replace screen children ---
    screen = proj["userPages"][0]["components"][0]
    screen["localStyles"] = local_styles({"MAIN": {"DEFAULT": {"bg_color": BK, "bg_opa": 255}}})
    screen["children"] = build_screen_children()

    with open(PROJ, "w", encoding="utf-8", newline="\n") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)

    # Count
    def cnt(n):
        c = 1
        for ch in n.get("children", []):
            c += cnt(ch)
        return c
    total = sum(cnt(c) for c in screen["children"])
    print(f"Total widgets: {total + 1}")
    print(f"Wrote: {PROJ}")


if __name__ == "__main__":
    main()
