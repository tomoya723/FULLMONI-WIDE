"""
eez003 Prodrive layout mockup generator — v3
Fixes from user feedback:
  1. RPM gauge: arc shape rising diagonally to upper-right (fan tacho)
  2. Warning telltales: grouped at center-bottom area
  3. 6 gauges: triangular shape (bars get longer going up, forming right-rising triangle)
  4. Fix text overlaps and overflow
"""
from PIL import Image, ImageDraw, ImageFont
import os, math

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

W, H = 800, 256
CONT_X, CONT_W = 18, 765

# Colors
CYAN    = (0, 220, 220)
WHITE   = (255, 255, 255)
GRAY    = (160, 160, 160)
DKGRAY  = (60, 60, 60)
RED     = (255, 40, 40)
BLACK   = (0, 0, 0)
BAR_BG  = (30, 30, 30)
BAR_FG  = (0, 190, 190)
BAR_RED = (200, 40, 40)
BORDER  = (0, 200, 180)
WARN_YEL = (220, 180, 0)
WARN_RED = (220, 40, 40)

try:
    font_lg  = ImageFont.truetype("arial.ttf", 48)
    font_big = ImageFont.truetype("arial.ttf", 27)
    font_gauge = ImageFont.truetype("arial.ttf", 22)
    font_spd = ImageFont.truetype("arial.ttf", 36)
    font_med = ImageFont.truetype("arial.ttf", 18)
    font_sm  = ImageFont.truetype("arial.ttf", 11)
    font_xs  = ImageFont.truetype("arial.ttf", 9)
except:
    font_lg = font_big = font_gauge = font_spd = font_med = font_sm = font_xs = ImageFont.load_default()

img = Image.new("RGB", (W, H), BLACK)
draw = ImageDraw.Draw(img)

def sx(x):
    return CONT_X + x

