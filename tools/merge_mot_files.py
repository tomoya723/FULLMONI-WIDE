#!/usr/bin/env python3
"""
Motorola S-Record Merge Tool
=============================

複数のMOTファイル（Motorola S-Record）を1つのファイルに統合します。
FULLMONI-WIDE プロジェクトでは、ブートローダーとアプリケーションを
統合したフラッシュ書き込み用ファイルを生成するために使用します。

使用方法:
    python merge_mot_files.py <出力MOT> <入力MOT1> <入力MOT2> ...

例:
    python merge_mot_files.py combined.mot bootloader.mot application.mot

Motorola S-Record フォーマット:
    S0: ヘッダレコード（オプション）
    S1: 16ビットアドレス データレコード
    S2: 24ビットアドレス データレコード
    S3: 32ビットアドレス データレコード
    S5: レコード数（オプション）
    S7: 32ビットアドレス 終了レコード（S3用）
    S8: 24ビットアドレス 終了レコード（S2用）
    S9: 16ビットアドレス 終了レコード（S1用）
"""

import sys
import os
from pathlib import Path
from typing import List, Tuple, Dict


class SRecord:
    """Motorola S-Recordレコードを表すクラス"""

    def __init__(self, line: str):
        """
        S-Recordレコードをパース

        Args:
            line: S-Recordの1行（改行なし）
        """
        self.raw_line = line.strip()

        if not line.startswith('S'):
            raise ValueError(f"Invalid S-Record: {line}")

        self.record_type = line[1]

        # バイトカウント（アドレス＋データ＋チェックサム）
        self.byte_count = int(line[2:4], 16)

        # レコードタイプに応じてアドレス長を決定
        if self.record_type in ['0', '1', '5', '9']:
            addr_len = 4  # 16ビット
        elif self.record_type in ['2', '8']:
            addr_len = 6  # 24ビット
        elif self.record_type in ['3', '7']:
            addr_len = 8  # 32ビット
        else:
            addr_len = 0

        # アドレス
        if addr_len > 0:
            self.address = int(line[4:4+addr_len], 16)
        else:
            self.address = 0

        # データ
        data_start = 4 + addr_len
        data_end = 4 + self.byte_count * 2  # チェックサムを含む
        self.data = line[data_start:data_end-2]  # チェックサムは除外

        # チェックサム
        self.checksum = int(line[data_end-2:data_end], 16)

    def is_data_record(self) -> bool:
        """データレコードかどうか判定"""
        return self.record_type in ['1', '2', '3']

    def is_termination_record(self) -> bool:
        """終了レコードかどうか判定"""
        return self.record_type in ['7', '8', '9']

    def is_header_record(self) -> bool:
        """ヘッダレコードかどうか判定"""
        return self.record_type == '0'

    def __str__(self) -> str:
        return self.raw_line


def calculate_checksum(record_type: str, address: int, data: bytes) -> int:
    """
    S-Recordのチェックサムを計算

    Args:
        record_type: レコードタイプ ('1', '2', '3')
        address: アドレス
        data: データバイト列

    Returns:
        チェックサム値
    """
    # アドレス長の決定
    if record_type in ['1', '9']:
        addr_bytes = address.to_bytes(2, 'big')
    elif record_type in ['2', '8']:
        addr_bytes = address.to_bytes(3, 'big')
    elif record_type in ['3', '7']:
        addr_bytes = address.to_bytes(4, 'big')
    else:
        addr_bytes = b''

    # バイトカウント = アドレス長 + データ長 + チェックサム(1)
    byte_count = len(addr_bytes) + len(data) + 1

    # チェックサム計算: バイトカウント + アドレス + データの合計の1の補数
    checksum = byte_count
    checksum += sum(addr_bytes)
    checksum += sum(data)
    checksum = (~checksum) & 0xFF

    return checksum


