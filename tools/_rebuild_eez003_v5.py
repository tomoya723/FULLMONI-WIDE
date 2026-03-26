"""Rebuild eez003.eez-project v5 — Prodrive Racing Dashboard layout.

Architecture:
  dashboard_bg.png = static text labels, fuel frame, mode border, branding
  EEZ widgets      = individual segment panels for all gauges + dynamic labels

Each gauge (RPM 25, boost 10, 6 sensors × 13) is a container with N segment panels.
Each segment has a colored border (outline) and transparent bg (bg_opa=0).
Firmware lights up segments by setting bg_opa/bg_color per child panel.

Run _gen_dashboard_bg.py first to generate dashboard_bg.png.
"""
import json, uuid, base64, os

PROJ = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003\eez003.eez-project"
ASSET_DIR = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003"

# Container constants
PW, PH = 765, 256
CX, CY = PW / 2, PH / 2  # 382.5, 128


def uid():
    return str(uuid.uuid4())


def style_obj():
    return {"objID": uid(), "useStyle": "default", "conditionalStyles": [], "childStyles": []}


def lsty(d):
    return {"objID": uid(), "definition": d}


# --- Coordinate converters: container-relative → EEZ offset ---

def c2c(x, y, w, h):
    """Container-relative (x, y, w, h) → CENTER align offsets (left, top)."""
    return round(x + w / 2 - CX), round(y + h / 2 - CY)


def c2rm(x, y, w, h):
    """Container-relative (x, y, w, h) → RIGHT_MID align offsets (left, top)."""
    return round(x + w - PW), round(y + h / 2 - CY)


def c2tl(seg_x, seg_y, cont_x, cont_y):
    """Dashboard-coord segment → TOP_LEFT offset within sub-container."""
    return (seg_x - cont_x, seg_y - cont_y)


# --- Widget builders ---

def w_base(wtype, left, top, width, height, ident=None, hidden=False, children=None):
    w = {
        "objID": uid(), "type": wtype,
        "left": left, "top": top, "width": width, "height": height,
        "customInputs": [], "customOutputs": [],
        "style": style_obj(), "timeline": [], "eventHandlers": [],
        "leftUnit": "px", "topUnit": "px", "widthUnit": "px", "heightUnit": "px",
    }
    if ident:
        w["identifier"] = ident
    if hidden:
        w["hiddenFlag"] = "HIDDEN"
    if children is not None:
        w["children"] = children
    elif wtype == "LVGLPanelWidget":
        w["children"] = []
    if wtype == "LVGLPanelWidget":
        w["hiddenInEditor"] = False
    return w


def _panel(left, top, w, h, ident, children, ls=None, hidden=False):
    o = w_base("LVGLPanelWidget", left, top, w, h, ident, hidden, children)
    if ls:
        o["localStyles"] = ls
    return o


