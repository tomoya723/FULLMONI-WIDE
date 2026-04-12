"""Generate dashboard_bg.png for eez003 Prodrive layout.

Renders ONLY truly static decorative elements:
  - Sensor name labels (WATER TEMP, OIL TEMP, etc.)
  - Tacho number labels (0-8)
  - BOOST / ENGINE RPM static labels
  - E/F fuel labels + fuel frame + fuel pump icon
  - Mode / SPORT+ border
  - Decorative line
  - ROAD SPEED label
  - prodrive branding
  - ODO/Trip/Range name labels

All gauge segments (RPM, boost, 6 sensors) are individual EEZ panel widgets.
Warning telltale icons are separate EEZ image widgets.
"""
from PIL import Image, ImageDraw, ImageFont
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
EEZ003_DIR = os.path.join(SCRIPT_DIR, "..", "Firmware", "eez", "eez003")
EEZ002_DIR = os.path.join(SCRIPT_DIR, "..", "Firmware", "eez", "eez002")

W, H = 765, 256  # ContainerDashboard size (not full 800px screen)

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

try:
    font_tacho_num = ImageFont.truetype("arialbd.ttf", 14)
    font_sm  = ImageFont.truetype("arial.ttf", 11)
    font_xs  = ImageFont.truetype("arial.ttf", 9)
    font_med = ImageFont.truetype("arial.ttf", 18)
except Exception:
    font_tacho_num = font_sm = font_xs = font_med = ImageFont.load_default()

img = Image.new("RGB", (W, H), (0, 0, 0))  # solid black background
draw = ImageDraw.Draw(img)


# ═══════════════════════════════════════════════════════════════════
# LEFT SENSORS — name labels only (gauge segments are EEZ widgets)
# ═══════════════════════════════════════════════════════════════════
left_rows = [
    ("WATER TEMP",     36),
    ("INLET AIR TEMP", 80),
    ("OIL TEMP",       124),
]
for name, yb in left_rows:
    draw.text((14, yb + 2), name, fill=GRAY, font=font_xs)

# ═══════════════════════════════════════════════════════════════════
# RIGHT SENSORS — name labels only (gauge segments are EEZ widgets)
# ═══════════════════════════════════════════════════════════════════
right_rows = [
    ("OIL PRESSURE",   36),
    ("A/F",            80),
    ("BATTERY VOLT",   124),
]
for name, yb in right_rows:
    draw.text((565, yb + 2), name, fill=GRAY, font=font_xs)

# ═══════════════════════════════════════════════════════════════════
# CENTER: BOOST / ENGINE RPM static labels only
# (Tachometer bars drawn by fan_tacho.c at runtime)
# (Boost arc is visual-only, no dynamic widget needed)
# ═══════════════════════════════════════════════════════════════════
tacho_left = 245
tacho_right = 521

draw.text((240, 30), "BOOST", fill=GRAY, font=font_xs)

# Tacho number labels: 0-8
for n in range(9):
    t = n / 8.0
    nx = tacho_left + int(t * (tacho_right - tacho_left))
    ny_bottom = int(125 - 60 * t - 70 * t * (1 - t))
    draw.text((nx - 3, ny_bottom + 2), str(n), fill=WHITE, font=font_tacho_num)

# ENGINE RPM label
draw.text((527, 12), "ENGINE", fill=GRAY, font=font_xs)
draw.text((535, 23), "RPM", fill=GRAY, font=font_xs)

# (Warning telltale icons are separate LVGL widgets for show/hide control)

# ═══════════════════════════════════════════════════════════════════
# BOTTOM LEFT: Fuel gauge frame (U-border + tick marks + E/F)
# (Fuel bar is a LVGL BarWidget; fuel icon is static decoration)
# ═══════════════════════════════════════════════════════════════════
fuel_bar_x = 40
fuel_bar_w = 124
fuel_bar_y = 158
fuel_bar_h = 12

# U-shaped border
bx1 = fuel_bar_x
bx2 = fuel_bar_x + fuel_bar_w
by1 = fuel_bar_y
by2 = fuel_bar_y + fuel_bar_h
draw.line([bx1, by1, bx1, by2], fill=WHITE, width=1)
draw.line([bx1, by2, bx2, by2], fill=WHITE, width=1)
draw.line([bx2, by1, bx2, by2], fill=WHITE, width=1)

# Quarter tick marks
for q in [0.25, 0.50, 0.75]:
    qx = fuel_bar_x + int(fuel_bar_w * q)
    draw.line([qx, by1, qx, by2], fill=WHITE, width=1)

# E / F labels
draw.text((fuel_bar_x - 2, fuel_bar_y + fuel_bar_h + 2), "E", fill=GRAY, font=font_xs)
draw.text((fuel_bar_x + fuel_bar_w - 4, fuel_bar_y + fuel_bar_h + 2), "F", fill=GRAY, font=font_xs)

# Fuel pump icon + left-pointing triangle (◀ filler side indicator)
fuel_icon_path = os.path.join(EEZ002_DIR, "ws_fuelcheck.png")
try:
    from PIL import ImageChops
    fuel_icon_img = Image.open(fuel_icon_path).convert("RGBA")
    r, g, b, _a = fuel_icon_img.split()
    alpha = ImageChops.lighter(r, ImageChops.lighter(g, b))
    white_ch = Image.new("L", fuel_icon_img.size, 255)
    fuel_icon_img = Image.merge("RGBA", (white_ch, white_ch, white_ch, alpha))
    # Layout: ◀(tri) 3px gap [icon 25px] 4px gap | U-border
    tri_tip_x = 2
    tri_base_x = tri_tip_x + 6
    icon_x = tri_base_x + 3           # 11
    icon_y = fuel_bar_y - 6
    img.paste(fuel_icon_img, (icon_x, icon_y), fuel_icon_img)
    tri_cy = icon_y + fuel_icon_img.size[1] // 2
    draw.polygon([(tri_tip_x, tri_cy), (tri_base_x, tri_cy - 4), (tri_base_x, tri_cy + 4)], fill=WHITE)
except Exception:
    draw.rectangle([5, fuel_bar_y + 1, 14, fuel_bar_y + fuel_bar_h - 1], outline=GRAY, width=1)

# Red low-fuel indicator bar (static, bottom track inside U-border)
red_bar_y = fuel_bar_y + fuel_bar_h - 4  # lower half inside frame
red_bar_w = int(fuel_bar_w * 0.15)       # ~15% from E side
draw.rectangle([fuel_bar_x + 2, red_bar_y, fuel_bar_x + 2 + red_bar_w, red_bar_y + 2], fill="#FF3030")

# ═══════════════════════════════════════════════════════════════════
# BOTTOM CENTER: static labels
# ═══════════════════════════════════════════════════════════════════
draw.text((310, 243), "ROAD SPEED", fill=GRAY, font=font_xs)
draw.text((375, 244), "prodrive", fill=DKGRAY, font=font_xs)

# ═══════════════════════════════════════════════════════════════════
# BOTTOM RIGHT: ODO/Trip/Range name labels (values are dynamic)
# ═══════════════════════════════════════════════════════════════════
draw.text((600, 160), "ODO", fill=GRAY, font=font_sm)
draw.text((600, 176), "Trip", fill=GRAY, font=font_sm)
draw.text((600, 192), "Range", fill=GRAY, font=font_sm)

# ─── Save ─────────────────────────────────────────────────────────
out = os.path.join(EEZ003_DIR, "dashboard_bg.png")
img.save(out)
print(f"Saved: {out}  ({W}x{H}, RGB)")
