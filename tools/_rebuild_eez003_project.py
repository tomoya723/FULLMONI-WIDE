"""Rebuild eez003.eez-project widget tree to match Prodrive design (screens.c)."""
import json
import uuid
import sys

def uid():
    return str(uuid.uuid4())

def style_obj():
    return {
        "objID": uid(),
        "useStyle": "default",
        "conditionalStyles": [],
        "childStyles": []
    }

def local_styles(definition):
    return {
        "objID": uid(),
        "definition": definition
    }

def base_widget(wtype, left, top, width, height, identifier=None, hidden=False, children=None):
    w = {
        "objID": uid(),
        "type": wtype,
        "left": left,
        "top": top,
        "width": width,
        "height": height,
        "customInputs": [],
        "customOutputs": [],
        "style": style_obj(),
        "timeline": [],
        "eventHandlers": [],
        "leftUnit": "px",
        "topUnit": "px",
        "widthUnit": "px",
        "heightUnit": "px",
    }
    if identifier:
        w["identifier"] = identifier
    if hidden:
        w["hiddenFlag"] = "HIDDEN"
    if children:
        w["children"] = children
    else:
        if wtype in ("LVGLPanelWidget",):
            w["children"] = []
    if wtype == "LVGLPanelWidget":
        w["hiddenInEditor"] = False
    return w

def panel(left, top, width, height, identifier, children, local=None, hidden=False):
    w = base_widget("LVGLPanelWidget", left, top, width, height, identifier, hidden, children)
    if local:
        w["localStyles"] = local
    return w

FONT_HEIGHTS = {
    "ui_font_Font1": 30,
    "ui_font_FontHUDmid": 22,
    "ui_font_FontHUDsmall": 20,
    "ui_font_FontLARGR": 78,
    "ui_font_FontHUD36": 42,
    "ui_font_FontHUD30": 34,
}

def label(left, top, text, identifier, font, color, align="CENTER", text_align=None, width=None, hidden=False, long_mode=None):
    fh = FONT_HEIGHTS.get(font, 24)
    # Estimate width from text length and font height ratio
    char_w = int(fh * 0.6) + 1
    est_w = max(len(text) * char_w, char_w * 2)
    lbl_w = width if width else est_w
    lbl_h = fh + 4
    w = base_widget("LVGLLabelWidget", left, top, lbl_w, lbl_h, identifier, hidden)
    w["text"] = text
    if long_mode:
        w["longMode"] = long_mode
    sdef = {"MAIN": {"DEFAULT": {"align": align, "text_font": font, "text_color": color}}}
    if text_align:
        sdef["MAIN"]["DEFAULT"]["text_align"] = text_align
    if width:
        sdef["MAIN"]["DEFAULT"]["width"] = width
    w["localStyles"] = local_styles(sdef)
    return w

def bar(left, top, width, height, identifier, min_v=0, max_v=100, value=0,
        bg_color="#1A2530", ind_color="#00E0FF", bg_img=None, ind_img=None):
    w = base_widget("LVGLBarWidget", left, top, width, height, identifier)
    w["min"] = min_v
    w["max"] = max_v
    w["value"] = value
    w["mode"] = "NORMAL"
    w["enableAnimation"] = False
    sdef = {
        "MAIN": {"DEFAULT": {"align": "CENTER", "radius": 0, "bg_color": bg_color, "bg_opa": 255}},
        "INDICATOR": {"DEFAULT": {"radius": 0, "bg_color": ind_color, "bg_opa": 255}}
    }
    if bg_img:
        sdef["MAIN"]["DEFAULT"]["bg_img_src"] = bg_img
    if ind_img:
        sdef["INDICATOR"]["DEFAULT"]["bg_img_src"] = ind_img
    w["localStyles"] = local_styles(sdef)
    return w

def image(left, top, img_name, identifier, hidden=False):
    w = base_widget("LVGLImageWidget", left, top, 0, 0, identifier, hidden)
    w["image"] = img_name
    w["sizeMode"] = "VIRTUAL"
    sdef = {"MAIN": {"DEFAULT": {"align": "CENTER"}}}
    w["localStyles"] = local_styles(sdef)
    return w

# === Color constants ===
C_WHT = "#FFFFFF"
C_CYAN = "#00E0FF"
C_RED = "#FF0000"
C_DIM = "#C0C0C0"
C_BLK = "#000000"
C_BARBG = "#1A2530"

