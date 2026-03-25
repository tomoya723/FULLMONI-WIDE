#!/usr/bin/env python3
"""Update fullmoni.eez-project coordinates to match aw002 AppWizard layout.

aw002 display layout (800x256):
  Telltale:  x=0,   w=35   (warning icons)
  01b:       x=35,  w=240  (bar graphs + labels)
  01a:       x=275, w=340  (gauge + RPM + gear)
  01c:       x=615, w=185  (speed/odo/trip/fuel)

  Opening:   x=35,  w=765  (splash image, shown first)

All EEZ widgets use CENTER alignment within their parent.
CENTER offset formula:
  left  = abs_x + w/2 - parent_w/2
  top   = abs_y + h/2 - parent_h/2
"""
import json
import sys
import shutil
from pathlib import Path

PROJ_PATH = Path(__file__).resolve().parent.parent / "Firmware" / "eez" / "fullmoni.eez-project"


def find_widget(children, identifier):
    """Recursively find a widget by identifier."""
    for w in children:
        if w.get("identifier") == identifier:
            return w
        if "children" in w:
            r = find_widget(w["children"], identifier)
            if r:
                return r
    return None


def update_widget(children, identifier, **kwargs):
    """Find a widget and update its properties."""
    w = find_widget(children, identifier)
    if not w:
        print(f"  WARNING: widget '{identifier}' not found")
        return False
    for k, v in kwargs.items():
        old = w.get(k, "N/A")
        w[k] = v
        if old != v:
            print(f"  {identifier}: {k} {old} -> {v}")
    return True


def center_offset(abs_x, abs_y, w, h, parent_w, parent_h):
    """Calculate CENTER alignment offsets from absolute position."""
    left = round(abs_x + w / 2 - parent_w / 2)
    top = round(abs_y + h / 2 - parent_h / 2)
    return left, top