def _seg(left, top, w, h, border_color="#00DCDC", border_width=2):
    """Gauge segment — BarWidget with colored border, fills left→right.
    MAIN: black bg + cyan border (outline always visible).
    INDICATOR: cyan fill (controlled by lv_bar_set_value)."""
    o = w_base("LVGLBarWidget", left, top, w, h)
    o["min"] = 0
    o["max"] = 100
    o["value"] = 0
    o["mode"] = "NORMAL"
    o["enableAnimation"] = False
    o["localStyles"] = lsty({
        "MAIN": {"DEFAULT": {
            "radius": 0,
            "bg_color": "#000000",
            "bg_opa": 255,
            "border_color": border_color,
            "border_width": border_width,
            "border_opa": 255,
            "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        }},
        "INDICATOR": {"DEFAULT": {
            "radius": 0,
            "bg_color": border_color,
            "bg_opa": 255,
        }},
    })
    return o


def _lbl(left, top, w, h, text, ident, font, color, align="CENTER", ta=None, hidden=False, lm=None):
    o = w_base("LVGLLabelWidget", left, top, w, h, ident, hidden)
    o["text"] = text
    if lm:
        o["longMode"] = lm
    sd = {"MAIN": {"DEFAULT": {"align": align, "text_font": font, "text_color": color}}}
    if ta:
        sd["MAIN"]["DEFAULT"]["text_align"] = ta
    o["localStyles"] = lsty(sd)
    return o


def _bar(left, top, w, h, ident, minv=0, maxv=100, val=0,
         bg="#2B2B2B", ind="#00B4B4", bg_img=None, ind_img=None):
    o = w_base("LVGLBarWidget", left, top, w, h, ident)
    o["min"] = minv
    o["max"] = maxv
    o["value"] = val
    o["mode"] = "NORMAL"
    o["enableAnimation"] = False
    sd = {
        "MAIN": {"DEFAULT": {"align": "CENTER", "radius": 0, "bg_color": bg, "bg_opa": 255}},
        "INDICATOR": {"DEFAULT": {"radius": 0, "bg_color": ind, "bg_opa": 255}},
    }
    if bg_img:
        sd["MAIN"]["DEFAULT"]["bg_img_src"] = bg_img
    if ind_img:
        sd["INDICATOR"]["DEFAULT"]["bg_img_src"] = ind_img
    o["localStyles"] = lsty(sd)
    return o


def _img(left, top, w, h, img_name, ident, hidden=False):
    o = w_base("LVGLImageWidget", left, top, w, h, ident, hidden)
    o["children"] = []
    o["image"] = img_name
    o["sizeMode"] = "VIRTUAL"
    o["innerAlign"] = "CENTER"
    o["setPivot"] = True
    o["pivotX"] = 0
    o["pivotY"] = 0
    o["zoom"] = 256
    o["angle"] = 0
    o["widgetFlags"] = ""
    o["hiddenFlagType"] = "literal" if not hidden else "NONE"
    o["clickableFlagType"] = "literal"
    o["checkedStateType"] = "literal"
    o["disabledStateType"] = "literal"
    o["states"] = ""
    o["groupIndex"] = 0
    o["localStyles"] = lsty({"MAIN": {"DEFAULT": {"align": "CENTER"}}})
    return o


# Colors
BK       = "#000000"
W        = "#FFFFFF"
GRAY     = "#A0A0A0"
RED      = "#FF2828"
CYAN     = "#00DCDC"
DKGRAY   = "#606060"

# Fonts
F1  = "ui_font_Font1"          # 27px — sensor values
F24 = "ui_font_FontHUD24"      # 24px — sensor values (native pixel size)
FM  = "ui_font_FontHUDmid"     # 18px — medium text
FS  = "ui_font_FontHUDsmall"   # ~10px — small labels
FL  = "ui_font_FontGear"       # 75px — gear (Micro Technic)
F36 = "ui_font_FontHUD36"      # 36px — speed
F30 = "ui_font_FontHUD30"      # 30px — ALS


def center_panel_ls():
    return lsty({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0, "bg_opa": 0}}})


