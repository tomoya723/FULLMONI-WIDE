# 起動画面書き込み機能 - ファームウェア実装ガイド

## 概要
Windows Terminal アプリケーションから USB CDC 経由で起動画面（BMP 画像）をフラッシュメモリに書き込む機能のファームウェア側実装ガイド。

## 必要な実装

### 1. Parameter Console に `imgwrite` コマンドを追加

**ファイル**: `Firmware/src/param_console.c` (または該当ファイル)

```c
// Parameter Console のコマンドハンドラに追加
if (strcmp(cmd, "imgwrite") == 0) {
    // 起動画面書き込みモードに入る
    startup_image_write_mode();
    return;
}
```

### 2. 起動画面書き込みプロトコルの実装

#### プロトコル仕様

```
1. ホスト → デバイス: "imgwrite\r"
2. デバイス → ホスト: (Ready 状態に遷移)
3. ホスト → デバイス: サイズ (4 byte, little-endian, uint32_t)
4. デバイス → ホスト: ACK ('.' = 0x2E)
5. ホスト → デバイス: データチャンク (256 bytes)
6. デバイス → ホスト: ACK ('.')
7. ステップ 5-6 を全データ送信まで繰り返し
8. デバイス → ホスト: "Done!" または "OK" (完了メッセージ)
```

#### サンプル実装

```c
#include <stdint.h>
#include <string.h>
#include "flash_api.h"  // フラッシュ API のヘッダ

// ACK 文字
#define ACK_CHAR '.'

// チャンクサイズ
#define CHUNK_SIZE 256

// mtc リソースのフラッシュアドレス
// 注: リンカマップから実際のアドレスを確認して設定する
#define MTC_FLASH_ADDRESS 0xFFE00000  // 仮のアドレス、要確認

void startup_image_write_mode(void)
{
    uint32_t total_size;
    uint32_t received_bytes = 0;
    uint8_t chunk_buffer[CHUNK_SIZE];
    uint32_t flash_offset = 0;
    
    // 1. サイズ受信 (4 byte, little-endian)
    if (!receive_bytes((uint8_t*)&total_size, 4, 5000)) {
        send_string("ERR: Timeout receiving size\r\n");
        return;
    }
    
    // サイズ検証（約 580KB 程度を想定）
    if (total_size < 1000 || total_size > 1000000) {
        send_string("ERR: Invalid size\r\n");
        return;
    }
    
    // ACK 送信
    send_char(ACK_CHAR);
    
    // 2. フラッシュ消去
    // mtc リソースの領域を消去（必要なセクタ数を計算）
    uint32_t sectors_needed = (total_size + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;
    for (uint32_t i = 0; i < sectors_needed; i++) {
        uint32_t sector_addr = MTC_FLASH_ADDRESS + (i * FLASH_SECTOR_SIZE);
        if (!flash_erase_sector(sector_addr)) {
            send_string("ERR: Flash erase failed\r\n");
            return;
        }
    }
    
    // 3. データ受信とフラッシュ書き込み
    while (received_bytes < total_size) {
        uint32_t remaining = total_size - received_bytes;
        uint32_t chunk_size = (remaining < CHUNK_SIZE) ? remaining : CHUNK_SIZE;
        
        // チャンク受信
        if (!receive_bytes(chunk_buffer, chunk_size, 5000)) {
            send_string("ERR: Timeout receiving data\r\n");
            return;
        }
        
        // フラッシュ書き込み
        uint32_t write_addr = MTC_FLASH_ADDRESS + flash_offset;
        if (!flash_write(write_addr, chunk_buffer, chunk_size)) {
            send_string("ERR: Flash write failed\r\n");
            return;
        }
        
        received_bytes += chunk_size;
        flash_offset += chunk_size;
        
        // ACK 送信
        send_char(ACK_CHAR);
    }
    
    // 4. 完了メッセージ送信
    send_string("Done!\r\n");
}

// ヘルパー関数（UART受信）
bool receive_bytes(uint8_t* buffer, uint32_t size, uint32_t timeout_ms)
{
    uint32_t received = 0;
    uint32_t start_time = get_tick_ms();
    
    while (received < size) {
        if (uart_available()) {
            buffer[received++] = uart_read_byte();
        }
        
        // タイムアウトチェック
        if ((get_tick_ms() - start_time) > timeout_ms) {
            return false;
        }
    }
    
    return true;
}
```

### 3. フラッシュアドレスの特定

#### 方法1: リンカマップファイルから確認

```bash
# ビルド後のマップファイルを検索
grep -A5 "acmtc" output.map
```

