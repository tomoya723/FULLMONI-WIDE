#!/usr/bin/env python3
"""
WAVファイルをC言語の配列に変換するスクリプト
使用法: python wav_to_c_array.py input.wav [output.h]

Windows 95の起動音を使う場合:
  C:\Windows\Media\tada.wav を使用
"""

import sys
import wave
import struct
import os

def convert_wav_to_c_array(input_file, output_file=None, target_rate=8000, array_name="s_pcm_data"):
    """WAVファイルを8kHz/8bitのC配列に変換"""

    if output_file is None:
        output_file = os.path.splitext(input_file)[0] + ".h"

    # WAVファイルを開く
    with wave.open(input_file, 'rb') as wav:
        n_channels = wav.getnchannels()
        sample_width = wav.getsampwidth()
        frame_rate = wav.getframerate()
        n_frames = wav.getnframes()

        print(f"入力: {input_file}")
        print(f"  チャンネル数: {n_channels}")
        print(f"  サンプル幅: {sample_width} bytes")
        print(f"  サンプルレート: {frame_rate} Hz")
        print(f"  フレーム数: {n_frames}")
        print(f"  長さ: {n_frames / frame_rate:.2f} 秒")

        # 全フレームを読み込み
        raw_data = wav.readframes(n_frames)

    # サンプルをリストに変換
    samples = []
    if sample_width == 1:  # 8bit unsigned
        for i in range(0, len(raw_data), n_channels):
            # モノラルに変換（左チャンネルのみ使用）
            samples.append(raw_data[i])
    elif sample_width == 2:  # 16bit signed
        for i in range(0, len(raw_data), 2 * n_channels):
            # 16bit signed → 8bit unsigned
            value = struct.unpack('<h', raw_data[i:i+2])[0]
            # -32768~32767 → 0~255
            samples.append(int((value + 32768) / 256))
    else:
        print(f"エラー: サポートされていないサンプル幅: {sample_width}")
        return

    # リサンプリング（簡易的な線形補間）
    if frame_rate != target_rate:
        ratio = frame_rate / target_rate
        new_samples = []
        for i in range(int(len(samples) / ratio)):
            idx = i * ratio
            idx_int = int(idx)
            frac = idx - idx_int
            if idx_int + 1 < len(samples):
                value = samples[idx_int] * (1 - frac) + samples[idx_int + 1] * frac
            else:
                value = samples[idx_int]
            new_samples.append(int(value))
        samples = new_samples
        print(f"  リサンプリング: {frame_rate} Hz → {target_rate} Hz")

    print(f"出力: {output_file}")
    print(f"  サンプル数: {len(samples)}")
    print(f"  サイズ: {len(samples)} bytes ({len(samples)/1024:.1f} KB)")
    print(f"  長さ: {len(samples) / target_rate:.2f} 秒")

    # C配列として出力
    with open(output_file, 'w') as f:
        f.write(f"/* Generated from {os.path.basename(input_file)} */\n")
        f.write(f"/* Sample rate: {target_rate} Hz, 8-bit unsigned */\n")
        f.write(f"/* Length: {len(samples)} samples ({len(samples) / target_rate:.2f} sec) */\n\n")
        f.write(f"#define PCM_SAMPLE_RATE {target_rate}\n")
        f.write(f"#define PCM_DATA_SIZE {len(samples)}\n\n")
        f.write(f"static const uint8_t {array_name}[PCM_DATA_SIZE] = {{\n")

        # 16個ずつ出力
        for i in range(0, len(samples), 16):
            chunk = samples[i:i+16]
            line = "    " + ", ".join(f"0x{s:02X}" for s in chunk)
            if i + 16 < len(samples):
                line += ","
            f.write(line + "\n")

        f.write("};\n")

    print("変換完了!")
    return len(samples)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("使用法: python wav_to_c_array.py input.wav [output.h] [sample_rate]")
        print("")
        print("例: python wav_to_c_array.py C:\\Windows\\Media\\tada.wav startup_sound.h 16000")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    target_rate = int(sys.argv[3]) if len(sys.argv) > 3 else 8000

    convert_wav_to_c_array(input_file, output_file, target_rate=target_rate)
