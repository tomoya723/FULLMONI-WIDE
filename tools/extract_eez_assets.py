#!/usr/bin/env python3
"""
EEZ Studio生成のui.cからassets[]バイナリを抽出する

使い方:
  python extract_eez_assets.py [ui.c のパス] [出力ファイル名]

デフォルト:
  入力: Firmware/src/ui/ui.c (または Firmware/eez/src/ui/ui.c)
  出力: eez_assets.bin
"""

import re
import sys
import os


def extract_assets(ui_c_path: str, output_path: str) -> None:
    with open(ui_c_path, "r", encoding="utf-8") as f:
        content = f.read()

    # assets[] 配列の検出
    match = re.search(
        r"const\s+uint8_t\s+assets\[(\d+)\]\s*=\s*\{([^}]+)\}", content, re.DOTALL
    )
    if not match:
        print("ERROR: assets[] array not found in", ui_c_path)
        sys.exit(1)

    size = int(match.group(1))
    hex_data = match.group(2)

    # 16進数値を抽出
    values = re.findall(r"0x([0-9A-Fa-f]{2})", hex_data)
    if len(values) != size:
        print(f"WARNING: Expected {size} bytes, found {len(values)}")

    binary = bytes(int(v, 16) for v in values)

    # EEZヘッダー検証
    if len(binary) >= 4:
        tag = int.from_bytes(binary[:4], "little")
        if tag == 0x5A45457E:
            print(f"EEZ header: OK (uncompressed, tag=0x{tag:08X})")
        elif tag == 0x7A65657E:
            print(f"EEZ header: OK (compressed, tag=0x{tag:08X})")
        else:
            print(f"WARNING: Unknown header tag=0x{tag:08X}")

    with open(output_path, "wb") as f:
        f.write(binary)

    print(f"Extracted {len(binary)} bytes to {output_path}")


if __name__ == "__main__":
    # デフォルトパス
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)

    default_inputs = [
        os.path.join(project_root, "Firmware", "src", "ui", "ui.c"),
    ]

    ui_c_path = sys.argv[1] if len(sys.argv) > 1 else None
    output_path = sys.argv[2] if len(sys.argv) > 2 else "eez_assets.bin"

    if ui_c_path is None:
        for p in default_inputs:
            if os.path.exists(p):
                ui_c_path = p
                break

    if ui_c_path is None or not os.path.exists(ui_c_path):
        print("ERROR: ui.c not found")
        sys.exit(1)

    print(f"Input: {ui_c_path}")
    extract_assets(ui_c_path, output_path)