def dark_panel_ls():
    return lsty({"MAIN": {"DEFAULT": {
        "align": "CENTER", "border_width": 0, "radius": 0,
        "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        "bg_color": BK, "bg_opa": 255}}})


# ═══════════════════════════════════════════════════════════════════════
# Gauge segment generators
# ═══════════════════════════════════════════════════════════════════════

def build_tacho_segs():
    """25 RPM tachometer segments → children of ui_ContainerTacho."""
    TACHO_L, TACHO_R = 245, 521
    N = 25
    SPAN = TACHO_R - TACHO_L  # 276
    CX_D, CY_D = 215, 0  # container origin in dashboard coords
    segs = []
    for i in range(N):
        t = i / (N - 1) if N > 1 else 0
        bx = TACHO_L + (SPAN * i) // N
        bx_next = TACHO_L + (SPAN * (i + 1)) // N
        bar_w = bx_next - bx
        y_bot = int(125 - 60 * t - 70 * t * (1 - t))
        bar_h = int(31 + 17 * t)
        y_top = y_bot - bar_h
        l, tp = c2tl(bx, y_top, CX_D, CY_D)
        bc = RED if t > 0.75 else CYAN
        segs.append(_seg(l, tp, bar_w, bar_h, border_color=bc, border_width=2))
    return segs


def build_boost_segs():
    """10 boost gauge segments → children of ui_ContainerBoost."""
    TACHO_L, TACHO_R = 245, 521
    BOOST_X, BOOST_W, N = 245, 80, 10
    H_MIN, H_MAX, ARC_BASE = 16, 25, 68

    positions = []
    for i in range(N):
        t = i / (N - 1) if N > 1 else 0
        bx = BOOST_X + (BOOST_W * i) // N
        bx_next = BOOST_X + (BOOST_W * (i + 1)) // N
        seg_w = bx_next - bx
        t_g = (bx - TACHO_L) / (TACHO_R - TACHO_L)
        seg_h = int(H_MIN + (H_MAX - H_MIN) * t)
        y_bot = int(ARC_BASE - 60 * t_g - 70 * t_g * (1 - t_g))
        y_top = y_bot - seg_h
        positions.append((bx, y_top, seg_w, seg_h))

    x_min = min(p[0] for p in positions)
    y_min = min(p[1] for p in positions)
    x_max = max(p[0] + p[2] for p in positions)
    y_max = max(p[1] + p[3] for p in positions)
    cx_d, cy_d, cw, ch = x_min - 1, y_min - 1, x_max - x_min + 2, y_max - y_min + 2

    segs = []
    for bx, yt, sw, sh in positions:
        l, tp = c2tl(bx, yt, cx_d, cy_d)
        segs.append(_seg(l, tp, sw, sh, border_color=CYAN, border_width=1))
    return segs, (cx_d, cy_d, cw, ch)


def build_sensor_segs(x, y_bottom, w, h_min=3, h_max=20, num_segs=13):
    """Wedge-shaped sensor gauge segments → children of container."""
    positions = []
    for s in range(num_segs):
        t_s = s / max(1, num_segs - 1)
        seg_h = max(3, int(h_min + (h_max - h_min) * t_s))
        seg_x = x + (w * s) // num_segs
        seg_x_next = x + (w * (s + 1)) // num_segs
        seg_w = seg_x_next - seg_x
        seg_top = y_bottom - seg_h
        positions.append((seg_x, seg_top, seg_w, seg_h))

    x_min = min(p[0] for p in positions)
    y_min = min(p[1] for p in positions)
    x_max = max(p[0] + p[2] for p in positions)
    y_max = max(p[1] + p[3] for p in positions)
    cx_d, cy_d, cw, ch = x_min, y_min, x_max - x_min, y_max - y_min

    segs = []
    for sx_p, sy_p, sw, sh in positions:
        l, tp = c2tl(sx_p, sy_p, cx_d, cy_d)
        segs.append(_seg(l, tp, sw, sh, border_color=CYAN, border_width=1))
    return segs, (cx_d, cy_d, cw, ch)


def build_dashboard():
    """Dashboard = bg image + segment gauge containers + telltale images + labels."""
    kids = []

    # ═══════════════════════════════════════════════════════════
    # BACKGROUND IMAGE — static text, fuel frame, mode border
    # ═══════════════════════════════════════════════════════════
    l, t = c2c(0, 0, PW, PH)
    kids.append(_img(l, t, PW, PH, "ui_img_dashboard_bg_png", "ui_ImgDashboardBg"))

    # ═══════════════════════════════════════════════════════════
    # RPM TACHOMETER — 25 segment panels in container
    # Firmware: lv_obj_get_child(ui_ContainerTacho, i)
    #   set bg_opa=LV_OPA_COVER + bg_color to light up
    # ═══════════════════════════════════════════════════════════
    l, t = c2c(215, 0, 340, 180)
    kids.append(_panel(l, t, 340, 180, "ui_ContainerTacho",
                       build_tacho_segs(), center_panel_ls()))

    # ═══════════════════════════════════════════════════════════
    # BOOST — 10 segment panels in container
    # ═══════════════════════════════════════════════════════════
    boost_segs, (bx, by, bw, bh) = build_boost_segs()
    l, t = c2c(bx, by, bw, bh)
    kids.append(_panel(l, t, bw, bh, "ui_ContainerBoost",
                       boost_segs, center_panel_ls()))

    # ═══════════════════════════════════════════════════════════
    # LEFT SENSOR GAUGES — 13 segment panels each
    # Firmware: lv_obj_get_child(ui_ContainerWaterTemp, i)
    # ═══════════════════════════════════════════════════════════
    left_sensors = [
        ("ui_ContainerWaterTemp",  14, 36, 130),
        ("ui_ContainerOilTemp",    14, 80, 130),
        ("ui_ContainerChargeTemp", 14, 124, 130),
    ]
    for ident, sx, yb, sw in left_sensors:
        segs, (cx_d, cy_d, cw, ch) = build_sensor_segs(sx, yb, sw)
        l, t = c2c(cx_d, cy_d, cw, ch)
        kids.append(_panel(l, t, cw, ch, ident, segs, center_panel_ls()))

    # LEFT SENSOR VALUES (right-aligned, after bar end)
    left_vals = [
        ("80",   "ui_LblWaterTemp",  24, 24),
        ("72",   "ui_LblOilTemp",    68, 24),
        ("53",   "ui_LblChargeTemp", 112, 24),
    ]
    for val_text, val_id, y, h in left_vals:
        l, t = c2c(172, y, 50, h)
        kids.append(_lbl(l, t, 50, h, val_text, val_id, F24, W,
                         align="CENTER", ta="RIGHT", lm="CLIP"))

    # ═══════════════════════════════════════════════════════════
    # RIGHT SENSOR GAUGES — 13 segment panels each
    # ═══════════════════════════════════════════════════════════
    right_sensors = [
        ("ui_ContainerOilPress",   565, 36, 130),
        ("ui_ContainerFuelPress",  565, 80, 130),
        ("ui_ContainerBattery",    565, 124, 130),
    ]
    for ident, sx, yb, sw in right_sensors:
        segs, (cx_d, cy_d, cw, ch) = build_sensor_segs(sx, yb, sw)
        l, t = c2c(cx_d, cy_d, cw, ch)
        kids.append(_panel(l, t, cw, ch, ident, segs, center_panel_ls()))

    # RIGHT SENSOR VALUES (anchored to dashboard right edge)
    right_vals = [
        ("5.8",  "ui_LblOilPress",   24, 24),
        ("3.5",  "ui_LblFuelPress",  68, 24),
        ("14.4", "ui_LblBattery",    112, 24),
    ]
    for val_text, val_id, y, h in right_vals:
        l, t = c2rm(700, y, 63, h)
        kids.append(_lbl(l, t, 63, h, val_text, val_id, F24, W,
                         align="RIGHT_MID", ta="RIGHT", lm="CLIP"))

    # ═══════════════════════════════════════════════════════════
    # BOOST VALUE
    # ═══════════════════════════════════════════════════════════
    l, t = c2c(335, 8, 65, 22)
    kids.append(_lbl(l, t, 65, 22, "-0.0", "ui_LblBoostVal", FM, W))

    # ═══════════════════════════════════════════════════════════
    # GEAR — large red letter, centered under tacho
    # Tacho center in container coords ≈ 384
    # Shadow spread simulates stroke_width from mockup (Pillow stroke_width=2)
    # ═══════════════════════════════════════════════════════════
    l, t = c2c(341, 93, 100, 100)
    kids.append(_lbl(l, t, 100, 100, "N", "ui_LblGear", FL, RED))

    # ALS indicator (cyan, hidden by default)
    l, t = c2c(505, 105, 60, 30)
    kids.append(_lbl(l, t, 60, 30, "ALS", "ui_LblALS", F30, CYAN, hidden=True))

    # ═══════════════════════════════════════════════════════════
    # WARNING TELLTALES — 9 images
    # ═══════════════════════════════════════════════════════════
    telltale_icons = [
        (468, 155, "ui_img_ws_oilpresswarning_png",  "ui_ImgWarnOilPress"),
        (506, 155, "ui_img_ws_masterwarning_png",    "ui_ImgWarnMaster"),
        (450, 185, "ui_img_ws_batterywarning_png",   "ui_ImgWarnBattery"),
        (488, 185, "ui_img_ws_exhaustwarning_png",   "ui_ImgWarnExhaust"),
        (526, 185, "ui_img_ws_waterwarning_png",     "ui_ImgWarnWaterHot"),
        (526, 185, "ui_img_ws_watercool_png",        "ui_ImgWarnWaterCold"),
        (440, 215, "ui_img_ws_breakwarning_png",     "ui_ImgWarnBrake"),
        (478, 215, "ui_img_ws_fuelcheck_png",        "ui_ImgWarnFuel"),
        (516, 215, "ui_img_ws_beltwarning_png",      "ui_ImageWarnBelt"),
    ]
    ICON_SZ = 25
    for ix, iy, img_name, ident in telltale_icons:
        l, t = c2c(ix, iy, ICON_SZ, ICON_SZ)
        kids.append(_img(l, t, ICON_SZ, ICON_SZ, img_name, ident))

    # ═══════════════════════════════════════════════════════════
    # FUEL BAR — visible bar (frame drawn in bg image)
    # ═══════════════════════════════════════════════════════════
    FUEL_W, FUEL_H = 120, 4
    l, t = c2c(42, 160, FUEL_W, FUEL_H)
    fuel = w_base("LVGLBarWidget", l, t, FUEL_W, FUEL_H, "ui_BarFuel")
    fuel["children"] = []
    fuel["widgetFlags"] = ""
    fuel["hiddenFlagType"] = "literal"
    fuel["clickableFlagType"] = "literal"
    fuel["checkedStateType"] = "literal"
    fuel["disabledStateType"] = "literal"
    fuel["states"] = ""
    fuel["group"] = ""
    fuel["groupIndex"] = 0
    fuel["min"] = 0
    fuel["max"] = 100
    fuel["value"] = 70
    fuel["valueType"] = "literal"
    fuel["mode"] = "NORMAL"
    fuel["enableAnimation"] = False
    fuel["localStyles"] = lsty({
        "MAIN": {"DEFAULT": {
            "align": "CENTER", "radius": 0,
            "bg_color": BK, "bg_opa": 0,
            "pad_top": 0, "pad_bottom": 0, "pad_left": 0, "pad_right": 0,
        }},
        "INDICATOR": {"DEFAULT": {
            "radius": 0, "bg_color": CYAN, "bg_opa": 255,
        }},
    })
    kids.append(fuel)

    # ═══════════════════════════════════════════════════════════
    # P brake, Speed, kph, ODO/Trip/Range
    # ═══════════════════════════════════════════════════════════
    l, t = c2c(440, 210, 20, 20)
    kids.append(_lbl(l, t, 20, 20, "P", "ui_LblParking", FS, RED, hidden=True))

    l, t = c2c(305, 210, 70, 35)
    kids.append(_lbl(l, t, 70, 35, "0", "ui_LblSpd", F36, W, ta="RIGHT"))

    l, t = c2c(375, 218, 35, 18)
    kids.append(_lbl(l, t, 35, 18, "kph", "ui_LblSpdUnit", FM, W))

    info_vals = [
        ("123456", "ui_LblOdo",   160, 14),
        ("0.0",    "ui_LblTrip",  176, 14),
        ("65 km",  "ui_LblRange", 192, 14),
    ]
    for val_text, val_id, y, h in info_vals:
        l, t = c2rm(665, y, 95, h)
        kids.append(_lbl(l, t, 95, h, val_text, val_id, FM, W,
                         align="RIGHT_MID", ta="RIGHT", lm="CLIP"))

    return kids


def build_telltale():
    """Vertical telltale strip (same as eez002 — left edge of screen)."""
    icons = [
        (-106, "ui_img_ws_masterwarning_png", "ui_ImgTelltMaster"),
        (-76, "ui_img_ws_oilpresswarning_png", "ui_ImgTelltOilPress"),
        (-46, "ui_img_ws_watercool_png", "ui_ImgTelltWaterCold"),
        (-46, "ui_img_ws_waterwarning_png", "ui_ImgTelltWaterHot"),
        (-16, "ui_img_ws_exhaustwarning_png", "ui_ImgTelltExhaust"),
        (14, "ui_img_ws_batterywarning_png", "ui_ImgTelltBattery"),
        (44, "ui_img_ws_breakwarning_png", "ui_ImgTelltBrake"),
        (74, "ui_img_ws_beltwarning_png", "ui_ImgTelltBelt"),
        (104, "ui_img_ws_fuelcheck_png", "ui_ImgTelltFuel"),
    ]
    return [_img(0, y, 25, 25, im, ident) for y, im, ident in icons]


def build_screen_children():
    return [
        _panel(0, 0, 765, 256, "ui_ContainerDashboard", build_dashboard(), dark_panel_ls()),
        _panel(0, 0, 765, 256, "ui_ContainerOpening",
               [_img(0, 0, 765, 256, "ui_img_fmw_op0_png", "ui_ImgOpening")],
               dark_panel_ls(), hidden=True),
        _panel(0, -100, 696, 50, "ui_NotifyBox",
               [_lbl(0, 0, 680, 42, "", "ui_NotifyLabel", FL, W, align="CENTER", ta="CENTER")],
               lsty({"MAIN": {"DEFAULT": {"align": "CENTER", "border_width": 0, "radius": 0,
                                           "pad_top": 4, "pad_bottom": 4, "pad_left": 4, "pad_right": 4,
                                           "bg_color": RED, "bg_opa": 255}}}), hidden=True),
    ]


def png_to_data_uri(path):
    with open(path, "rb") as f:
        return "data:image/png;base64," + base64.b64encode(f.read()).decode("ascii")


def main():
    with open(PROJ, "r", encoding="utf-8") as f:
        proj = json.load(f)

    proj["settings"]["general"]["description"] = "FULLMONI-WIDE Prodrive Racing Dashboard"

    # Ensure FontHUD24 (24px = native pixel size) exists
    font_names = {f["name"] for f in proj.get("fonts", [])}
    if "ui_font_FontHUD24" not in font_names:
        proj.setdefault("fonts", []).append({
            "objID": str(uuid.uuid4()),
            "name": "ui_font_FontHUD24",
            "renderingEngine": "LVGL",
            "source": {
                "objID": str(uuid.uuid4()),
                "filePath": "x14y24pxHeadUpDaisy.ttf",
                "size": 24,
            },
            "bpp": 1,
            "threshold": 128,
            "glyphs": [],
            "lvglRanges": "32-127",
            "lvglSymbols": "",
        })
        print("  +font: ui_font_FontHUD24 (24px native)")

    # Ensure FontGear (Micro Technic Extended Outline Bold) exists
    if "ui_font_FontGear" not in font_names:
        proj.setdefault("fonts", []).append({
            "objID": str(uuid.uuid4()),
            "name": "ui_font_FontGear",
            "renderingEngine": "LVGL",
            "source": {
                "objID": str(uuid.uuid4()),
                "filePath": "FontsFree-Net-Apice-BoldOutline.ttf",
                "size": 112,
            },
            "bpp": 4,
            "threshold": 128,
            "glyphs": [],
            "lvglRanges": "32-127",
            "lvglSymbols": "",
        })
        print("  +font: ui_font_FontGear (75px Micro Technic)")

    # Ensure all required bitmaps exist
    required_imgs = {
        "ui_img_dashboard_bg_png":        "dashboard_bg.png",
    }
    existing = {b["name"] for b in proj.get("bitmaps", [])}
    # Remove old dashboard_bg entry if exists (force re-embed)
    proj["bitmaps"] = [b for b in proj.get("bitmaps", []) if b["name"] not in required_imgs]
    for name, fn in required_imgs.items():
        path = os.path.join(ASSET_DIR, fn)
        if os.path.exists(path):
            proj["bitmaps"].append({
                "objID": uid(), "name": name,
                "image": png_to_data_uri(path),
                "bpp": 24, "lvglBinaryOutputFormat": 4, "lvglDither": False,
            })
            print(f"  +bitmap: {name}")
        else:
            print(f"  WARNING: missing asset: {path}")

    screen = proj["userPages"][0]["components"][0]
    screen["localStyles"] = lsty({"MAIN": {"DEFAULT": {"bg_color": BK, "bg_opa": 255}}})
    screen["children"] = build_screen_children()

    with open(PROJ, "w", encoding="utf-8", newline="\n") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)

    # Count widgets
    def cnt(n):
        return 1 + sum(cnt(c) for c in n.get("children", []))
    total = sum(cnt(c) for c in screen["children"])
    print(f"Total widgets: {total + 1}")
    print(f"Wrote: {PROJ}")

    # Verify absolute positions
    print("\n=== Verify absolute positions (ContainerDashboard children) ===")
    # screen=800, panel=765 CENTER-aligned with left=0 → left_edge = (800-765)/2 = 17.5
    CONT_X = 17.5
    for child in screen["children"]:
        if child.get("identifier") == "ui_ContainerDashboard":
            for w in child.get("children", []):
                ident = w.get("identifier", "?")
                l, t, ww, hh = w["left"], w["top"], w["width"], w["height"]
                ls = w.get("localStyles", {})
                defn = ls.get("definition", {})
                align = defn.get("MAIN", {}).get("DEFAULT", {}).get("align", "?")
                if align == "CENTER":
                    ax = CONT_X + CX + l - ww / 2
                    ay = CY + t - hh / 2
                elif align == "RIGHT_MID":
                    ax = CONT_X + PW + l - ww
                    ay = CY + t - hh / 2
                else:
                    ax = ay = 0
                print(f"  {ident:<30} abs=({ax:.0f},{ay:.0f}) {ww}x{hh} align={align}")


if __name__ == "__main__":
    main()
