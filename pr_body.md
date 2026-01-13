## 概要

USB CDC経由で起動画面の読み取り・書き込み機能を実装。Windowsアプリから起動画面をカスタマイズできるようになりました。

## 変更点

### Firmware
- **imgread コマンド**: デバイスからAppWizard形式の起動画面データを読み取り
- **imgwrite コマンド**: デバイスに起動画面データを書き込み
- **転送プロトコル**: 64バイトチャンク × 3ms間隔、CRC-16検証、終端マーカー
- **RAM2領域の活用**: フラッシュ書き込み関数とprefixデータをRAM2に配置（各4KB）
- **PREFIX_SAVE_MAX拡張**: 1KB → 4KB（大きなセクタのprefixデータに対応）
- ドキュメント追加（Startup Image Transfer Protocol）

### Windows App (FULLMONI-WIDE-Terminal)
- **起動画面変更タブ**: タブ名を「起動画面書き込み」→「起動画面変更」に変更
- **タブ順序変更**: 「起動画面変更」タブを「ファームウェア更新」タブの前に移動
- **読み取り機能**: デバイスから現在の起動画面を読み取り
- **BMP保存機能**: 読み取った画像をBMPファイルとして保存
- **書き戻し機能**: 読み取った画像をそのまま再書き込み可能
- **プレビュー表示**: 読み取り/選択した画像のリアルタイムプレビュー
- **AppWizardオフセット修正**: 16バイトオフセット（2ピクセルずれ修正）
- **Bootloaderモード対応**: 起動画面変更タブもBootloaderモード時に無効化
- **起動時タブ**: About タブから起動するように変更
- デバッグログのクリーンアップ
- ドキュメント更新

## 転送仕様

| 項目 | 値 |
|------|-----|
| 画像サイズ | 765×256ピクセル |
| フォーマット | RGB565 (16bit) + 16バイトヘッダ |
| 総データサイズ | 391,696バイト |
| チャンクサイズ | 64バイト |
| 転送間隔 | 3ms（安定性確保） |
| CRC | CRC-16/CCITT-FALSE |
| 終端マーカー | 0xED 0x0F 0xAA 0x55 |

## 影響範囲

- `Firmware/src/startup_image_write.c` - imgread/imgwrite実装
- `Firmware/README.md` - プロトコルドキュメント追加
- `HostApp/.../MainWindow.xaml` - タブ順序・名称変更
- `HostApp/.../Views/StartupImageView.xaml` - UI改善
- `HostApp/.../ViewModels/StartupImageViewModel.cs` - 読み取り/BMP保存機能
- `HostApp/.../Services/StartupImageService.cs` - 転送サービス
- `HostApp/.../Services/AppWizardImageConverter.cs` - オフセット修正
- `HostApp/.../README.md` - ドキュメント更新

## テスト

- [x] imgwrite: BMP→AppWizard変換→書き込み→再起動で表示確認
- [x] imgread: 読み取り→プレビュー表示→BMP保存
- [x] 読み取り画像の書き戻し
- [x] CRC検証（エラー時のリトライなし、エラー表示）
- [x] 3ms間隔での安定転送（2msでは間欠的に失敗）
- [x] Bootloaderモードでタブ無効化確認
- [x] 未接続時にタブ無効化確認

## 関連Issue

- #56 起動画面書き込み機能
