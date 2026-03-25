#!/usr/bin/env python3
"""Add missing images (Back7, w_back) to fullmoni.eez-project.

Back7.bmp (185x256) - 01c speed panel background
w_back.png (35x256) - telltale panel background
"""
import json
import uuid
import base64
import shutil
from pathlib import Path

PROJ_PATH = Path(__file__).resolve().parent.parent / "Firmware" / "eez" / "fullmoni.eez-project"
RES_DIR = Path(__file__).resolve().parent.parent / "Firmware" / "eez" / "aw002" / "Resource" / "Image"


def image_to_data_uri(path):
    """Convert an image file to a data: URI for EEZ embedding."""
    with open(path, "rb") as f:
        data = f.read()
    ext = path.suffix.lower()
    mime = {"png": "image/png", "bmp": "image/bmp", "jpg": "image/jpeg"}.get(ext.lstrip("."), "image/png")
    return f"data:{mime};base64,{base64.b64encode(data).decode()}"


def make_bitmap(name, image_path):
    return {
        "objID": str(uuid.uuid4()),
        "name": name,
        "image": image_to_data_uri(image_path),
        "bpp": 16,
        "alwaysBuild": True,
    }


def find_widget(children, identifier):
    for w in children:
        if w.get("identifier") == identifier:
            return w
        if "children" in w:
            r = find_widget(w["children"], identifier)
            if r:
                return r
    return None


def center_offset(abs_x, abs_y, w, h, parent_w, parent_h):
    left = round(abs_x + w / 2 - parent_w / 2)
    top = round(abs_y + h / 2 - parent_h / 2)
    return left, top


def make_image_widget(identifier, bitmap_name, left, top, width, height):
    return {
        "objID": str(uuid.uuid4()),
        "type": "LVGLImageWidget",
        "identifier": identifier,
        "left": left,
        "top": top,
        "width": width,
        "height": height,
        "children": [],
        "localStyles": {
            "objID": str(uuid.uuid4()),
            "definition": {
                "MAIN": {
                    "DEFAULT": {
                        "align": "CENTER"
                    }
                }
            },
        },
        "bitmap": bitmap_name,
        "hiddenFlagType": "NONE",
        "clickableFlag": False,
    }


def main():
    with open(PROJ_PATH, "r", encoding="utf-8") as f:
        proj = json.load(f)

    bitmaps = proj.get("bitmaps", [])
    bitmap_names = {b["name"] for b in bitmaps}

    # ─── 1. Add bitmap resources ──────────────────────────────────
    print("=== Adding bitmap resources ===")

    # Back7.png for 01c background
    back7_png = RES_DIR / "Back7.png"
    if "ui_img_back7_png" not in bitmap_names:
        bitmaps.append(make_bitmap("ui_img_back7_png", back7_png))
        print(f"  Added: ui_img_back7_png ({back7_png})")
    else:
        print("  Already exists: ui_img_back7_png")

    # w_back.png for telltale background
    wback_png = RES_DIR / "w_back.png"
    if "ui_img_w_back_png" not in bitmap_names:
        bitmaps.append(make_bitmap("ui_img_w_back_png", wback_png))
        print(f"  Added: ui_img_w_back_png ({wback_png})")
    else:
        print("  Already exists: ui_img_w_back_png")

    proj["bitmaps"] = bitmaps

    # ─── 2. Add image widgets ─────────────────────────────────────
    print("\n=== Adding image widgets ===")
    screen = proj["userPages"][0]["components"][0]
    children = screen["children"]

    DASH_W, DASH_H = 765, 256
    TELL_W, TELL_H = 35, 256

    # Back7 image inside ui_ContainerDashboard
    # 01c area: dashboard-relative x=580, w=185, full height
    dashboard = find_widget(children, "ui_ContainerDashboard")
    if dashboard:
        existing_back7 = find_widget(dashboard.get("children", []), "ui_ImgBack7")
        if not existing_back7:
            l, t = center_offset(580, 0, 185, 256, DASH_W, DASH_H)
            back7_widget = make_image_widget("ui_ImgBack7", "ui_img_back7_png", l, t, 185, 256)
            # Insert at beginning so it's behind other widgets
            dashboard["children"].insert(0, back7_widget)
            print(f"  Added: ui_ImgBack7 (left={l}, top={t}) in ui_ContainerDashboard")
        else:
            print("  Already exists: ui_ImgBack7")

    # w_back image inside ui_ContainerTelltale
    telltale = find_widget(children, "ui_ContainerTelltale")
    if telltale:
        existing_wback = find_widget(telltale.get("children", []), "ui_ImgTelltale")
        if not existing_wback:
            # Full-size background, centered in 35x256 panel
            wback_widget = make_image_widget("ui_ImgTelltale", "ui_img_w_back_png", 0, 0, 35, 256)
            telltale["children"].insert(0, wback_widget)
            print(f"  Added: ui_ImgTelltale (left=0, top=0) in ui_ContainerTelltale")
        else:
            print("  Already exists: ui_ImgTelltale")

    # ─── Save ────────────────────────────────────────────────────
    backup = PROJ_PATH.with_suffix(".eez-project.img-bak")
    shutil.copy2(PROJ_PATH, backup)
    print(f"\nBackup: {backup}")

    with open(PROJ_PATH, "w", encoding="utf-8") as f:
        json.dump(proj, f, indent=2, ensure_ascii=False)
    print(f"Saved: {PROJ_PATH}")


if __name__ == "__main__":
    main()