def main():
    # Load project
    with open(PROJ_PATH, "r", encoding="utf-8") as f:
        proj = json.load(f)

    screen = proj["userPages"][0]["components"][0]  # LVGLScreenWidget
    children = screen["children"]

    SCR_W, SCR_H = 800, 256
    DASH_W, DASH_H = 765, 256
    TELL_W, TELL_H = 35, 256

    # ═══════════════════════════════════════════════════════════════════
    # 1. Top-level container positions (within 800x256 screen)
    # ═══════════════════════════════════════════════════════════════════
    print("=== Container positions (within 800x256 screen) ===")

    # Telltale: absolute (0, 0), size 35x256
    tl, tt = center_offset(0, 0, TELL_W, TELL_H, SCR_W, SCR_H)
    update_widget(children, "ui_ContainerTelltale", left=tl, top=tt, width=TELL_W)

    # Dashboard: absolute (35, 0), size 765x256
    dl, dt = center_offset(35, 0, DASH_W, DASH_H, SCR_W, SCR_H)
    update_widget(children, "ui_ContainerDashboard", left=dl, top=dt)

    # Opening: same area as dashboard
    update_widget(children, "ui_ContainerOpening", left=dl, top=dt)

    # ═══════════════════════════════════════════════════════════════════
    # 2. Telltale icons (within 35x256 panel, CENTER aligned)
    #    aw002: all icons 25x25, x=5 (centered in 35px), y from top
    # ═══════════════════════════════════════════════════════════════════
    print("\n=== Telltale icon positions ===")
    icon_positions = {
        "ui_ImgWarnMaster":    10,   # y=10
        "ui_ImgWarnOilPress":  40,   # y=40
        "ui_ImgWarnWaterCold": 70,   # y=70
        "ui_ImgWarnWaterHot":  70,   # y=70 (overlaps cold)
        "ui_ImgWarnExhaust":   100,  # y=100
        "ui_ImgWarnBattery":   130,  # y=130
        "ui_ImgWarnBrake":     160,  # y=160
        "ui_ImageWarnBelt":    190,  # y=190
        "ui_ImgWarnFuel":      220,  # y=220
    }
    for name, ay in icon_positions.items():
        il, it = center_offset(5, ay, 25, 25, TELL_W, TELL_H)
        update_widget(children, name, left=il, top=it)

    # ═══════════════════════════════════════════════════════════════════
    # 3. Dashboard widgets (within 765x256 panel, CENTER aligned)
    # ═══════════════════════════════════════════════════════════════════
    print("\n=== Dashboard widgets - 01b area (bars/labels, dashboard x=0-240) ===")

    # ── 01b area: bar graph panel (dashboard relative x=0, 240px wide) ──
    # Background image Back6.bmp
    l, t = center_offset(0, 0, 240, 256, DASH_W, DASH_H)
    update_widget(children, "ui_Image2", left=l, top=t, width=240, height=256)

    # aw002 bars: 80x10-12 at x=43, labels 60-68x28 at x=128-135
    bars_01b = [
        # (identifier, ax, ay, w, h)
        ("ui_BarWaterTemp",  43,  21,  80, 10),
        ("ui_BarIAT",        43,  64,  80, 10),
        ("ui_BarOilTemp",    43, 107,  80, 10),
        ("ui_BarMAP",        43, 146,  80, 10),
        ("ui_BarOilPress",   43, 185,  80, 10),
        ("ui_BarBattery",    43, 224,  80, 10),
    ]
    labels_01b = [
        ("ui_LblWaterTemp",  135,  21,  60, 28),
        ("ui_LblIAT",        135,  61,  60, 28),
        ("ui_LblOilTemp",    135, 103,  60, 28),
        ("ui_LblMAP",        135, 143,  60, 28),
        ("ui_LblOilPress",   128, 180,  67, 28),
        ("ui_LblBattery",    127, 215,  68, 32),
    ]
    for name, ax, ay, w, h in bars_01b + labels_01b:
        l, t = center_offset(ax, ay, w, h, DASH_W, DASH_H)
        update_widget(children, name, left=l, top=t, width=w, height=h)

    # ── 01a area: gauge panel (dashboard relative x=240, 340px wide) ──
    print("\n=== Dashboard widgets - 01a area (gauge, dashboard x=240-580) ===")

    # Background Back5.bmp 340x256
    l, t = center_offset(240, 0, 340, 256, DASH_W, DASH_H)
    update_widget(children, "ui_ImgTacho", left=l, top=t)

    # Gauge/Arc 256x256 at 01a-relative (-3, 0) → dashboard (237, 0)
    l, t = center_offset(237, 0, 256, 256, DASH_W, DASH_H)
    update_widget(children, "ui_ArcRPM", left=l, top=t, width=256, height=256)

    # Rotary needles overlay gauge at same position 256x256
    # These are image widgets with rotation, keep same position as gauge
    # but their actual size is the marker image (80x7 and 20x7),
    # the 256x256 is the rotation area. In LVGL these are positioned differently.
    # Keep the image widget sizes but move to correct position.
    # The pivot point for rotation needs the center of the gauge.
    gauge_cx = 237 + 128  # = 365 (center of gauge in dashboard coords)
    gauge_cy = 128        # center Y

    # ui_imageRPM: 80x7 RPM needle, pivot should be at gauge center
    # Position so that needle base is at gauge center
    # Current: left=96, top=3, width=80, height=7
    # With CENTER: abs_x = 382.5+96-40 = 438.5, which is 438.5-237=201.5 within 01a
    # In aw002 the rotary is centered on the gauge center (which is at 01a-relative 128, 128)
    # Keep current sizes but adjust position
    l_rpm, t_rpm = center_offset(gauge_cx - 40, gauge_cy - 3, 80, 7, DASH_W, DASH_H)
    update_widget(children, "ui_imageRPM", left=l_rpm, top=t_rpm)

    l_peak, t_peak = center_offset(gauge_cx - 10, gauge_cy - 3, 20, 7, DASH_W, DASH_H)
    update_widget(children, "ui_ImagePeakRPM", left=l_peak, top=t_peak)

    # Text widgets in 01a (positions relative to 01a origin at dashboard x=240)
    texts_01a = [
        # (identifier, ax_in_01a, ay_in_01a, w, h)
        ("ui_LblAFR",   193, 193,  130,  39),  # NUM_AF
        ("ui_LblGEAR",  256,  72,  100, 100),  # NUM_GEAR
        ("ui_LblRPM",   125, 145,  100,  28),  # NUM_RV (rpm value)
        ("ui_LblTIME",  270,   0,   94,  31),  # NUM_TIME
    ]
    for name, ax, ay, w, h in texts_01a:
        l, t = center_offset(240 + ax, ay, w, h, DASH_W, DASH_H)
        update_widget(children, name, left=l, top=t, width=w, height=h)

    # RPM unit label (not in aw002, position near LblRPM)
    l, t = center_offset(240 + 225, 155, 22, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblRPMUnit", left=l, top=t)

    # AFR progress bars in 01a area
    print("\n=== Dashboard widgets - 01a AFR bars ===")
    # PROGBAR_AFR_WARN: 139x3 at (187, 237) in 01a
    # Note: these don't exist in current eez as separate bars.
    # PROGBAR_AFR: 139x6 at (187, 230) in 01a
    # Map to any existing AFR bar widgets if they exist
    # (Currently no AFR bar in eez-project, skip)

    # ── 01c area: speed/odo panel (dashboard relative x=580, 185px wide) ──
    print("\n=== Dashboard widgets - 01c area (speed/odo, dashboard x=580-765) ===")

    # From generated C code, positions within 01c (185x256):
    # NUM_FPS:   x=185-42-32=111, y=0,   size 32x17  (no eez widget exists)
    # NUM_SPEED: x=185-61-100=24, y=150, size 100x28
    # NUM_TRIP:  x=185-32-50=103, y=190, size 50x20
    # NUM_ODO:   x=185-32-60=93,  y=209, size 60x20
    # PROGBAR:   x=185-53-100=32, y=234, size 100x15

    texts_01c = [
        ("ui_LblSPD",    24,  150, 100, 28),
        ("ui_LblTrip",  103,  190,  50, 20),
        ("ui_LblODO",    93,  209,  60, 20),
    ]
    for name, ax, ay, w, h in texts_01c:
        l, t = center_offset(580 + ax, ay, w, h, DASH_W, DASH_H)
        update_widget(children, name, left=l, top=t, width=w, height=h)

    # Fuel bar
    l, t = center_offset(580 + 32, 234, 100, 15, DASH_W, DASH_H)
    update_widget(children, "ui_BarFUEL", left=l, top=t, width=100, height=15)

    # Auxiliary labels (not in aw002, position relative to parent values)
    print("\n=== Auxiliary labels (positioned near associated values) ===")
    # SPD unit: right of speed value
    l, t = center_offset(580 + 124, 155, 42, 24, DASH_W, DASH_H)
    update_widget(children, "ui_LblSPDUnit", left=l, top=t)

    # Trip name/unit: near trip value
    l, t = center_offset(580 + 73, 190, 24, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblTripName", left=l, top=t)
    l, t = center_offset(580 + 155, 192, 16, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblTripUnit", left=l, top=t)

    # ODO name/unit: near odo value
    l, t = center_offset(580 + 63, 209, 21, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblODOName", left=l, top=t)
    l, t = center_offset(580 + 155, 211, 16, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblODOUnit", left=l, top=t)

    # Fuel E/F labels: near fuel bar
    l, t = center_offset(580 + 20, 236, 6, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblEmpty", left=l, top=t)
    l, t = center_offset(580 + 134, 236, 6, 12, DASH_W, DASH_H)
    update_widget(children, "ui_LblFull", left=l, top=t)

    # ═══════════════════════════════════════════════════════════════════
    # Save
    # ═══════════════════════════════════════════════════════════════════
    backup = PROJ_PATH.with_suffix(".eez-project.bak")
    shutil.copy2(PROJ_PATH, backup)
    print(f"\nBackup saved to: {backup}")

    with open(PROJ_PATH, "w", encoding="utf-8") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)
    print(f"Updated project saved to: {PROJ_PATH}")


if __name__ == "__main__":
    main()
