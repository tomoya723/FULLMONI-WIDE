"""Generate sensor icons for Prodrive dashboard (eez003).
Creates 24x24 pixel monochrome icons with transparent background.
Output: PNG files in eez003 assets directory.
"""
import math
from PIL import Image, ImageDraw

OUT_DIR = r"C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\eez\eez003"
CYAN = (0, 224, 255, 255)
TRANSP = (0, 0, 0, 0)
SZ = 24  # icon size


def save(img, name):
    path = f"{OUT_DIR}/{name}.png"
    img.save(path)
    print(f"  {name}.png ({img.size[0]}x{img.size[1]})")


def icon_water_temp():
    """Thermometer icon for water temperature."""
    img = Image.new("RGBA", (SZ, SZ), TRANSP)
    d = ImageDraw.Draw(img)
    # Bulb at bottom
    d.ellipse([7, 16, 17, 24], fill=CYAN)
    # Stem
    d.rectangle([10, 2, 14, 18], fill=CYAN)
    # Cap
    d.rectangle([9, 1, 15, 3], fill=CYAN)
    # Liquid level marks
    for y in [6, 9, 12]:
        d.line([16, y, 19, y], fill=CYAN, width=1)
    save(img, "ico_water_temp")


def icon_oil_temp():
    """Oil drop icon for oil temperature."""
    img = Image.new("RGBA", (SZ, SZ), TRANSP)
    d = ImageDraw.Draw(img)
    # Oil drop shape
    pts = [(12, 2), (18, 13), (17, 17), (14, 20), (10, 20), (7, 17), (6, 13)]
    d.polygon(pts, fill=CYAN)
    # Inner highlight (darker)
    d.ellipse([9, 12, 15, 19], fill=(0, 180, 210, 255))
    save(img, "ico_oil_temp")


def icon_exhaust_temp():
    """Wavy line icon for exhaust/intake air temperature."""
    img = Image.new("RGBA", (SZ, SZ), TRANSP)
    d = ImageDraw.Draw(img)
    # Two wavy lines (heat waves)
    for base_y in [8, 15]:
        pts = []
        for x in range(2, 22):
            y = base_y + int(3 * math.sin((x - 2) * math.pi / 5))
            pts.append((x, y))
        d.line(pts, fill=CYAN, width=2)
    save(img, "ico_exhaust_temp")


def icon_map():
    """Pressure wave icon for MAP (manifold absolute pressure)."""
    img = Image.new("RGBA", (SZ, SZ), TRANSP)
    d = ImageDraw.Draw(img)
    # Sine wave (pressure)
    pts = []
    for x in range(1, 23):
        y = 12 + int(7 * math.sin((x - 1) * 2 * math.pi / 11))
        pts.append((x, y))
    d.line(pts, fill=CYAN, width=2)
    # Baseline
    d.line([1, 22, 23, 22], fill=CYAN, width=1)
    save(img, "ico_map")


def icon_fuel_press():
    """Double wave / fuel nozzle icon for fuel pressure."""
    img = Image.new("RGBA", (SZ, SZ), TRANSP)
    d = ImageDraw.Draw(img)
    # Fuel pump body
    d.rectangle([4, 6, 14, 20], outline=CYAN, width=2)
    # Nozzle
    d.rectangle([14, 10, 20, 14], fill=CYAN)
    d.line([20, 10, 20, 6], fill=CYAN, width=2)
    # Fill level
    d.rectangle([6, 12, 12, 18], fill=CYAN)
    save(img, "ico_fuel_press")


def icon_battery():
    """Battery icon."""
    img = Image.new("RGBA", (SZ, SZ), TRANSP)
    d = ImageDraw.Draw(img)
    # Battery body
    d.rectangle([2, 8, 20, 20], outline=CYAN, width=2)
    # Terminal
    d.rectangle([8, 5, 14, 9], fill=CYAN)
    # + and - marks
    # Plus
    d.line([6, 13, 10, 13], fill=CYAN, width=1)
    d.line([8, 11, 8, 15], fill=CYAN, width=1)
    # Minus
    d.line([14, 13, 18, 13], fill=CYAN, width=1)
    save(img, "ico_battery")


def tacho_bg():
    """Tachometer number ring background (transparent with number marks).
    320x155 to match ui_container_tacho size.
    Numbers 0-9 in a fan arc + tick marks.
    """
    W, H = 320, 155
    img = Image.new("RGBA", (W, H), TRANSP)
    d = ImageDraw.Draw(img)

    # Fan center is below the visible area (like a real round gauge seen from above)
    cx, cy = 160, 380
    r_num = 340   # radius for numbers
    r_tick_out = 320
    r_tick_in = 305

    # Angle sweep: from 228 deg to 312 deg (top-ish arc)
    # In image coords: 0deg = right, 90deg = down
    # We want the arc from about 220 to 320, numbers 0-9 (x1000 RPM)
    start_angle = 222
    end_angle = 318
    n_major = 10  # 0..9

    DIM_CYAN = (0, 224, 255, 120)

    for i in range(n_major):
        frac = i / (n_major - 1)
        angle_deg = start_angle + frac * (end_angle - start_angle)
        angle_rad = math.radians(angle_deg)

        # Number position
        nx = cx + r_num * math.cos(angle_rad)
        ny = cy + r_num * math.sin(angle_rad)

        # Only draw if visible in the image bounds
        if 0 <= nx <= W and 0 <= ny <= H:
            # Draw number
            txt = str(i)
            # Use simple rectangle as text placeholder (Pillow default font is tiny)
            # We'll use the built-in font
            d.text((nx - 4, ny - 6), txt, fill=DIM_CYAN)

        # Tick mark
        tx1 = cx + r_tick_out * math.cos(angle_rad)
        ty1 = cy + r_tick_out * math.sin(angle_rad)
        tx2 = cx + r_tick_in * math.cos(angle_rad)
        ty2 = cy + r_tick_in * math.sin(angle_rad)
        if 0 <= tx1 <= W or 0 <= tx2 <= W:
            d.line([(tx1, ty1), (tx2, ty2)], fill=DIM_CYAN, width=2)

        # Minor ticks (5 per segment except last)
        if i < n_major - 1:
            for j in range(1, 5):
                mfrac = (i + j / 5) / (n_major - 1)
                mangle = math.radians(start_angle + mfrac * (end_angle - start_angle))
                mx1 = cx + r_tick_out * math.cos(mangle)
                my1 = cy + r_tick_out * math.sin(mangle)
                mx2 = cx + (r_tick_in + 8) * math.cos(mangle)
                my2 = cy + (r_tick_in + 8) * math.sin(mangle)
                if 0 <= mx1 <= W or 0 <= mx2 <= W:
                    d.line([(mx1, my1), (mx2, my2)], fill=(0, 224, 255, 60), width=1)

    save(img, "tacho_bg")


def afr_box_bg():
    """AFR display box background with border.
    A dark rectangle with cyan border, sized for 'AFR 14.7' text.
    """
    W, H = 140, 36
    img = Image.new("RGBA", (W, H), TRANSP)
    d = ImageDraw.Draw(img)
    # Dark fill with cyan border
    d.rectangle([0, 0, W - 1, H - 1], fill=(10, 20, 30, 200), outline=CYAN, width=2)
    save(img, "afr_box_bg")


def main():
    print("Generating Prodrive icons...")
    icon_water_temp()
    icon_oil_temp()
    icon_exhaust_temp()
    icon_map()
    icon_fuel_press()
    icon_battery()
    tacho_bg()
    afr_box_bg()
    print("Done!")


if __name__ == "__main__":
    main()