# === Font names ===
F1 = "ui_font_Font1"
FMID = "ui_font_FontHUDmid"
FSMALL = "ui_font_FontHUDsmall"
FLARGE = "ui_font_FontLARGR"
F36 = "ui_font_FontHUD36"
F30 = "ui_font_FontHUD30"

# === Value label helper (right-aligned) ===
def val_label(left, top, text, identifier, font, width_px, color=C_WHT):
    return label(left, top, text, identifier, font, color,
                 align="RIGHT_MID", text_align="RIGHT", width=width_px, long_mode="CLIP")

# === Name label helper (small dim) ===
def name_label(left, top, text, identifier):
    return label(left, top, text, identifier, FSMALL, C_DIM)

# === Build Prodrive widget tree ===
def build_dashboard_children():
    kids = []

    # Fan Tachometer container (custom drawn in C code)
    kids.append(panel(20, -38, 320, 155, "ui_ContainerTacho", [],
                      local_styles({"MAIN": {"DEFAULT": {"align": "CENTER", "border_width": 0, "radius": 0, "bg_opa": 0,
                                                         "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0}}})))

    # Boost labels
    kids.append(label(-130, -115, "BOOST", "ui_LblBoost", FSMALL, C_CYAN))
    kids.append(val_label(-50, -115, " 0.00", "ui_LblBoostVal", FMID, 85))

    # Engine RPM (below tachometer)
    kids.append(val_label(20, 48, "0", "ui_LblEngineRPM", F30, 100))

    # LEFT COLUMN: 3 bar gauges
    kids.append(bar(-250, -86, 80, 8, "ui_BarWaterTemp"))
    kids.append(val_label(-180, -86, "0", "ui_LblWaterTemp", F1, 55))
    kids.append(name_label(-325, -86, "WATER", "ui_LblWaterTempName"))

    kids.append(bar(-250, -54, 80, 8, "ui_BarOilTemp"))
    kids.append(val_label(-180, -54, "0", "ui_LblOilTemp", F1, 55))
    kids.append(name_label(-325, -54, "OIL T", "ui_LblOilTempName"))

    kids.append(bar(-250, -22, 80, 8, "ui_BarChargeTemp"))
    kids.append(val_label(-180, -22, "0", "ui_LblChargeTemp", F1, 55))
    kids.append(name_label(-325, -22, "CHG T", "ui_LblChargeTempName"))

    # RIGHT COLUMN: 3 bar gauges
    kids.append(bar(250, -86, 80, 8, "ui_BarOilPress"))
    kids.append(val_label(310, -86, "0.0", "ui_LblOilPress", F1, 55))
    kids.append(name_label(175, -86, "OIL P", "ui_LblOilPressName"))

    kids.append(bar(250, -54, 80, 8, "ui_BarFuelPress"))
    kids.append(val_label(310, -54, "0", "ui_LblFuelPress", F1, 55))
    kids.append(name_label(175, -54, "FUEL P", "ui_LblFuelPressName"))

    kids.append(bar(250, -22, 80, 8, "ui_BarBattery"))
    kids.append(val_label(310, -22, "0.0", "ui_LblBattery", F1, 55))
    kids.append(name_label(175, -22, "BATT", "ui_LblBatteryName"))

    # CENTER: gear, speed, ALS
    kids.append(label(25, -18, "N", "ui_LblGear", FLARGE, C_WHT))
    kids.append(label(-30, 18, "ALS", "ui_LblALS", FSMALL, C_CYAN, hidden=True))
    kids.append(val_label(-15, 70, "0", "ui_LblSPD", F36, 100))
    kids.append(label(65, 82, "km/h", "ui_LblSpdUnit", FSMALL, C_DIM))
    w = val_label(-15, 70, "0", "ui_LblRoadSpeed", F36, 100)
    w["hiddenFlag"] = "HIDDEN"
    kids.append(w)
    w = label(20, 100, "P BRAKE", "ui_LblParking", FSMALL, C_RED, hidden=True)
    kids.append(w)

    # LEFT BOTTOM: Fuel bar
    kids.append(bar(-240, 20, 100, 14, "ui_BarFuel", value=50,
                    bg_img="ui_img_fuel2_png", ind_img="ui_img_fuel1_png"))
    kids.append(label(-310, 20, "FUEL", "ui_LblFuelIcon", FSMALL, C_DIM))

    # LEFT BOTTOM: Mode selector
    kids.append(label(-290, 48, "MODE", "ui_LblMode", FSMALL, C_DIM))
    kids.append(label(-280, 64, "ROAD", "ui_LblModeRoad", FSMALL, C_WHT))
    kids.append(label(-280, 80, "SPORT", "ui_LblModeSport", FSMALL, C_DIM))
    kids.append(label(-280, 96, "SPORT+", "ui_LblModeSportPlus", FSMALL, C_DIM))
    kids.append(panel(-280, 60, 60, 18, "ui_ModeBorder", [],
                      local_styles({"MAIN": {"DEFAULT": {"align": "CENTER", "bg_opa": 0,
                                                         "border_color": C_CYAN, "border_width": 1, "radius": 2,
                                                         "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0}}})))

    # RIGHT BOTTOM: ODO/Trip/Range
    kids.append(name_label(195, 25, "ODO", "ui_LblOdoName"))
    kids.append(val_label(310, 25, "0", "ui_LblOdo", FMID, 75))
    kids.append(name_label(195, 48, "TRIP", "ui_LblTripName"))
    kids.append(val_label(310, 48, "0.0", "ui_LblTrip", FMID, 75))
    kids.append(name_label(195, 71, "RANGE", "ui_LblRangeName"))
    kids.append(val_label(310, 71, "---", "ui_LblRange", FMID, 75))

    # BOTTOM CENTER: branding + decorative line
    kids.append(label(20, 118, "prodrive", "ui_LblProdrive", FSMALL, C_CYAN))
    kids.append(panel(20, 58, 220, 1, "ui_LineDeco", [],
                      local_styles({"MAIN": {"DEFAULT": {"align": "CENTER", "bg_color": C_CYAN, "bg_opa": 80,
                                                         "border_width": 0, "radius": 0,
                                                         "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0}}})))

    return kids


def build_telltale_children():
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
    return [image(0, y, img, ident) for y, img, ident in icons]


def build_screen_children():
    children = []

    # 1. ContainerDashboard
    dash_local = local_styles({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_color": C_BLK, "bg_opa": 255
    }}})
    children.append(panel(18, 0, 765, 256, "ui_ContainerDashboard",
                          build_dashboard_children(), dash_local))

    # 2. ContainerOpening (hidden)
    opening_local = local_styles({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_color": C_BLK, "bg_opa": 255
    }}})
    children.append(panel(18, 0, 765, 256, "ui_ContainerOpening",
                          [image(0, 0, "ui_img_fmw_op0_png", "ui_ImgOpening")],
                          opening_local, hidden=True))

    # 3. ContainerTelltale
    tell_local = local_styles({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_opa": 0
    }}})
    children.append(panel(-382, 0, 35, 256, "ui_ContainerTelltale",
                          build_telltale_children(), tell_local))

    # 4. NotifyBox (hidden)
    notify_label = label(0, 0, "", "ui_NotifyLabel", FLARGE, C_WHT,
                         align="CENTER", text_align="CENTER")
    notify_local = local_styles({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 4, "pad_bottom": 4, "pad_left": 4, "pad_right": 4,
        "bg_color": C_RED, "bg_opa": 255
    }}})
    children.append(panel(0, -100, 696, 50, "ui_NotifyBox",
                          [notify_label], notify_local, hidden=True))

    return children


def main():
    proj_path = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003\eez003.eez-project"

    with open(proj_path, "r", encoding="utf-8") as f:
        proj = json.load(f)

    # Update description
    proj["settings"]["general"]["description"] = "FULLMONI-WIDE Prodrive Racing Dashboard"

    # Get the Screen widget
    screen_widget = proj["userPages"][0]["components"][0]

    # Preserve screen-level styles
    screen_local = {
        "objID": uid(),
        "definition": {
            "MAIN": {
                "DEFAULT": {
                    "bg_color": C_BLK,
                    "bg_opa": 255
                }
            }
        }
    }
    screen_widget["localStyles"] = screen_local

    # Replace children with Prodrive design
    screen_widget["children"] = build_screen_children()

    # Write back
    with open(proj_path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)

    print(f"Done! Wrote {proj_path}")
    # Count widgets
    def count_widgets(node):
        n = 1
        for c in node.get("children", []):
            n += count_widgets(c)
        return n
    total = sum(count_widgets(c) for c in screen_widget["children"])
    print(f"Total widgets in Screen1: {total + 1}")  # +1 for screen itself

if __name__ == "__main__":
    main()
