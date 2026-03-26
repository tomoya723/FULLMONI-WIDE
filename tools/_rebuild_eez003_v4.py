"""Rebuild eez003.eez-project v4 - Correct coordinate system.

Coordinate system for ContainerDashboard (765x256):
  CENTER align: offset from (382.5, 128)
    absolute_x = 18 + 382.5 + left - width/2
    absolute_y = 128 + top - height/2
  RIGHT_MID align: offset from (765, 128)
    absolute_right = 18 + 765 + left
    absolute_x = absolute_right - width

Target absolute pixel positions (800x256 screen):
  Icons:      x=22-46   (24px wide at far left)
  Bars:       x=50-140  (90px wide)
  Values:     x=148-213 (65px wide, same as eez002)
  Units:      x=218-260 (small text)
  Tachometer: x=258-598 (center, 340px, same as eez002)
  Gear:       x=544-586 (right of tachometer)
  RPM:        x=360-480 (below tachometer center)
  Time:       x=538-593 (top right of tachometer)
  Speed:      x=618-718 (right side)
  AFR:        x=440-580 (bottom center)
  Trip/Total: x=620-750 (bottom right)
  Fuel E-F:   x=620-740 (very bottom right)
"""
import json, uuid, base64, os

PROJ = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003\eez003.eez-project"
ASSET_DIR = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003"

# Container constants
CONT_X = 18     # container x on screen
PW = 765        # parent width
PH = 256        # parent height
CX = PW / 2     # 382.5 - center x within container
CY = PH / 2     # 128 - center y within container

def uid(): return str(uuid.uuid4())
def style_obj(): return {"objID": uid(), "useStyle": "default", "conditionalStyles": [], "childStyles": []}
def lsty(d): return {"objID": uid(), "definition": d}

# --- Coordinate converters: absolute screen pixel -> EEZ offset ---

def abs_to_center(abs_x, abs_y, w, h):
    """Convert absolute screen coords to CENTER align offsets."""
    # Center of widget in container coords
    widget_cx = (abs_x - CONT_X) + w / 2
    widget_cy = abs_y + h / 2
    left = widget_cx - CX
    top = widget_cy - CY
    return round(left), round(top)

def abs_to_right_mid(abs_right, abs_y, w, h):
    """Convert absolute right edge + y to RIGHT_MID align offsets."""
    right_in_cont = abs_right - CONT_X
    left = right_in_cont - PW
    top = (abs_y + h / 2) - CY
    return round(left), round(top)

# --- Widget builders ---

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
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0, "bg_opa": 0}}})