def hbar_seg(x, y, w, h, fill_pct, num_segs=12):
    """Segmented bar (cyan blocks) — flat horizontal"""
    ax = sx(x)
    gap = 2
    seg_w = max(1, (w - gap * (num_segs - 1)) // num_segs)
    filled = int(num_segs * fill_pct)
    for s in range(num_segs):
        seg_x = ax + s * (seg_w + gap)
        c = BAR_FG if s < filled else BAR_BG
        draw.rectangle([seg_x, y, seg_x + seg_w, y + h], fill=c)

def wedge_bar_seg(x, y_bottom, w, h_min, h_max, fill_pct, num_segs=12, rtl=False, outline_width=1):
    """Wedge-shaped segmented bar — gap-less, overlapping segments.
    Filled = solid CYAN, unfilled = CYAN outline only (like RPM tacho)."""
    ax = sx(x)
    seg_w = w // (num_segs - 1)  # gap-less
    filled = int(num_segs * fill_pct)
    for s in range(num_segs):
        t = s / max(1, num_segs - 1)  # 0.0 → 1.0
        seg_h = int(h_min + (h_max - h_min) * t)
        seg_x = ax + s * seg_w
        seg_top = y_bottom - seg_h
        if rtl:
            is_filled = (num_segs - 1 - s) < filled
        else:
            is_filled = s < filled
        if is_filled:
            draw.rectangle([seg_x, seg_top, seg_x + seg_w, y_bottom], fill=CYAN)
        else:
            draw.rectangle([seg_x, seg_top, seg_x + seg_w, y_bottom], fill=None, outline=CYAN, width=outline_width)

# ═══════════════════════════════════════════════════════════════════
# LEFT SENSORS (3 rows) — WEDGE BARS: each bar is a ramp shape
# Bottom edge flat, top edge rises diagonally left→right
# Overall triangle: top row longest bar, bottom shortest
# ═══════════════════════════════════════════════════════════════════
left_rows = [
    # (name, value, bar_w, fill_pct, y_bottom)
    ("WATER TEMP",   "80",  130, 0.67, 36),   # top
    ("OIL TEMP",     "72",  130, 0.60, 80),    # middle
    ("CHARGE TEMP",  "53",  130, 0.44, 124),   # bottom
]
for name, val, bw, pct, yb in left_rows:
    ns = max(6, int(bw / 10))
    wedge_bar_seg(14, yb, bw, 3, 20, pct, num_segs=ns)
    draw.text((sx(14), yb + 2), name, fill=GRAY, font=font_xs)
    draw.text((sx(195), yb - 18), val, fill=WHITE, font=font_gauge, anchor="rt")

# ═══════════════════════════════════════════════════════════════════
# RIGHT SENSORS (3 rows) — WEDGE BARS: left-origin (same as left side)
# ═══════════════════════════════════════════════════════════════════
right_bar_x = 590
right_rows = [
    # (name, value, bar_w, fill_pct, y_bottom)
    ("OIL PRESSURE",   "5.8",  130, 0.72, 36),   # top
    ("FUEL PRESSURE",  "3.5",  130, 0.44, 80),    # middle
    ("BATTERY VOLT",   "14.4", 130, 0.80, 124),   # bottom
]
for name, val, bw, pct, yb in right_rows:
    ns = max(6, int(bw / 10))
    wedge_bar_seg(right_bar_x, yb, bw, 3, 20, pct, num_segs=ns)
    draw.text((sx(right_bar_x), yb + 2), name, fill=GRAY, font=font_xs)
    draw.text((sx(right_bar_x + bw + 50), yb - 18), val, fill=WHITE, font=font_gauge, anchor="rt")

# ═══════════════════════════════════════════════════════════════════
# BOOST GAUGE — arc-shaped segmented bars (concentric with tacho arc)
# ═══════════════════════════════════════════════════════════════════
tacho_left = 245
tacho_right = 521
boost_x = 245
boost_num_segs = 10
boost_w = 80
boost_seg_w = boost_w // (boost_num_segs - 1)  # gap-less
boost_h_min = 16
boost_h_max = 25
boost_fill_pct = 0.33
# Tacho arc: y = 125 - 60*t_global - 70*t_global*(1-t_global)
# BOOST uses same curvature (70) with global t, offset to BOOST y-position
boost_arc_base = 68   # y_bottom at leftmost bar (53 + 15)
for i in range(boost_num_segs):
    t = i / (boost_num_segs - 1)
    bx = boost_x + int(t * boost_w)
    # Global t relative to full tacho span (concentric arc)
    t_global = (bx - tacho_left) / (tacho_right - tacho_left)
    seg_h = int(boost_h_min + (boost_h_max - boost_h_min) * t)
    # Same curvature as tacho: linear slope + parabolic bulge
    y_bottom = int(boost_arc_base - 60 * t_global - 70 * t_global * (1 - t_global))
    y_top = y_bottom - seg_h
    if t <= boost_fill_pct:
        draw.rectangle([sx(bx), y_top, sx(bx + boost_seg_w), y_bottom], fill=CYAN)
    else:
        draw.rectangle([sx(bx), y_top, sx(bx + boost_seg_w), y_bottom], fill=None, outline=CYAN, width=1)

draw.text((sx(240), 30), "BOOST", fill=GRAY, font=font_xs)
draw.text((sx(335), 10), "-0.0", fill=WHITE, font=font_med)

# ═══════════════════════════════════════════════════════════════════
# CENTER: FAN TACHOMETER — Arc shape rising diagonally upper-right
# Unfilled cyan = outline only, red zone = outline only
# ═══════════════════════════════════════════════════════════════════
num_bars = 25
rpm_fill_pct = 0.45  # current RPM ~3600 of 8000
for i in range(num_bars):
    t = i / (num_bars - 1)
    bx = tacho_left + int(t * (tacho_right - tacho_left))

    y_bottom = int(125 - 60 * t - 70 * t * (1 - t))
    bar_height = int(31 + 17 * t)  # 0: 31px → 8: 48px
    y_top = y_bottom - bar_height

    bar_w = (tacho_right - tacho_left) // (num_bars - 1)  # gap-less
    if t > 0.75:
        # Red zone: always outline only
        draw.rectangle([sx(bx), y_top, sx(bx + bar_w), y_bottom], fill=None, outline=BAR_RED, width=2)
    elif t <= rpm_fill_pct:
        # Reached: filled cyan
        draw.rectangle([sx(bx), y_top, sx(bx + bar_w), y_bottom], fill=CYAN)
    else:
        # Not reached: outline only cyan
        draw.rectangle([sx(bx), y_top, sx(bx + bar_w), y_bottom], fill=None, outline=CYAN, width=2)

# Tacho number labels: 0-8
try:
    font_tacho_num = ImageFont.truetype("arialbd.ttf", 14)
except:
    font_tacho_num = font_sm
for n in range(9):
    t = n / 8.0
    nx = tacho_left + int(t * (tacho_right - tacho_left))
    ny_bottom = int(125 - 60 * t - 70 * t * (1 - t))
    draw.text((sx(nx - 3), ny_bottom + 2), str(n), fill=WHITE, font=font_tacho_num)

# ENGINE RPM label
draw.text((sx(527), 12), "ENGINE", fill=GRAY, font=font_xs)
draw.text((sx(535), 23), "RPM", fill=GRAY, font=font_xs)

# ═══════════════════════════════════════════════════════════════════
# GEAR (large red, centered below tacho bars)
# ═══════════════════════════════════════════════════════════════════
try:
    font_gear = ImageFont.truetype(os.path.join(SCRIPT_DIR, "..", "Firmware", "eez", "eez003", "ui", "fonts", "Micro Technic Extended Outline Bold.ttf"), 75)
except:
    font_gear = font_lg
# Center "N" horizontally and vertically on tacho area
gear_bbox = font_gear.getbbox("N")
gear_w = gear_bbox[2] - gear_bbox[0]
gear_h = gear_bbox[3] - gear_bbox[1]
gear_cx = (tacho_left + tacho_right) // 2
gear_y = (256 - gear_h) // 2 - gear_bbox[1] + 30  # vertically centered + 30px down
draw.text((sx(gear_cx - gear_w // 2), gear_y), "N", fill=RED, font=font_gear, stroke_width=2, stroke_fill=RED)

# ═══════════════════════════════════════════════════════════════════
# WARNING TELLTALES — 2-row x 4-col grid, actual PNG icons
# Positioned right of speed display (regulatory requirement)
# ═══════════════════════════════════════════════════════════════════
# Layout: 2-3-3 per user specification
# Row 0 (2, centered): oilpress, masterwarning
# Row 1 (3): battery, exhaust, waterwarning
# Row 2 (3, half-pitch left): brake, fuelcheck, belt
warn_rows = [
    ["ws_oilpresswarning.png", "ws_masterwarning.png"],
    ["ws_batterywarning.png",  "ws_exhaustwarning.png", "ws_waterwarning.png"],
    ["ws_breakwarning.png",    "ws_fuelcheck.png",      "ws_beltwarning.png"],
]
warn_icon_dir = os.path.join(os.path.dirname(__file__), "..", "Firmware", "eez", "eez002")
icon_size = 30
warn_x_start = 450
warn_y_start = 155
col_gap = 38
row_gap = 30
for r, row in enumerate(warn_rows):
    if len(row) == 2:
        x_offset = col_gap // 2   # center 2-icon row
    elif r == 2:
        x_offset = -(col_gap // 4)  # half-pitch left for row 2
    else:
        x_offset = 0
    for c, icon_name in enumerate(row):
        wx = sx(warn_x_start + x_offset + c * col_gap)
        wy = warn_y_start + r * row_gap
        icon_path = os.path.join(warn_icon_dir, icon_name)
        try:
            icon_img = Image.open(icon_path).convert("RGBA")
            icon_img = icon_img.resize((icon_size, icon_size), Image.LANCZOS)
            img.paste(icon_img, (wx, wy), icon_img)
        except Exception:
            draw.rectangle([wx, wy, wx + icon_size, wy + icon_size], outline=RED)
            draw.text((wx + 2, wy + 6), icon_name[:3], fill=RED, font=font_xs)

# ═══════════════════════════════════════════════════════════════════
# BOTTOM LEFT: Fuel gauge + Mode + SPORT+
# ═══════════════════════════════════════════════════════════════════
# Fuel gauge: U-shaped border (no top), quarter tick marks, two-track bar
fuel_icon_x = 5
fuel_bar_x = 28
fuel_bar_w = 136
fuel_bar_y = 158
fuel_bar_h = 12

# Fuel pump icon (load PNG, keep original colors)
fuel_icon_path = os.path.join(os.path.dirname(__file__), "..", "Firmware", "eez", "eez002", "ws_fuelcheck.png")
try:
    fuel_icon_img = Image.open(fuel_icon_path).convert("RGBA")
    # Black bg + orange icon → use max(R,G,B) as alpha, RGB = white (Pillow only)
    r, g, b, _a = fuel_icon_img.split()
    from PIL import ImageChops
    alpha = ImageChops.lighter(r, ImageChops.lighter(g, b))  # max(R,G,B)
    white_ch = Image.new("L", fuel_icon_img.size, 255)
    fuel_icon_img = Image.merge("RGBA", (white_ch, white_ch, white_ch, alpha))
    icon_y = fuel_bar_y - 6
    img.paste(fuel_icon_img, (sx(fuel_icon_x - 2), icon_y), fuel_icon_img)
    # Left-pointing triangle (◀) — fuel filler side indicator
    tri_x = sx(fuel_icon_x - 2) - 8
    tri_cy = icon_y + fuel_icon_img.size[1] // 2
    draw.polygon([(tri_x, tri_cy), (tri_x + 7, tri_cy - 5), (tri_x + 7, tri_cy + 5)], fill=WHITE)
except Exception:
    draw.rectangle([sx(fuel_icon_x), fuel_bar_y + 1, sx(fuel_icon_x + 9), fuel_bar_y + fuel_bar_h - 1], outline=GRAY, width=1)

# U-shaped border (left, bottom, right — NO top edge)
bx1 = sx(fuel_bar_x)
bx2 = sx(fuel_bar_x + fuel_bar_w)
by1 = fuel_bar_y
by2 = fuel_bar_y + fuel_bar_h
draw.line([bx1, by1, bx1, by2], fill=WHITE, width=1)          # left
draw.line([bx1, by2, bx2, by2], fill=WHITE, width=1)          # bottom
draw.line([bx2, by1, bx2, by2], fill=WHITE, width=1)          # right

# Quarter tick marks (vertical lines at 25%, 50%, 75%)
for q in [0.25, 0.50, 0.75]:
    qx = sx(fuel_bar_x + int(fuel_bar_w * q))
    draw.line([qx, by1, qx, by2], fill=WHITE, width=1)

# Top track: cyan fuel level (inside, upper half)
track_top_y = fuel_bar_y + 2
track_h = 3
fuel_fill = 0.80
fill_w = int((fuel_bar_w - 4) * fuel_fill)
draw.rectangle([sx(fuel_bar_x + 2), track_top_y, sx(fuel_bar_x + 2 + fill_w), track_top_y + track_h], fill=CYAN)

# Bottom track: short red line, left-aligned (inside, lower half)
track_bot_y = fuel_bar_y + fuel_bar_h - 2 - track_h
red_w = int((fuel_bar_w - 4) * 0.15)  # short, ~15% of bar width
draw.rectangle([sx(fuel_bar_x + 2), track_bot_y, sx(fuel_bar_x + 2 + red_w), track_bot_y + track_h], fill=BAR_RED)

# ═══════════════════════════════════════════════════════════════════
# BOTTOM CENTER: Speed + kph + ROAD SPEED + P brake
# ═══════════════════════════════════════════════════════════════════
draw.text((sx(340), 210), "0", fill=WHITE, font=font_spd)
draw.text((sx(375), 217), "kph", fill=WHITE, font=font_med)

draw.text((sx(310), 243), "ROAD SPEED", fill=GRAY, font=font_xs)

# (P brake removed — replaced by warning telltale grid)

# ═══════════════════════════════════════════════════════════════════
# BOTTOM RIGHT: ODO / Trip / Range
# ═══════════════════════════════════════════════════════════════════
info_x_name = 600
info_x_val = 665
draw.text((sx(info_x_name), 160), "ODO", fill=GRAY, font=font_sm)
draw.text((sx(info_x_val), 160), "123456", fill=WHITE, font=font_sm)

draw.text((sx(info_x_name), 176), "Trip", fill=GRAY, font=font_sm)
draw.text((sx(info_x_val), 176), "0.0", fill=WHITE, font=font_sm)

draw.text((sx(info_x_name), 192), "Range", fill=GRAY, font=font_sm)
draw.text((sx(info_x_val), 192), "65 km", fill=WHITE, font=font_sm)

# ═══════════════════════════════════════════════════════════════════
# BRANDING
# ═══════════════════════════════════════════════════════════════════
draw.text((sx(375), 244), "prodrive", fill=DKGRAY, font=font_xs)

# ─── Save ─────────────────────────────────────────────────────────
out = os.path.join(os.path.dirname(__file__), "..", "docs", "eez003_layout_mockup.png")
img.save(out)
print(f"Saved: {out}")
