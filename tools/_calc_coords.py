"""Convert eez002 CENTER-offset coords to absolute pixel coords.
Parent: ContainerDashboard at screen x=18, 765x256
CENTER: base = (382.5, 128)
RIGHT_MID: base right edge = (765, 128)
"""

# ContainerDashboard: screen x=18, size 765x256
# Center of container (in container coords): (382.5, 128)
CX, CY = 382.5, 128.0
PW = 765  # parent width

widgets_eez002 = [
    # (ident, left, top, w, h, align)
    ("ui_BarWaterTemp",  -300, -102, 80, 10, "CENTER"),
    ("ui_LblWaterTemp",  -570, -93,  65, 28, "RIGHT_MID"),
    ("ui_BarIAT",        -300, -59,  80, 10, "CENTER"),
    ("ui_LblIAT",        -570, -53,  65, 28, "RIGHT_MID"),
    ("ui_BarOilTemp",    -300, -16,  80, 10, "CENTER"),
    ("ui_LblOilTemp",    -570, -11,  65, 28, "RIGHT_MID"),
    ("ui_BarMAP",        -300,  23,  80, 10, "CENTER"),
    ("ui_LblMAP",        -570,  29,  65, 28, "RIGHT_MID"),
    ("ui_BarOilPress",   -300,  62,  80, 10, "CENTER"),
    ("ui_LblOilPress",   -570,  66,  65, 28, "RIGHT_MID"),
    ("ui_BarBattery",    -300, 101,  80, 10, "CENTER"),
    ("ui_LblBattery",    -570, 103,  65, 28, "RIGHT_MID"),
    ("ui_LblGEAR",       164,  -6,   42, 72, "CENTER"),
    ("ui_LblRPM",        -300,  31, 100, 30, "RIGHT_MID"),
    ("ui_LblTIME",       -190,-113,  55, 18, "RIGHT_MID"),
    ("ui_LblSPD",        -65,   36, 100, 36, "RIGHT_MID"),
    ("ui_LblAFR",        -230,  85, 100, 36, "RIGHT_MID"),
    ("ui_LblTrip",       -40,   72,  70, 18, "RIGHT_MID"),
    ("ui_LblODO",        -40,   91,  70, 18, "RIGHT_MID"),
    ("ui_BarFUEL",       280,  114, 100, 15, "CENTER"),
    ("ui_ImgTacho",       27,    0, 340,256, "CENTER"),
    ("ui_ArcRPM",        -18,    0, 240,240, "CENTER"),
    ("ui_Image2",       -262,    0, 240,256, "CENTER"),
]

print("=== eez002 Absolute positions (in 800x256 screen) ===")
print(f"{'Widget':<22} {'Align':<10} {'Abs X range':>16} {'Abs Y range':>16}")
print("-" * 68)

for name, left, top, w, h, align in widgets_eez002:
    if align == "CENTER":
        # Center of widget = container_center + offset
        cx = CX + left
        cy = CY + top
        abs_x1 = 18 + cx - w/2
        abs_y1 = cy - h/2
        abs_x2 = abs_x1 + w
        abs_y2 = abs_y1 + h
    elif align == "RIGHT_MID":
        # Right edge of widget = parent_right + left
        right_edge = PW + left  # in container coords
        cy = CY + top
        abs_x2 = 18 + right_edge
        abs_x1 = abs_x2 - w
        abs_y1 = cy - h/2
        abs_y2 = abs_y1 + h

    print(f"{name:<22} {align:<10} x={abs_x1:6.1f}-{abs_x2:6.1f}  y={abs_y1:6.1f}-{abs_y2:6.1f}")