出力例:
```
.rodata.acmtc
                0xffe80000       0x5f990 ./Resource/Image/mtc.o
                0xffe80000                acmtc
```

→ この例では `0xFFE80000` が mtc リソースのアドレス

#### 方法2: ソースコードから確認

```c
// Resource.c または mtc.c
extern GUI_CONST_STORAGE unsigned char acmtc[];

// デバッグ用に実行時にアドレスを出力
printf("mtc address: 0x%08X\r\n", (uint32_t)acmtc);
```

#### 方法3: コンパイラプラグマで配置指定

```c
// mtc.c に追加（コンパイラに依存）
#pragma address acmtc=0xFFE80000
GUI_CONST_STORAGE unsigned char acmtc[391696UL + 1] = {
    // ... データ ...
};
```

### 4. フラッシュ書き込み API

RX72N の場合、Renesas Flash API を使用:

```c
#include "r_flash_rx_if.h"

bool flash_erase_sector(uint32_t address)
{
    flash_err_t ret;
    
    ret = R_FLASH_Erase((flash_block_address_t)address, 1);
    return (ret == FLASH_SUCCESS);
}

bool flash_write(uint32_t address, const uint8_t* data, uint32_t size)
{
    flash_err_t ret;
    
    // RX72N はプログラミングサイズ要件あり（通常 128 または 256 バイト）
    // サイズ調整が必要な場合はパディング
    ret = R_FLASH_Write((uint32_t)data, (uint32_t)address, size);
    return (ret == FLASH_SUCCESS);
}
```

### 5. 注意事項

#### フラッシュメモリ仕様
- **RX72N のコードフラッシュ**: 4MB (0xFF000000 - 0xFFFFFFFF)
- **セクタサイズ**: 通常 32KB または 64KB (デバイス依存)
- **プログラミング単位**: 128 バイトまたは 256 バイト
- **書き込み前に消去が必要**

#### タイミング
- フラッシュ消去: 約 1-2 秒/セクタ
- フラッシュ書き込み: 約 10-20ms/256 バイト
- 全体で約 10-15 秒程度

#### セキュリティ
- フラッシュ書き込み前に既存データのバックアップを推奨
- 書き込み失敗時のリカバリ処理を実装
- 不正なサイズ・データの検証

### 6. デバッグ方法

#### テスト用コマンド追加

```c
// Parameter Console にテストコマンドを追加
if (strcmp(cmd, "mtcinfo") == 0) {
    printf("mtc address: 0x%08X\r\n", (uint32_t)acmtc);
    printf("mtc size: %u bytes\r\n", sizeof(acmtc));
    
    // 最初の 16 バイトを表示
    printf("First 16 bytes:");
    for (int i = 0; i < 16; i++) {
        printf(" %02X", acmtc[i]);
    }
    printf("\r\n");
    return;
}
```

#### ログ出力

```c
// デバッグ用のログ出力を追加
#define DEBUG_LOG(fmt, ...) printf("[IMGWRITE] " fmt "\r\n", ##__VA_ARGS__)

void startup_image_write_mode(void)
{
    DEBUG_LOG("Entered image write mode");
    
    // ... 実装 ...
    
    DEBUG_LOG("Received size: %u bytes", total_size);
    DEBUG_LOG("Writing to address: 0x%08X", MTC_FLASH_ADDRESS);
    DEBUG_LOG("Erasing %u sectors", sectors_needed);
    
    // ... 実装 ...
    
    DEBUG_LOG("Write completed successfully");
}
```

## テスト手順

### 1. 単体テスト

```c
// テスト用の小さなデータで動作確認
uint8_t test_data[256] = { /* テストパターン */ };
bool result = flash_write(MTC_FLASH_ADDRESS, test_data, 256);
printf("Test write: %s\r\n", result ? "OK" : "FAIL");
```

### 2. Windows アプリとの統合テスト

1. ファームウェアをビルド・書き込み
2. Windows Terminal アプリを起動
3. デバイスに接続
4. 「起動画面書き込み」タブを開く
5. テスト用 BMP 画像（765×256）を選択
6. 「書き込み開始」をクリック
7. ログを確認しながら進捗を監視
8. 完了後、デバイスを再起動して新しい起動画面を確認

### 3. エラーケースのテスト

- サイズ不正（大きすぎる/小さすぎる）
- 通信タイムアウト
- フラッシュ消去失敗
- フラッシュ書き込み失敗

## まとめ

この実装により、ユーザーは以下が可能になります:
- コンパイル環境なしで起動画面をカスタマイズ
- Windows アプリから簡単に書き込み
- 進捗をリアルタイムで確認

ファームウェア側の実装は比較的シンプルで、既存のフラッシュ API を利用できます。
