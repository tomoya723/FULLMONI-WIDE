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

    # Bootloader application area limit: 0xFFC20000-0xFFE1FFFF = 2MB
    BL_APP_MAX_SIZE = 0x200000  # 2,097,152 bytes
    if file_size > BL_APP_MAX_SIZE:
        print(f"Error: Firmware too large! Max={BL_APP_MAX_SIZE:,} bytes, Got={file_size:,} bytes")
        print(f"  Exceeds limit by {file_size - BL_APP_MAX_SIZE:,} bytes")
        return False

    try:
        # Bootloader (boot_loader.c) implements XON(0x11)/XOFF(0x13) software flow
        # control: it sends XOFF when its ring buffer passes BL_XOFF_THRESHOLD (4KB),
        # and additionally sends XOFF/XON around *every single* flash write (interrupts
        # are disabled during the write, so it can't receive). For a ~2MB image that's
        # thousands of XOFF/XON pairs over the transfer. xonxoff=True makes pyserial/
        # the OS driver pause transmission on XOFF and resume on XON automatically;
        # without it (as before) those bytes were sent but never honored, so large
        # images could silently overrun the bootloader's buffer and flash corrupted
        # data while still reporting a "successful" transfer.
        #
        # write_timeout applies to a whole ser.write() call, and each 16KB chunk below
        # contains ~128 flash-write-sized XOFF/XON pauses -- their cumulative stall time
        # can run several seconds, so give it plenty of headroom (observed: hit the old
        # 5s limit around 95% through a ~1.9MB image).
        ser = serial.Serial(port, baudrate, timeout=2, write_timeout=180, xonxoff=True)
        print(f"Connected to {port} (XON/XOFF flow control enabled)")
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
        # チャンクサイズ: 4KB (16KBだと1回のwrite()にXOFF一時停止が最大128回分
        # 累積しうる。実機で95%前後にWrite timeoutが再現したため縮小)
        chunk_size = 4 * 1024
        sent = 0
        start_time = time.time()
        write_timed_out = False
        MAX_CHUNK_RETRIES = 5

        for i in range(0, len(firmware_data), chunk_size):
            chunk = firmware_data[i:i+chunk_size]
            # write_timeoutに達した場合、pyserialはそのwrite()で書けなかった
            # 残りバイトを送出せず例外を投げる(= このチャンクの一部または全部が
            # 欠落する)。以前は「どうせOS側バッファ経由で送出される」とみなして
            # breakしていたが、実機で本当に末尾~28KBが欠落する事例を確認したため、
            # 同じチャンクを再送してから先に進む(安全側に倒す)。
            for attempt in range(1, MAX_CHUNK_RETRIES + 1):
                t0 = time.time()
                try:
                    ser.write(chunk)
                    break
                except serial.SerialTimeoutException:
                    dt = time.time() - t0
                    # デバイス側が何かエラー文字列等を返していないか確認 (非ブロッキング)
                    pending = b''
                    try:
                        if ser.in_waiting:
                            pending = ser.read(ser.in_waiting)
                    except Exception:
                        pass
                    extra = f" device said: {pending!r}" if pending else ""
                    if attempt == MAX_CHUNK_RETRIES:
                        print(f"\nWrite timeout at {sent / file_size * 100:.1f}% "
                              f"({sent:,}/{file_size:,} bytes) after {MAX_CHUNK_RETRIES} retries "
                              f"on the same chunk ({dt:.1f}s this attempt) - aborting (firmware incomplete on device).{extra}")
                        write_timed_out = True
                    else:
                        print(f"\nWrite timeout at {sent / file_size * 100:.1f}% "
                              f"({dt:.1f}s this attempt) - retrying chunk ({attempt}/{MAX_CHUNK_RETRIES})...{extra}")
            if write_timed_out:
                break
            sent += len(chunk)

            # 進捗表示
            progress = (sent / file_size) * 100
            kb_sent = sent / 1024
            elapsed = time.time() - start_time
            speed = kb_sent / elapsed if elapsed > 0 else 0
            print(f"\rProgress: {progress:5.1f}% ({kb_sent:.1f} KB / {file_size/1024:.1f} KB) - {speed:.1f} KB/s", end='', flush=True)

            # 小さな遅延でUSBバッファを安定化
            time.sleep(0.01)

        if write_timed_out:
            # チャンク再送を使い切ってもなお書けなかった = 実機で欠落を確認済み
            # (以前は「どうせ届く」とみなして続行していたが、転送が末尾~28KB
            # 欠けたまま再起動不能になる事例があった)。ここで止めて明示的に失敗
            # とする。デバイス側はブートローダーの入力待ちで止まっているはずなので、
            # 電源再投入してから再実行すること。
            print("Firmware transfer incomplete - aborting. "
                  "Power-cycle the device and retry (it is likely stuck in the bootloader).")
            ser.close()
            return False

        print("\n\nWaiting for completion message...")

        # 完了メッセージを待つ (最大30秒)
        ser.timeout = 30
        response = b''
        try:
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
        except (serial.SerialException, OSError) as e:
            # Bootloaderは成功後、応答を送ってから自分でソフトリセットしてアプリに
            # ジャンプする。そのリセットでUSB CDCが瞬断/再列挙され、読み取り中の
            # ser.read()がWindows側のClearCommError等で例外を投げることがある
            # (実機確認: この経路でも転送自体は正常完了していたケースがある)。
            # なのでここでは即failにせず、inconclusiveとして completion message
            # なしの警告扱いに倒す。
            print(f"\n(Device likely reset/re-enumerated during completion wait: {e})")

        if response:
            print(f"Response: {response.decode('utf-8', errors='replace')}")

        if b'Done' in response:
            pass  # Success
        elif b'ERR' in response:
            print("Flash write error!")
            ser.close()
            return False
        else:
            print("Warning: No completion message received (device may have reset already - verify visually)")

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
