#!/usr/bin/env python3
"""
FULLMONI Bootloader Firmware Upload Tool
USB CDC経由でファームウェアをアップロード
"""
import serial
import sys
import time
import os

def upload_firmware(port, firmware_path, baudrate=115200):
    """ファームウェアをアップロード"""

    if not os.path.exists(firmware_path):
        print(f"Error: File not found: {firmware_path}")
        return False

    file_size = os.path.getsize(firmware_path)
    print(f"Firmware: {firmware_path}")
    print(f"Size: {file_size:,} bytes ({file_size/1024:.1f} KB)")

    try:
        ser = serial.Serial(port, baudrate, timeout=2, write_timeout=5)
        print(f"Connected to {port}")
        time.sleep(0.5)

        # 既存のデータをフラッシュ
        ser.reset_input_buffer()

        # Uコマンドを送信してEraseを開始
        print("\nSending 'U' command to erase Flash...")
        ser.write(b'U')

        # Erase完了を待つ (最大60秒)
        response = b''
        start_time = time.time()
        while time.time() - start_time < 60:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                response += data
                print(data.decode('utf-8', errors='replace'), end='', flush=True)
                if b'ESC to finish' in response:
                    break
                if b'Fail' in response:
                    print("\nErase failed!")
                    ser.close()
                    return False
            time.sleep(0.1)

        if b'Send size+firmware' not in response:
            print("\nTimeout waiting for erase completion")
            ser.close()
            return False

        print("\n\nStarting firmware transfer...")

        # ファームウェアを読み込み
        with open(firmware_path, 'rb') as f:
            firmware_data = f.read()

        # 最初に4バイトのサイズを送信 (little-endian)
        size_bytes = file_size.to_bytes(4, 'little')
        ser.write(size_bytes)

        # ACK待ち (サイズ受信後のみ)
        ack = ser.read(1)
        if ack != b'.':
            print(f"Size ACK failed: {ack}")
            ser.close()
            return False
        print(f"Size ACK received. Starting streaming transfer...")

        # ストリーミングモード - 中間ACKなしで高速転送
        # チャンクサイズ (16KB = ホスト側送信単位)
        chunk_size = 16 * 1024
        sent = 0
        start_time = time.time()

        for i in range(0, len(firmware_data), chunk_size):
            chunk = firmware_data[i:i+chunk_size]
            ser.write(chunk)
            sent += len(chunk)

            # 進捗表示
            progress = (sent / file_size) * 100
            kb_sent = sent / 1024
            elapsed = time.time() - start_time
            speed = kb_sent / elapsed if elapsed > 0 else 0
            print(f"\rProgress: {progress:5.1f}% ({kb_sent:.1f} KB / {file_size/1024:.1f} KB) - {speed:.1f} KB/s", end='', flush=True)

            # 小さな遅延でUSBバッファを安定化
            time.sleep(0.01)

        print("\n\nWaiting for completion message...")

        # 完了メッセージを待つ (最大30秒)
        ser.timeout = 30
        response = b''
        while True:
            data = ser.read(1)
            if not data:
                break
            response += data
            if b'Done' in response or b'ERR' in response:
                # 残りを読む
                time.sleep(0.3)
                if ser.in_waiting:
                    response += ser.read(ser.in_waiting)
                break

        if response:
            print(f"Response: {response.decode('utf-8', errors='replace')}")

        if b'Done' in response:
            pass  # Success
        elif b'ERR' in response:
            print("Flash write error!")
            ser.close()
            return False
        else:
            print("Warning: No completion message received")

        elapsed = time.time() - start_time
        speed = (file_size / 1024) / elapsed if elapsed > 0 else 0
        print(f"\nTransfer complete! ({elapsed:.1f}s, {speed:.1f} KB/s)")

        ser.close()
        print("\nUpload completed!")
        return True

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return False
    except KeyboardInterrupt:
        print("\nCancelled by user")
        return False

def main():
    if len(sys.argv) < 3:
        print("Usage: python fw_upload.py <COM_PORT> <FIRMWARE_FILE>")
        print("Example: python fw_upload.py COM7 Firmware.bin")
        sys.exit(1)

    port = sys.argv[1]
    firmware = sys.argv[2]

    if not upload_firmware(port, firmware):
        sys.exit(1)

if __name__ == '__main__':
    main()