def read_mot_file(file_path: str) -> List[SRecord]:
    """
    MOTファイルを読み込んでS-Recordリストを返す

    Args:
        file_path: MOTファイルパス

    Returns:
        S-Recordのリスト
    """
    records = []

    with open(file_path, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue

            try:
                record = SRecord(line)
                records.append(record)
            except Exception as e:
                print(f"Warning: Failed to parse line {line_num} in {file_path}: {e}", file=sys.stderr)

    return records


def merge_mot_records(record_lists: List[List[SRecord]]) -> List[SRecord]:
    """
    複数のS-Recordリストを統合

    Args:
        record_lists: S-Recordリストのリスト

    Returns:
        統合されたS-Recordリスト
    """
    # アドレス範囲とデータを記録
    data_map: Dict[int, bytes] = {}
    record_type = '3'  # 32ビットアドレスを使用
    entry_address = 0

    for records in record_lists:
        for record in records:
            if record.is_data_record():
                # データレコードを処理
                addr = record.address
                data_bytes = bytes.fromhex(record.data)

                # データをマップに追加（重複アドレスは上書き）
                for i, byte in enumerate(data_bytes):
                    data_map[addr + i] = byte

                # より新しいレコードタイプを採用
                if record.record_type > record_type:
                    record_type = record.record_type

            elif record.is_termination_record():
                # 最後の終了レコードのアドレスを保持
                entry_address = record.address

    # アドレスをソート
    sorted_addresses = sorted(data_map.keys())

    # 連続したデータをまとめてS-Recordを生成
    merged_records = []

    # ヘッダレコード追加
    header_data = b"FULLMONI-WIDE Combined"
    header_checksum = calculate_checksum('0', 0, header_data)
    header_line = f"S00{len(header_data) + 3:02X}0000{header_data.hex().upper()}{header_checksum:02X}"
    merged_records.append(SRecord(header_line))

    # データレコード生成
    MAX_DATA_PER_RECORD = 32  # 1レコードあたりの最大データバイト数

    i = 0
    while i < len(sorted_addresses):
        # 連続するアドレスのデータを収集
        start_addr = sorted_addresses[i]
        data_bytes = bytearray()

        while i < len(sorted_addresses) and len(data_bytes) < MAX_DATA_PER_RECORD:
            expected_addr = start_addr + len(data_bytes)

            if sorted_addresses[i] != expected_addr:
                # 不連続な場合は新しいレコードを開始
                break

            data_bytes.append(data_map[sorted_addresses[i]])
            i += 1

        # S-Recordを生成
        checksum = calculate_checksum(record_type, start_addr, bytes(data_bytes))

        if record_type == '1':
            addr_str = f"{start_addr:04X}"
        elif record_type == '2':
            addr_str = f"{start_addr:06X}"
        else:  # '3'
            addr_str = f"{start_addr:08X}"

        byte_count = len(addr_str) // 2 + len(data_bytes) + 1
        record_line = f"S{record_type}{byte_count:02X}{addr_str}{data_bytes.hex().upper()}{checksum:02X}"
        merged_records.append(SRecord(record_line))

    # 終了レコード追加
    term_type = {'1': '9', '2': '8', '3': '7'}[record_type]

    if term_type == '9':
        term_addr_str = f"{entry_address:04X}"
    elif term_type == '8':
        term_addr_str = f"{entry_address:06X}"
    else:  # '7'
        term_addr_str = f"{entry_address:08X}"

    term_byte_count = len(term_addr_str) // 2 + 1
    term_checksum = calculate_checksum(term_type, entry_address, b'')
    term_line = f"S{term_type}{term_byte_count:02X}{term_addr_str}{term_checksum:02X}"
    merged_records.append(SRecord(term_line))

    return merged_records


def write_mot_file(file_path: str, records: List[SRecord]):
    """
    S-RecordリストをMOTファイルに書き込み

    Args:
        file_path: 出力ファイルパス
        records: S-Recordリスト
    """
    with open(file_path, 'w') as f:
        for record in records:
            f.write(str(record) + '\n')


def print_mot_info(records: List[SRecord], label: str):
    """MOTファイルの情報を表示"""
    data_records = [r for r in records if r.is_data_record()]

    if not data_records:
        print(f"{label}: データレコードなし")
        return

    min_addr = min(r.address for r in data_records)
    max_addr = max(r.address + len(r.data) // 2 for r in data_records)
    total_bytes = sum(len(r.data) // 2 for r in data_records)

    print(f"{label}:")
    print(f"  データレコード数: {len(data_records)}")
    print(f"  アドレス範囲: 0x{min_addr:08X} - 0x{max_addr:08X}")
    print(f"  合計データサイズ: {total_bytes} bytes (0x{total_bytes:X})")


def main():
    """メイン処理"""
    print("=" * 70)
    print("Motorola S-Record Merge Tool")
    print("=" * 70)

    # コマンドライン引数チェック
    if len(sys.argv) < 3:
        print("Usage: python merge_mot_files.py <output.mot> <input1.mot> [input2.mot ...]")
        print("\nExample:")
        print("  python merge_mot_files.py combined.mot bootloader.mot application.mot")
        return 1

    output_file = sys.argv[1]
    input_files = sys.argv[2:]

    print(f"\n[入力ファイル]")
    for i, input_file in enumerate(input_files, 1):
        if not os.path.exists(input_file):
            print(f"Error: Input file not found: {input_file}", file=sys.stderr)
            return 1
        print(f"  {i}. {input_file}")

    print(f"\n[出力ファイル]")
    print(f"  {output_file}")

    # 各MOTファイルを読み込み
    print("\n" + "=" * 70)
    print("MOTファイル読み込み...")
    print("=" * 70)

    record_lists = []
    for input_file in input_files:
        print(f"\n読み込み中: {input_file}")
        records = read_mot_file(input_file)
        print_mot_info(records, f"  {os.path.basename(input_file)}")
        record_lists.append(records)

    # MOTファイルを統合
    print("\n" + "=" * 70)
    print("MOTファイル統合中...")
    print("=" * 70)

    merged_records = merge_mot_records(record_lists)
    print_mot_info(merged_records, "\n統合結果")

    # 出力ファイルに書き込み
    print("\n" + "=" * 70)
    print(f"統合MOTファイル書き込み: {output_file}")
    print("=" * 70)

    write_mot_file(output_file, merged_records)

    print(f"\n✓ 統合MOTファイル生成完了: {output_file}")
    print(f"  ファイルサイズ: {os.path.getsize(output_file)} bytes")

    return 0


if __name__ == '__main__':
    sys.exit(main())
