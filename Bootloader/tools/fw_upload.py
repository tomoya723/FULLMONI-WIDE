#!/usr/bin/env python3
"""
FULLMONI-WIDE Firmware Upload Tool (Size + ACK Protocol)
Protocol: 4-byte size prefix + ACK ('.' after each 128-byte write)
"""

import serial
import sys
import time
import argparse

class FirmwareUploader:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None

    def upload(self, firmware_path):
        """ファームウェアをアップロード"""
        # ファームウェア読み込み
        print(f"Loading firmware: {firmware_path}")
        with open(firmware_path, 'rb') as f:
            fw_data = f.read()

        fw_size = len(fw_data)
        print(f"Firmware size: {fw_size:,} bytes")

        # マジック確認
        if fw_size >= 4:
            magic = int.from_bytes(fw_data[0:4], 'little')
            if magic == 0x52584657:
                print("Firmware header: OK (RXFW)")
            else:
                print(f"Warning: Invalid magic (0x{magic:08X})")

        # シリアルポートを開く
        print(f"Opening {self.port} at {self.baudrate} baud...")
        self.ser = serial.Serial(
            port=self.port,
            baudrate=self.baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1.0
        )

        try:
            # 少し待ってから開始
            time.sleep(1.0)

            # 受信バッファをクリア
            if self.ser.in_waiting > 0:
                garbage = self.ser.read(self.ser.in_waiting)
                print(f"Cleared {len(garbage)} bytes from buffer")

            # 「U」コマンドを送信してUpdate開始
            print("Sending 'U' command...")
            self.ser.write(b'U')

            # 応答を待つ
            print("Waiting for 'Erase OK'...")
            response = b''
            start = time.time()
            while time.time() - start < 120:  # 最大120秒待ち（消去に時間がかかる）
                if self.ser.in_waiting > 0:
                    data = self.ser.read(self.ser.in_waiting)
                    response += data
                    print(data.decode('utf-8', errors='replace'), end='', flush=True)
                    if b'Erase OK' in response:
                        print()  # 改行
                        break
                time.sleep(0.01)
            else:
                print("\nERROR: Timeout waiting for erase")
                return False

            time.sleep(0.1)

            # サイズを送信（4バイト、リトルエンディアン）
            print(f"Sending size: {fw_size} bytes...")
            size_bytes = fw_size.to_bytes(4, 'little')
            self.ser.write(size_bytes)

            # ACK ('.')を待つ
            ack = self.ser.read(1)
            if ack != b'.':
                print(f"ERROR: Expected ACK '.', got {ack}")
                return False
            print("Size ACK received")

            # データ送信
            print("Sending firmware data...")
            sent = 0
            ack_count = 0
            start_time = time.time()

            while sent < fw_size:
                # チャンク送信（64バイトずつ）
                chunk_size = min(64, fw_size - sent)
                chunk = fw_data[sent:sent + chunk_size]
                self.ser.write(chunk)
                sent += chunk_size

                # 128バイトごとにACKを待つ
                if (sent % 128) == 0 or sent >= fw_size:
                    ack = self.ser.read(1)
                    if ack == b'.':
                        ack_count += 1
                        # 進捗表示（10回ごと = 1280バイトごと）
                        if ack_count % 80 == 0:
                            progress = (sent * 100) // fw_size
                            elapsed = time.time() - start_time
                            speed = sent / elapsed / 1024 if elapsed > 0 else 0
                            print(f"  {sent:,}/{fw_size:,} bytes ({progress}%) - {speed:.1f} KB/s")
                    elif ack:
                        print(f"\nWarning: Unexpected response: {ack}")

            elapsed = time.time() - start_time
            speed = fw_size / elapsed / 1024 if elapsed > 0 else 0
            print(f"\nTransfer complete: {fw_size:,} bytes in {elapsed:.1f}s ({speed:.1f} KB/s)")

            # 完了を待つ（ブートローダーがリセットして切断する可能性あり）
            print("Waiting for completion...")
            try:
                time.sleep(0.5)
                if self.ser.in_waiting > 0:
                    response = self.ser.read(self.ser.in_waiting)
                    print(response.decode('utf-8', errors='replace'))
            except Exception:
                # デバイス切断 - 正常なアップデート完了後に発生する
                pass

            print("\n" + "="*50)
            print("Firmware upload successful!")
            print("Bootloader will reset and start new firmware.")
            print("="*50)
            return True

        finally:
            if self.ser:
                self.ser.close()


def main():
    parser = argparse.ArgumentParser(description='FULLMONI-WIDE Firmware Upload (Size+ACK)')
    parser.add_argument('firmware', help='Firmware file (.bin)')
    parser.add_argument('-p', '--port', default='COM7', help='Serial port (default: COM7)')
    parser.add_argument('-b', '--baud', type=int, default=115200, help='Baud rate (default: 115200)')

    args = parser.parse_args()

    uploader = FirmwareUploader(args.port, args.baud)
    success = uploader.upload(args.firmware)

    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
