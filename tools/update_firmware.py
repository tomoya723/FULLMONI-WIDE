#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RX72N Bootloader ファームウェア更新ツール

使用方法:
    python update_firmware.py <COMポート> <ファームウェアファイル>

例:
    python update_firmware.py COM3 application.bin
"""

import sys
import time
import struct
import serial
from pathlib import Path

# コマンド定義
CMD_SYNC = 0x01
CMD_GET_VERSION = 0x02
CMD_ERASE_APP = 0x10
CMD_WRITE_DATA = 0x11
CMD_VERIFY_CRC = 0x12
CMD_JUMP_APP = 0x20
CMD_ACK = 0xA0
CMD_NACK = 0xA1

# パケット設定
PACKET_SOH = 0x01
PACKET_MAX_DATA_SIZE = 1024

# Application領域設定
APPLICATION_START = 0xFFC20000
APPLICATION_SIZE = 0x003E0000  # ~3.9MB

class FirmwareUpdater:
    def __init__(self, port, baudrate=115200):
        """
        初期化
        :param port: COMポート (例: COM3, /dev/ttyUSB0)
        :param baudrate: ボーレート (デフォルト: 115200)
        """
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=5.0
        )
        print(f"シリアルポート {port} をオープンしました ({baudrate}bps)")

    def __del__(self):
        """クローズ"""
        if hasattr(self, 'ser') and self.ser.is_open:
            self.ser.close()

    def calc_crc16(self, data):
        """
        CRC-16 (CCITT) 計算
        :param data: データバイト列
        :return: CRC-16値
        """
        crc = 0xFFFF
        for byte in data:
            crc ^= (byte << 8)
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ 0x1021
                else:
                    crc <<= 1
                crc &= 0xFFFF
        return crc

    def send_packet(self, cmd, data=None):
        """
        パケット送信
        :param cmd: コマンド
        :param data: データ (オプション)
        :return: True=成功, False=失敗
        """
        if data is None:
            data = b''

        length = len(data)
        if length > PACKET_MAX_DATA_SIZE:
            print(f"エラー: データサイズが大きすぎます ({length} > {PACKET_MAX_DATA_SIZE})")
            return False

        # ヘッダ作成
        header = struct.pack('>BBH', PACKET_SOH, cmd, length)

        # CRC計算
        crc_data = header + data
        crc = self.calc_crc16(crc_data)
        crc_bytes = struct.pack('>H', crc)

        # 送信
        packet = header + data + crc_bytes
        self.ser.write(packet)

        return True

    def recv_packet(self, timeout=5.0):
        """
        パケット受信
        :param timeout: タイムアウト (秒)
        :return: (cmd, data) or None
        """
        old_timeout = self.ser.timeout
        self.ser.timeout = timeout

        try:
            # ヘッダ受信
            header = self.ser.read(4)
            if len(header) != 4:
                print("エラー: ヘッダ受信タイムアウト")
                return None

            soh, cmd, length = struct.unpack('>BBH', header)

            if soh != PACKET_SOH:
                print(f"エラー: 無効なSOH (0x{soh:02X})")
                return None

            # データ受信
            data = b''
            if length > 0:
                data = self.ser.read(length)
                if len(data) != length:
                    print(f"エラー: データ受信不完全 ({len(data)}/{length})")
                    return None

            # CRC受信
            crc_bytes = self.ser.read(2)
            if len(crc_bytes) != 2:
                print("エラー: CRC受信タイムアウト")
                return None

            crc_recv = struct.unpack('>H', crc_bytes)[0]

            # CRC検証
            crc_calc = self.calc_crc16(header + data)
            if crc_recv != crc_calc:
                print(f"エラー: CRC不一致 (受信:0x{crc_recv:04X}, 計算:0x{crc_calc:04X})")
                return None

            return (cmd, data)

        finally:
            self.ser.timeout = old_timeout

    def wait_ack(self, timeout=5.0):
        """
        ACK応答待機
        :param timeout: タイムアウト (秒)
        :return: True=ACK受信, False=NACK/タイムアウト
        """
        result = self.recv_packet(timeout)
        if result is None:
            return False

        cmd, data = result

        if cmd == CMD_ACK:
            if len(data) > 0:
                status = data[0]
                print(f"  ACK受信 (status=0x{status:02X})")
            return True
        elif cmd == CMD_NACK:
            if len(data) > 0:
                error = data[0]
                print(f"  NACK受信 (error=0x{error:02X})")
            return False
        else:
            print(f"  予期しない応答 (cmd=0x{cmd:02X})")
            return False

    def sync(self):
        """同期確認"""
        print("同期確認...")
        self.send_packet(CMD_SYNC)
        if self.wait_ack():
            print("同期確認OK")
            return True
        else:
            print("同期確認失敗")
            return False

    def get_version(self):
        """バージョン取得"""
        print("バージョン取得...")
        self.send_packet(CMD_GET_VERSION)
        result = self.recv_packet()
        if result:
            cmd, data = result
            if cmd == CMD_ACK and len(data) >= 4:
                major, minor, patch, _ = struct.unpack('>HHHH', data[:8])
                print(f"Bootloader バージョン: v{major}.{minor}.{patch}")
                return True
        print("バージョン取得失敗")
        return False

    def erase_application(self):
        """Application領域消去"""
        print("Application領域消去中...")
        self.send_packet(CMD_ERASE_APP)
        if self.wait_ack(timeout=60.0):  # 消去は時間がかかる
            print("消去完了")
            return True
        else:
            print("消去失敗")
            return False

    def write_firmware(self, firmware_data):
        """
        ファームウェア書き込み
        :param firmware_data: ファームウェアデータ
        :return: True=成功, False=失敗
        """
        print(f"ファームウェア書き込み中 ({len(firmware_data)} バイト)...")

        # 128バイト単位で書き込み (RX72N Flash書き込み単位)
        chunk_size = 128
        total_chunks = (len(firmware_data) + chunk_size - 1) // chunk_size

        for i in range(total_chunks):
            offset = i * chunk_size
            chunk = firmware_data[offset:offset + chunk_size]

            # 128バイトに満たない場合は0xFFで埋める
            if len(chunk) < chunk_size:
                chunk += b'\xFF' * (chunk_size - len(chunk))

            # アドレスとデータを送信
            addr = APPLICATION_START + offset
            write_data = struct.pack('>I', addr) + chunk

            self.send_packet(CMD_WRITE_DATA, write_data)

            if not self.wait_ack(timeout=2.0):
                print(f"  書き込み失敗 (offset=0x{offset:08X})")
                return False

            # 進捗表示
            progress = (i + 1) * 100 // total_chunks
            print(f"\r  進捗: {progress}% ({i + 1}/{total_chunks} chunks)", end='')

        print("\n書き込み完了")
        return True

    def verify_firmware(self, firmware_data):
        """
        ファームウェア検証 (CRC-32)
        :param firmware_data: ファームウェアデータ
        :return: True=一致, False=不一致
        """
        print("ファームウェア検証中...")

        # CRC-32計算 (簡易実装)
        import zlib
        crc32 = zlib.crc32(firmware_data) & 0xFFFFFFFF

        # 検証コマンド送信
        verify_data = struct.pack('>II', len(firmware_data), crc32)
        self.send_packet(CMD_VERIFY_CRC, verify_data)

        if self.wait_ack(timeout=30.0):
            print("検証OK")
            return True
        else:
            print("検証失敗")
            return False

    def jump_to_application(self):
        """Application起動"""
        print("Application起動...")
        self.send_packet(CMD_JUMP_APP)
        # 起動後は応答なし
        print("起動完了")

    def update(self, firmware_path):
        """
        ファームウェア更新実行
        :param firmware_path: ファームウェアファイルパス
        :return: True=成功, False=失敗
        """
        # ファームウェア読み込み
        firmware_path = Path(firmware_path)
        if not firmware_path.exists():
            print(f"エラー: ファイルが見つかりません: {firmware_path}")
            return False

        firmware_data = firmware_path.read_bytes()
        print(f"ファームウェアファイル: {firmware_path} ({len(firmware_data)} バイト)")

        # 更新シーケンス
        if not self.sync():
            return False

        time.sleep(0.1)

        if not self.get_version():
            return False

        time.sleep(0.1)

        if not self.erase_application():
            return False

        time.sleep(0.5)

        if not self.write_firmware(firmware_data):
            return False

        time.sleep(0.5)

        if not self.verify_firmware(firmware_data):
            return False

        time.sleep(0.5)

        self.jump_to_application()

        print("\n===== 更新成功 =====")
        return True


def main():
    """メイン処理"""
    if len(sys.argv) != 3:
        print("使用方法: python update_firmware.py <COMポート> <ファームウェアファイル>")
        print("例: python update_firmware.py COM3 application.bin")
        sys.exit(1)

    port = sys.argv[1]
    firmware_file = sys.argv[2]

    try:
        updater = FirmwareUpdater(port)
        success = updater.update(firmware_file)
        sys.exit(0 if success else 1)

    except serial.SerialException as e:
        print(f"シリアルポートエラー: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n中断されました")
        sys.exit(1)
    except Exception as e:
        print(f"エラー: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == '__main__':
    main()
