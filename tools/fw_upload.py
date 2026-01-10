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
        
        # ACK待ち
        ack = ser.read(1)
        if ack != b'.':
            print(f"Size ACK failed: {ack}")
            ser.close()
            return False
        print(f"Size sent: {file_size} bytes")

        # チャンクサイズ (128バイト = Flash write単位)
        chunk_size = 128
        sent = 0
        start_time = time.time()

        for i in range(0, len(firmware_data), chunk_size):
            chunk = firmware_data[i:i+chunk_size]
            ser.write(chunk)
            sent += len(chunk)

            # 進捗表示
            progress = (sent / file_size) * 100
            kb_sent = sent / 1024
            print(f"\rProgress: {progress:5.1f}% ({kb_sent:.1f} KB / {file_size/1024:.1f} KB)", end='', flush=True)

            # ACK ('.')を待つ - デバイスがFlash書き込み完了後に送信
            # 最後のチャンクでは完了メッセージが返ってくる
            ack = ser.read(1)
            if ack == b'.':
                continue  # ACK received, continue
            elif ack == b'\r' or ack == b'\n' or ack == b'D':
                # Completion message started - read the rest
                time.sleep(0.3)
                remaining = ser.read(ser.in_waiting) if ser.in_waiting else b''
                print(f"\n\n{(ack + remaining).decode('utf-8', errors='replace')}")
                break  # Transfer complete
            elif ack == b'':
                print(f"\n\nTimeout waiting for ACK at {sent} bytes")
                ser.close()
                return False
            else:
                # Read remaining error message
                time.sleep(0.2)
                remaining = ser.read(ser.in_waiting) if ser.in_waiting else b''
                print(f"\n\nError at {sent} bytes: {(ack + remaining).decode('utf-8', errors='replace')}")
                ser.close()
                return False

        elapsed = time.time() - start_time
        speed = (file_size / 1024) / elapsed if elapsed > 0 else 0
        print(f"\n\nTransfer complete! ({elapsed:.1f}s, {speed:.1f} KB/s)")

        # 完了メッセージを待つ (デバイスが自動的に送信)
        time.sleep(0.5)
        response = b''
        while ser.in_waiting:
            response += ser.read(ser.in_waiting)
            time.sleep(0.1)

        if response:
            print(f"\nResponse: {response.decode('utf-8', errors='replace')}")

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
