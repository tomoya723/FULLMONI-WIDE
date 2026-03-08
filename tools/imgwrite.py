#!/usr/bin/env python3
"""
FULLMONI imgwrite - 起動画像書き込みツール
USB CDC経由でBMP画像をファームウェアのフラッシュに書き込む。

変換処理:
  1. BMP読み込み (24bit/16bit対応)
  2. 765x256にリサイズ (必要な場合)
  3. 行順反転 (BMP下→上 → LVGL上→下)
  4. BGR24→RGB565変換
  5. 391,680バイトのRAW RGB565データをファームウェアに送信

使用方法:
  python imgwrite.py <COMポート> <BMPファイル> [--baudrate 115200]

例:
  python imgwrite.py COM5 opening.bmp
  python imgwrite.py /dev/ttyACM0 opening.bmp --baudrate 115200

プロトコル (ファームウェアと対応):
  1. Host → FW: total_size (4バイト, little-endian) = 391680
  2. FW → Host: ACK (0x06)
  3. [消去完了後]
  4. FW → Host: ACK (0x06)
  5. ループ (256バイト単位):
     Host → FW: chunk (256バイト)
     FW → Host: ACK (0x06)
  6. 完了
"""

import sys
import os
import struct
import time
import argparse

try:
    import serial
except ImportError:
    print("Error: pyserial が必要です。")
    print("  pip install pyserial")
    sys.exit(1)

try:
    from PIL import Image
    HAS_PIL = True
except ImportError:
    HAS_PIL = False

# ファームウェア定義と合わせる
TARGET_WIDTH  = 765
TARGET_HEIGHT = 256
TARGET_SIZE   = TARGET_WIDTH * TARGET_HEIGHT * 2  # RGB565: 391,680 bytes
CHUNK_SIZE    = 256
ACK_CHAR      = 0x06
ACK_TIMEOUT   = 60.0  # 消去に最大60秒


def bmp_to_rgb565(bmp_path: str) -> bytes:
    """
    BMPファイルをLVGL用RGB565データ (top-to-bottom) に変換する。

    PIL (Pillow) がある場合はPIL経由で処理 (任意サイズ対応)。
    ない場合は生のBMPバイト列を直接解析する (765x256 24bit固定)。
    """
    if HAS_PIL:
        return _bmp_to_rgb565_pil(bmp_path)
    else:
        return _bmp_to_rgb565_raw(bmp_path)


def _bmp_to_rgb565_pil(bmp_path: str) -> bytes:
    """PIL使用版: 任意サイズ・形式に対応"""
    img = Image.open(bmp_path).convert("RGB")

    w, h = img.size
    if w != TARGET_WIDTH or h != TARGET_HEIGHT:
        print(f"  リサイズ: {w}x{h} → {TARGET_WIDTH}x{TARGET_HEIGHT}")
        img = img.resize((TARGET_WIDTH, TARGET_HEIGHT), Image.LANCZOS)

    # PIL は top-to-bottom で格納。そのまま変換する。
    pixels = img.load()
    buf = bytearray(TARGET_SIZE)
    idx = 0
    for row in range(TARGET_HEIGHT):
        for col in range(TARGET_WIDTH):
            r, g, b = pixels[col, row]
            rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
            buf[idx]     = rgb565 & 0xFF          # little-endian
            buf[idx + 1] = (rgb565 >> 8) & 0xFF
            idx += 2
    return bytes(buf)


def _bmp_to_rgb565_raw(bmp_path: str) -> bytes:
    """
    PILなし版: 生のBMPバイト列を直接解析する。
    765x256, 24bitBMP専用。
    """
    with open(bmp_path, "rb") as f:
        data = f.read()

    # BMP シグネチャ確認
    if len(data) < 54 or data[0:2] != b'BM':
        raise ValueError("BMP形式ではありません (PILをインストールすると他の形式も対応)")

    # BMP ヘッダ解析
    pixel_offset = struct.unpack_from("<I", data, 10)[0]
    width        = struct.unpack_from("<i", data, 18)[0]
    height_raw   = struct.unpack_from("<i", data, 22)[0]
    bpp          = struct.unpack_from("<H", data, 28)[0]

    top_to_bottom = height_raw < 0  # 負の高さ = top-to-bottom BMP
    height = abs(height_raw)

    if width != TARGET_WIDTH or height != TARGET_HEIGHT:
        raise ValueError(
            f"画像サイズが不正: {width}x{height} (期待値: {TARGET_WIDTH}x{TARGET_HEIGHT})\n"
            "  Pillowをインストールすると自動リサイズに対応します: pip install Pillow"
        )
    if bpp != 24:
        raise ValueError(
            f"24bitBMPのみ対応 (現在: {bpp}bit)\n"
            "  Pillowをインストールすると他のビット深度に対応します: pip install Pillow"
        )

    # 行ストライド (4バイト境界)
    row_bytes = width * 3
    row_stride = (row_bytes + 3) & ~3

    buf = bytearray(TARGET_SIZE)
    idx = 0

    for row_idx in range(height):
        if top_to_bottom:
            # top-to-bottom BMP: そのまま
            src_row = row_idx
        else:
            # bottom-to-top BMP (標準): 末尾行から読む
            src_row = height - 1 - row_idx

        row_start = pixel_offset + src_row * row_stride
        for col in range(width):
            px = row_start + col * 3
            b = data[px]
            g = data[px + 1]
            r = data[px + 2]
            rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
            buf[idx]     = rgb565 & 0xFF
            buf[idx + 1] = (rgb565 >> 8) & 0xFF
            idx += 2

    return bytes(buf)


