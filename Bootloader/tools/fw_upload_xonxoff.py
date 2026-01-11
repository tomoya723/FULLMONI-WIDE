#!/usr/bin/env python3
"""
FULLMONI-WIDE Firmware Upload Tool (XON/XOFF Version)
UART互換プロトコル: XON/XOFF フロー制御 + 2秒タイムアウト
"""

import serial
import sys
import time
import argparse
import threading

# XON/XOFF
XON = 0x11
XOFF = 0x13

# 送信設定
CHUNK_SIZE = 512      # 512バイトずつ送信
SEND_DELAY_MS = 5     # チャンク間のディレイ (ms)

class FirmwareUploader:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.xoff_received = False
        self.xon_received = False
        self.erase_complete = False
        self.prompt_received = False
        self.stop_receiver = False
        self.received_data = []

    def receiver_thread(self):
        """受信スレッド - XON/XOFFとメッセージを処理"""
        while not self.stop_receiver:
            try:
                if self.ser and self.ser.in_waiting > 0:
                    data = self.ser.read(self.ser.in_waiting)
                    for b in data:
                        if b == XON:
                            self.xoff_received = False
                            self.xon_received = True
                            print(f"[RX] XON (0x{b:02X}) - resume")
                        elif b == XOFF:
                            self.xoff_received = True
                            print(f"[RX] XOFF (0x{b:02X}) - pause")
                        elif b >= 0x20 and b <= 0x7E:
                            # 印字可能文字を蓄積
                            self.received_data.append(chr(b))
                        elif b == 0x0D or b == 0x0A:
                            # 改行でバッファをフラッシュ
                            if self.received_data:
                                line = ''.join(self.received_data)
                                print(f"[RX] {line}")
                                # 「Erase complete」を検出
                                if "Erase complete" in line:
                                    self.erase_complete = True
                                # プロンプト検出
                                if "Commands:" in line or line.strip() == ">":
                                    self.prompt_received = True
                                self.received_data = []
                        elif b == ord('>'):
                            # プロンプト文字
                            self.received_data.append(chr(b))
                            self.prompt_received = True
                        else:
                            # その他の制御文字
                            print(f"[RX] <0x{b:02X}>")
                else:
                    time.sleep(0.001)
            except Exception as e:
                if not self.stop_receiver:
                    print(f"Receiver error: {e}")
                break

    def wait_for_xon(self, timeout=10):
        """XONを待つ"""
        print(f"Waiting for XON (timeout: {timeout}s)...")
        start = time.time()
        while time.time() - start < timeout:
            if self.xon_received:
                print("XON detected!")
                return True
            time.sleep(0.01)
        return False

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
            timeout=0.1,
            xonxoff=False,  # ソフトウェアフロー制御は自前で処理
            rtscts=False
        )

        # 受信スレッド開始
        self.stop_receiver = False
        self.xoff_received = True  # 初期状態：XON待ち（送信停止）
        self.xon_received = False  # XON受信フラグ
        rx_thread = threading.Thread(target=self.receiver_thread, daemon=True)
        rx_thread.start()

        try:
            # 少し待ってから開始（接続安定化）
            time.sleep(0.5)

            # 受信バッファをクリア
            if self.ser.in_waiting > 0:
                garbage = self.ser.read(self.ser.in_waiting)
                print(f"[RX] Cleared {len(garbage)} bytes from buffer")

            # プロンプトを待つ（最大10秒）
            print("Waiting for bootloader prompt...")
            start = time.time()
            while time.time() - start < 10:
                if self.prompt_received:
                    print("Prompt received!")
                    break
                time.sleep(0.1)
            else:
                print("Warning: Prompt not detected, sending 'U' anyway...")

            time.sleep(0.2)

            # 「U」コマンドを送信してUpdate開始
            print("Sending 'U' command to start update...")
            self.ser.write(b'U')

            # 「Erase complete」メッセージを待つ（消去に時間がかかる）
            print("Waiting for erase to complete (timeout: 120s)...")
            start = time.time()
            while time.time() - start < 120:
                if self.erase_complete:
                    print("Erase complete detected!")
                    break
                time.sleep(0.1)
            else:
                print("ERROR: Timeout waiting for erase")
                return False

            # XONを待つ（既に送信されているはず）
            time.sleep(0.5)  # XON送信のための待ち
            if not self.xon_received:
                print("Warning: XON not explicitly received, continuing anyway...")
            else:
                print("XON received!")

            print("XON received - starting transfer...")

            # データ送信
            sent = 0
            last_progress = -1
            start_time = time.time()

            while sent < fw_size:
                # XOFFなら待機
                while self.xoff_received:
                    time.sleep(0.001)

                # チャンク送信
                chunk_size = min(CHUNK_SIZE, fw_size - sent)
                chunk = fw_data[sent:sent + chunk_size]
                self.ser.write(chunk)
                sent += chunk_size

                # 進捗表示
                progress = (sent * 100) // fw_size
                if progress != last_progress:
                    last_progress = progress
                    if progress % 10 == 0:
                        elapsed = time.time() - start_time
                        speed = sent / elapsed / 1024 if elapsed > 0 else 0
                        print(f"  {sent:,}/{fw_size:,} bytes ({progress}%) - {speed:.1f} KB/s")

                # ディレイ
                time.sleep(SEND_DELAY_MS / 1000.0)

            elapsed = time.time() - start_time
            speed = fw_size / elapsed / 1024 if elapsed > 0 else 0
            print(f"\nTransfer complete: {fw_size:,} bytes in {elapsed:.1f}s ({speed:.1f} KB/s)")
            print("Waiting for bootloader to verify (2s timeout)...")

            # ブートローダーの検証・リセットを待つ
            time.sleep(5)

            print("Done!")
            return True

        finally:
            self.stop_receiver = True
            time.sleep(0.1)
            if self.ser:
                self.ser.close()


def main():
    parser = argparse.ArgumentParser(description='FULLMONI-WIDE Firmware Upload (XON/XOFF)')
    parser.add_argument('firmware', help='Firmware file (.mot or .bin)')
    parser.add_argument('-p', '--port', default='COM7', help='Serial port (default: COM7)')
    parser.add_argument('-b', '--baud', type=int, default=115200, help='Baud rate (default: 115200)')

    args = parser.parse_args()

    uploader = FirmwareUploader(args.port, args.baud)
    success = uploader.upload(args.firmware)

    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