def dark_panel_ls():
    return lsty({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_color": BK, "bg_opa": 255}}})


def build_dashboard():
    kids = []

    # ===== LEFT: 6 sensor rows =====
    # Absolute Y positions for each row (matching eez002 spacing):
    # Row 1: y≈21-49  (water temp)
    # Row 2: y≈61-89  (oil temp)
    # Row 3: y≈103-131 (exhaust temp)
    # Row 4: y≈143-171 (MAP)
    # Row 5: y≈180-208 (fuel press)
    # Row 6: y≈217-245 (battery)

    sensor_rows = [
        # (abs_y_center, icon_bmp, bar_id, val_id, val_text, unit_id, unit_text, icon_id)
        (26,  "ui_img_ico_water_temp_png", "ui_BarWaterTemp",  "ui_LblWaterTemp",  "100",  "ui_LblWaterTempUnit",  "deg",         "ui_IcoWaterTemp"),
        (69,  "ui_img_ico_oil_temp_png",   "ui_BarOilTemp",    "ui_LblOilTemp",    "40",   "ui_LblOilTempUnit",    "deg",         "ui_IcoOilTemp"),
        (112, "ui_img_ico_exhaust_temp_png","ui_BarExhaustTemp","ui_LblExhaustTemp","100",  "ui_LblExhaustTempUnit","deg",         "ui_IcoExhaustTemp"),
        (151, "ui_img_ico_map_png",         "ui_BarMAP",       "ui_LblMAP",        "99",   "ui_LblMAPUnit",        "kPa",         "ui_IcoMAP"),
        (190, "ui_img_ico_fuel_press_png",  "ui_BarFuelPress", "ui_LblFuelPress",  "4.0",  "ui_LblFuelPressUnit",  "x100\nkPa",   "ui_IcoFuelPress"),
        (229, "ui_img_ico_battery_png",     "ui_BarBattery",   "ui_LblBattery",    "14.5", "ui_LblBatteryUnit",    "v",           "ui_IcoBattery"),
    ]

    for abs_yc, ico_bmp, bar_id, val_id, val_text, unit_id, unit_text, ico_id in sensor_rows:
        # Icon: 24x24 at absolute x=22, centered on row y
        ico_l, ico_t = abs_to_center(22, abs_yc - 12, 24, 24)
        kids.append(_img(ico_l, ico_t, 24, 24, ico_bmp, ico_id))

        # Bar: 80x10 at absolute x=55
        bar_l, bar_t = abs_to_center(55, abs_yc - 5, 80, 10)
        kids.append(_bar(bar_l, bar_t, 80, 10, bar_id))

        # Value: 65x28 right-aligned, right edge at absolute x=213 (same as eez002)
        val_l, val_t = abs_to_right_mid(213, abs_yc - 14, 65, 28)
        kids.append(_lbl(val_l, val_t, 65, 28, val_text, val_id, F1, W,
                         align="RIGHT_MID", ta="RIGHT", lm="CLIP"))

        # Unit: ~42x28 at absolute x=218
        unit_l, unit_t = abs_to_center(218, abs_yc - 10, 42, 20)
        kids.append(_lbl(unit_l, unit_t, 42, 20, unit_text, unit_id, FS, D))

    # ===== CENTER: Fan Tachometer container =====
    # Same area as eez002 ImgTacho: absolute x=258-598, full height
    tacho_l, tacho_t = abs_to_center(258, 0, 340, 256)
    kids.append(_panel(tacho_l, tacho_t, 340, 256, "ui_ContainerTacho", [],
                       lsty({"MAIN": {"DEFAULT": {"align": "CENTER", "border_width": 0, "radius": 0,
                                                   "bg_opa": 0, "pad_top": 0, "pad_bottom": 0,
                                                   "pad_left": 0, "pad_right": 0}}})))

    # Tachometer background overlay (numbers)
    kids.append(_img(tacho_l, tacho_t, 340, 256, "ui_img_tacho_bg_png", "ui_ImgTachoBg"))

    # ===== GEAR (right of tachometer): abs x≈544-586, y=86-158 =====
    gear_l, gear_t = abs_to_center(544, 62, 60, 82)
    kids.append(_lbl(gear_l, gear_t, 60, 82, "N", "ui_LblGear", FL, W))

    # ===== RPM: abs x≈370-480, y=158-186 =====
    rpm_l, rpm_t = abs_to_right_mid(483, 152, 110, 34)
    kids.append(_lbl(rpm_l, rpm_t, 110, 34, "3000", "ui_LblRPM", F30, W,
                     align="RIGHT_MID", ta="RIGHT"))

    # ===== TIME: abs x≈538-593, y=6-24 =====
    time_l, time_t = abs_to_right_mid(593, 6, 70, 20)
    kids.append(_lbl(time_l, time_t, 70, 20, "00:00", "ui_LblTime", FM, W,
                     align="RIGHT_MID", ta="RIGHT"))

    # ===== SPEED: abs x≈620-720, y=158-194 =====
    spd_l, spd_t = abs_to_right_mid(718, 155, 100, 42)
    kids.append(_lbl(spd_l, spd_t, 100, 42, "180", "ui_LblSPD", F36, W,
                     align="RIGHT_MID", ta="RIGHT", lm="CLIP"))

    # km/h unit: abs x≈720-760, y=168-186
    kmh_l, kmh_t = abs_to_center(720, 168, 45, 20)
    kids.append(_lbl(kmh_l, kmh_t, 45, 20, "km/h", "ui_LblSpdUnit", FS, D))

    # ===== AFR box: abs x≈380-520, y=198-234 =====
    afr_box_l, afr_box_t = abs_to_center(390, 200, 140, 36)
    kids.append(_img(afr_box_l, afr_box_t, 140, 36, "ui_img_afr_box_bg_png", "ui_ImgAfrBox"))

    # AFR label: abs x≈392-430
    afr_name_l, afr_name_t = abs_to_center(395, 206, 40, 24)
    kids.append(_lbl(afr_name_l, afr_name_t, 40, 24, "AFR", "ui_LblAfrName", FS, W))

    # AFR value: abs x≈440-520
    afr_val_l, afr_val_t = abs_to_right_mid(525, 200, 80, 36)
    kids.append(_lbl(afr_val_l, afr_val_t, 80, 36, "14.7", "ui_LblAFR", F30, W,
                     align="RIGHT_MID", ta="RIGHT"))

    # ===== Trip: abs x≈620-750, y=198-214 =====
    trip_name_l, trip_name_t = abs_to_center(618, 198, 48, 18)
    kids.append(_lbl(trip_name_l, trip_name_t, 48, 18, "Trip:", "ui_LblTripName", FS, D))
    trip_val_l, trip_val_t = abs_to_right_mid(728, 198, 60, 18)
    kids.append(_lbl(trip_val_l, trip_val_t, 60, 18, "0.0", "ui_LblTrip", FS, W,
                     align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    trip_unit_l, trip_unit_t = abs_to_center(730, 198, 30, 18)
    kids.append(_lbl(trip_unit_l, trip_unit_t, 30, 18, "km", "ui_LblTripUnit", FS, D))

    # ===== Total: abs x≈620-750, y=216-232 =====
    odo_name_l, odo_name_t = abs_to_center(610, 216, 55, 18)
    kids.append(_lbl(odo_name_l, odo_name_t, 55, 18, "Total:", "ui_LblOdoName", FS, D))
    odo_val_l, odo_val_t = abs_to_right_mid(728, 216, 60, 18)
    kids.append(_lbl(odo_val_l, odo_val_t, 60, 18, "999999", "ui_LblOdo", FS, W,
                     align="RIGHT_MID", ta="RIGHT", lm="CLIP"))
    odo_unit_l, odo_unit_t = abs_to_center(730, 216, 30, 18)
    kids.append(_lbl(odo_unit_l, odo_unit_t, 30, 18, "km", "ui_LblOdoUnit", FS, D))

    # ===== Fuel gauge E-F: abs x≈625-740, y=236-252 =====
    fuel_e_l, fuel_e_t = abs_to_center(622, 238, 14, 16)
    kids.append(_lbl(fuel_e_l, fuel_e_t, 14, 16, "E", "ui_LblFuelE", FS, D))

    fuel_bar_l, fuel_bar_t = abs_to_center(640, 237, 80, 15)
    kids.append(_bar(fuel_bar_l, fuel_bar_t, 80, 15, "ui_BarFuel", val=70,
                     bg_img="ui_img_fuel2_png", ind_img="ui_img_fuel1_png"))

    fuel_f_l, fuel_f_t = abs_to_center(724, 238, 14, 16)
    kids.append(_lbl(fuel_f_l, fuel_f_t, 14, 16, "F", "ui_LblFuelF", FS, D))

    # Fuel pump icon next to F
    fuel_ico_l, fuel_ico_t = abs_to_center(740, 236, 20, 20)
    kids.append(_img(fuel_ico_l, fuel_ico_t, 20, 20, "ui_img_ws_fuelcheck_png", "ui_IcoFuelGauge"))

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
    return [
        _panel(18, 0, 765, 256, "ui_ContainerDashboard", build_dashboard(), dark_panel_ls()),
        _panel(18, 0, 765, 256, "ui_ContainerOpening",
               [_img(0, 0, 765, 256, "ui_img_fmw_op0_png", "ui_ImgOpening")],
               dark_panel_ls(), hidden=True),
        _panel(-382, 0, 35, 256, "ui_ContainerTelltale", build_telltale(), center_panel_ls()),
        _panel(0, -100, 696, 50, "ui_NotifyBox",
               [_lbl(0, 0, 680, 42, "", "ui_NotifyLabel", FL, W, align="CENTER", ta="CENTER")],
               lsty({"MAIN": {"DEFAULT": {"align": "CENTER", "border_width": 0, "radius": 0,
                                           "pad_top": 4, "pad_bottom": 4, "pad_left": 4, "pad_right": 4,
                                           "bg_color": R, "bg_opa": 255}}}), hidden=True),
    ]


def png_to_data_uri(path):
    with open(path, "rb") as f:
        return "data:image/png;base64," + base64.b64encode(f.read()).decode("ascii")


def main():
    with open(PROJ, "r", encoding="utf-8") as f:
        proj = json.load(f)

    proj["settings"]["general"]["description"] = "FULLMONI-WIDE Prodrive Racing Dashboard"

    # Ensure new bitmaps exist
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

    screen = proj["userPages"][0]["components"][0]
    screen["localStyles"] = lsty({"MAIN": {"DEFAULT": {"bg_color": BK, "bg_opa": 255}}})
    screen["children"] = build_screen_children()

    with open(PROJ, "w", encoding="utf-8", newline="\n") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)

    def cnt(n): return 1 + sum(cnt(c) for c in n.get("children", []))
    total = sum(cnt(c) for c in screen["children"])
    print(f"Total widgets: {total + 1}")
    print(f"Wrote: {PROJ}")

    # Verify key positions
    print("\n=== Verify absolute positions ===")
    for child in screen["children"]:
        if child.get("identifier") == "ui_ContainerDashboard":
            for w in child.get("children", []):
                ident = w.get("identifier", "?")
                l, t, ww, hh = w["left"], w["top"], w["width"], w["height"]
                ls = w.get("localStyles", {})
                defn = ls.get("definition", {})
                align = defn.get("MAIN", {}).get("DEFAULT", {}).get("align", "?")
                if align == "CENTER":
                    ax = CONT_X + CX + l - ww/2
                    ay = CY + t - hh/2
                elif align == "RIGHT_MID":
                    ax = CONT_X + PW + l - ww
                    ay = CY + t - hh/2
                else:
                    ax = ay = 0
                print(f"  {ident:<24} abs=({ax:.0f},{ay:.0f}) {ww}x{hh} align={align}")


if __name__ == "__main__":
    main()