def wait_ack(ser: serial.Serial, timeout: float = ACK_TIMEOUT) -> bool:
    """ACK (0x06) を受信するまで待機する。タイムアウト時はFalseを返す。"""
    start = time.time()
    while time.time() - start < timeout:
        b = ser.read(1)
        if len(b) > 0:
            if b[0] == ACK_CHAR:
                return True
            # ACK以外のバイト (ログ文字列等) は無視
    return False


def imgwrite(port: str, bmp_path: str, baudrate: int = 115200) -> bool:
    """
    BMPファイルをRGB565に変換してファームウェアに書き込む。
    成功時True、失敗時Falseを返す。
    """
    print(f"=== FULLMONI imgwrite ===")
    print(f"ポート  : {port}")
    print(f"画像    : {bmp_path}")

    # 変換
    print("変換中...")
    try:
        pixel_data = bmp_to_rgb565(bmp_path)
    except Exception as e:
        print(f"変換エラー: {e}")
        return False

    assert len(pixel_data) == TARGET_SIZE, f"変換後サイズ異常: {len(pixel_data)}"
    print(f"変換完了: {len(pixel_data):,} bytes ({TARGET_WIDTH}x{TARGET_HEIGHT} RGB565)")

    # シリアル接続
    try:
        ser = serial.Serial(port, baudrate, timeout=2, write_timeout=5)
        print(f"接続: {port} @ {baudrate}bps")
        time.sleep(0.3)
        ser.reset_input_buffer()
    except serial.SerialException as e:
        print(f"接続エラー: {e}")
        return False

    try:
        # 1. サイズ送信 (4バイト little-endian)
        size_bytes = struct.pack("<I", len(pixel_data))
        ser.write(size_bytes)
        ser.flush()
        print(f"サイズ送信: {len(pixel_data)} bytes")

        # 2. ACK受信 (消去開始前)
        print("ACK待ち (消去開始)...")
        if not wait_ack(ser, timeout=10.0):
            print("ERR: ACK受信タイムアウト (サイズ応答)")
            return False

        # 3. ACK受信 (消去完了後)
        print("消去完了待ち (最大60秒)...")
        if not wait_ack(ser, timeout=ACK_TIMEOUT):
            print("ERR: ACK受信タイムアウト (消去完了)")
            return False
        print("消去完了")

        # 4. データ送信 (256バイトチャンク単位)
        total = len(pixel_data)
        sent  = 0
        chunk_num = 0
        last_pct = -1

        print("データ送信中...")
        while sent < total:
            end = min(sent + CHUNK_SIZE, total)
            chunk = pixel_data[sent:end]
            ser.write(chunk)
            ser.flush()

            if not wait_ack(ser, timeout=10.0):
                print(f"\nERR: ACK受信タイムアウト (チャンク {chunk_num}, offset {sent})")
                return False

            sent += len(chunk)
            chunk_num += 1

            pct = sent * 100 // total
            if pct != last_pct and pct % 5 == 0:
                print(f"  {pct:3d}% ({sent:,}/{total:,} bytes)", end="\r")
                last_pct = pct

        print(f"\n書き込み完了: {sent:,} bytes")
        print("デバイスが再起動します...")
        time.sleep(3.0)

    finally:
        ser.close()

    return True


def main():
    parser = argparse.ArgumentParser(
        description="FULLMONI 起動画像書き込みツール (BMP → RGB565 → Flash)"
    )
    parser.add_argument("port",     help="シリアルポート (例: COM5, /dev/ttyACM0)")
    parser.add_argument("bmp",      help="入力BMPファイル (765x256推奨, 24bit)")
    parser.add_argument("--baudrate", type=int, default=115200, help="ボーレート (既定: 115200)")
    args = parser.parse_args()

    if not os.path.isfile(args.bmp):
        print(f"Error: ファイルが見つかりません: {args.bmp}")
        sys.exit(1)

    ok = imgwrite(args.port, args.bmp, args.baudrate)
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
