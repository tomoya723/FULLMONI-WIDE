# emWin画像リソースの物理アドレスについて

## 概要

このドキュメントは、PR #62で追加されたもので、emWin画像リソースのメモリアドレス配置に関する技術的な質問に答えるために作成されました。コンパイル後のemWin画像リソースの物理アドレスが固定か変動するかという疑問に対し、詳細な技術解説と現在の実装の適切性を説明しています。

**結論**: アドレスはコンパイルごとに変動します。現在の実装は実行時アドレス解決を使用しており、これが最も適切なアプローチです。

---

## 質問
emWinの画像リソースをコンパイル後実装される物理アドレスは、コンパイルごとに変動する可能性はあるか？もしくは固定アドレスか？

## 結論

**コンパイルごとに変動する可能性があります（固定アドレスではありません）**

## 理由

### 1. リンカによる自動配置

emWin AppWizardで生成される画像リソースは以下のように宣言されます：

```c
// Firmware/aw001/Resource/Image/mtc.c
GUI_CONST_STORAGE unsigned char acmtc[391696UL + 1] = {
    // ... 画像データ ...
};
```

`GUI_CONST_STORAGE`は`const`として定義されており（`GUI_ConfDefaults.h`）、このデータは`.rodata`セクションに配置されます。

### 2. リンカスクリプトの設定

現在のリンカスクリプト（`Firmware/src/linker_script.ld`）では、`.rodata`セクションは以下のように配置されています：

```ld
.rodata :
{
    *(.rodata)
    *(.rodata.*)
    *(C_1)
    *(C_2)
    *(C)
    _erodata = .;
} >ROM
```

この設定では：
- リンカが`.rodata`セクション内のシンボルを**自動的に配置**します
- 配置順序は以下の要因で変わる可能性があります：
  - オブジェクトファイルのリンク順序
  - コンパイルされるファイルの追加・削除
  - 他の`.rodata`シンボルのサイズ変更
  - 最適化オプションの変更

### 3. 実際の配置例

ビルドごとに`acmtc`のアドレスは変動する可能性があります：

**ビルド1:**
```
acmtc: 0xFFE80000
```

**ビルド2（コードサイズが変化した場合）:**
```
acmtc: 0xFFE90000  ← 変わる可能性あり
```

## 現在の実装の対処方法

### 実行時アドレス取得

`startup_image_write.c`では、コンパイル時に固定アドレスを仮定せず、**実行時にシンボルのアドレスを取得**しています：

```c
// Firmware/src/startup_image_write.c (576行目)
void startup_image_write_mode(void)
{
    uint32_t acmtc_addr;
    
    // 実行時にacmtcのアドレスを取得
    acmtc_addr = (uint32_t)acmtc;
    param_console_printf("Target address: 0x%08lX\r\n", acmtc_addr);
    
    // このアドレスを使ってフラッシュ書き込みを行う
    // ...
}
```

この方法により、以下のメリットがあります：

1. **コンパイルごとのアドレス変動に対応**
2. **保守性が高い**（リンカスクリプト変更に強い）
3. **リビルド時に手動修正不要**

### 情報表示コマンド

`imginfo`コマンドで現在のアドレスを確認できます：

```c
// Firmware/src/startup_image_write.c (369行目)
void startup_image_show_info(void)
{
    uint32_t addr = (uint32_t)acmtc;
    
    param_console_printf("=== Startup Image Info ===\r\n");
    param_console_printf("Symbol: acmtc\r\n");
    param_console_printf("Address: 0x%08lX\r\n", addr);  // 実行時アドレス表示
    param_console_printf("Max Size: %u bytes\r\n", STARTUP_IMAGE_MAX_SIZE);
}
```

## アドレスを固定する方法（推奨しません）

もしアドレスを固定したい場合は、以下の方法がありますが、**保守性の観点から推奨しません**：

### 方法1: リンカスクリプトでセクション固定

```ld
/* リンカスクリプトに追加 */
.startup_image 0xFFE00000 : AT(0xFFE00000)
{
    KEEP(*(.startup_image))
} >ROM
```

```c
/* mtc.c に追加 */
__attribute__((section(".startup_image")))
GUI_CONST_STORAGE unsigned char acmtc[391696UL + 1] = {
    // ...
};
```

**デメリット:**
- アドレス管理が複雑になる
- 他のコードサイズ変更の影響で競合する可能性
- メモリレイアウトの柔軟性が失われる

### 方法2: プラグマでアドレス指定（コンパイラ依存）

```c
#pragma address acmtc=0xFFE00000
GUI_CONST_STORAGE unsigned char acmtc[391696UL + 1] = {
    // ...
};
```

**デメリット:**
- コンパイラ依存の構文
- GCC-RXでは非推奨
- リンク時に衝突エラーが発生する可能性

## 推奨事項

### ✅ 推奨: 現在の実装を維持

現在の実装（実行時アドレス取得）は以下の理由で**最も適切**です：

1. **移植性が高い** - 標準的なC言語の方法
2. **保守しやすい** - リンカスクリプト変更に自動対応
3. **安全性が高い** - コンパイル時にアドレス衝突の心配がない
4. **柔軟性がある** - 将来の拡張に対応しやすい

### ❌ 非推奨: 固定アドレス配置

固定アドレスは以下の場合のみ検討してください：

- 外部ツールが特定アドレスを要求する場合
- ハードウェア制約で固定が必要な場合
- 複数バイナリの厳密な配置制御が必要な場合

通常のファームウェア開発では**不要**です。

## 起動画面書き込み機能の動作

起動画面書き込み機能は以下のように動作します：

1. `imgwrite`コマンド受信
2. **実行時に`acmtc`のアドレスを取得** (`(uint32_t)acmtc`)
3. 32KB境界にアラインしたアドレスを計算
4. 必要なフラッシュブロックを消去
5. プレフィックスデータを保存・復元
6. 新しい画像データを書き込み
7. 自動再起動

このプロセス全体が**実行時アドレス解決**に基づいているため、コンパイルごとのアドレス変動に完全対応しています。

## まとめ

| 項目 | 内容 |
|------|------|
| **アドレスは固定か？** | いいえ、コンパイルごとに変動する可能性があります |
| **現在の対処方法** | 実行時にシンボルアドレスを取得（`(uint32_t)acmtc`） |
| **推奨アプローチ** | 現在の実装を維持（実行時取得が最適） |
| **固定アドレス化** | 技術的には可能だが、非推奨 |

## 参考コード

- **リンカスクリプト**: `Firmware/src/linker_script.ld` (`.rodata`セクション定義)
- **画像リソース定義**: `Firmware/aw001/Resource/Image/mtc.c` (`acmtc`配列宣言)
- **実行時アドレス取得**: `Firmware/src/startup_image_write.c` (`startup_image_write_mode()`関数内)
- **アドレス情報表示**: `Firmware/src/startup_image_write.c` (`startup_image_show_info()`関数)

## 関連ドキュメント

- [STARTUP_IMAGE_WRITE.md](./STARTUP_IMAGE_WRITE.md) - 起動画面書き込み機能の実装ガイド
- [Firmware README](../README.md) - ファームウェア全般の説明
