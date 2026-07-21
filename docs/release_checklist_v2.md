# FULLMONI-WIDE v2.0.0 リリースチェックリスト（LVGL + EEZ Studio 移行）

`dev_LVGL_EEZ` ブランチを対象とした、メジャーリリース **v2.0.0** に向けた残作業一覧。

## 背景

- **内容:** 描画バックエンドを emWin / AppWizard / SquareLine Studio から **LVGL 8.3 + EEZ Studio** へ全面移行（main比 +37 / −3 コミット、約 618 ファイル変更）。
- **バージョン方針:** 描画エンジン刷新に伴う破壊的変更のため **v2.0.0**（メジャー更新）。
- **variant 構成:** `aw001/aw002`（AppWizard）→ **`eez001`(Standard) / `eez002`(Racing) / `eez003`(Prodrive)**。切替は `tools/switch_design.ps1`（Windows Directory Junction 方式）。
- **リリース対象ブランチ:** `dev_LVGL_EEZ` 一本。前身の `dev_LVGL` は成果が完全内包済みのため、リリース後にクローズ/削除可。

---

## A. ブランチ整合（PR の前提）

- [ ] **A-1. main の未取り込み 3 コミットを反映**（rebase または merge）
  - `81a5792` fix: OnlineUpdateService 改善 + マニフェスト SHA256 更新 + debug 環境調整 (#121)
  - `ec5d228` fix: **FPU denormalized float freeze 修正** + robustness improvements (v1.0.2) (#120) ← フリーズ対策のため必須
  - `6345ca4` fix: AppWizard ヘッダサイズ 12→16 バイト (#118) ← AppWizard 撤去済みのため影響確認のみ
- [ ] **A-2. LVGL submodule 対応の確認** — `Firmware/src/lvgl`（`release/v8.3`）。PR / クローン手順 / CI で `submodules: recursive` を考慮。

## B. CI/CD の刷新（現状 AppWizard 前提で陳腐化）

- [ ] **B-1. `.github/workflows/build-check.yml` の書き換え** — 削除済みの `Firmware/aw001` / `Firmware/aw002` の AppWizard Image フォルダをまだチェックしている（main と同一）。eez001/002/003 前提へ全面改修 + submodule チェックアウト追加。
- [ ] **B-2. `.github/workflows/release.yml` の改修** — `FULLMONI-WIDE-*-aw*.bin` のみマッチ、variant 名（aw001/aw002/aw003）と `minimumBootloaderVersion:1.0.0` をハードコード。eez 変種を拾えないため EEZ 対応へ。
- [ ] **B-3. `release-manifest.json` の更新** — 旧 `Firmware_1.0.1_aw001/aw002.bin`・旧バージョンのまま。v2.0.0 + eez 変種へ更新。

## C. バージョニング / リリース資産

- [ ] **C-1. v2.0.0 への横断更新**
  - `Firmware/src/firmware_version.h`（現状 1.0.1）
  - WPF ホストアプリ AssemblyInfo / `MainWindow.xaml`
  - `release-manifest.json`
- [ ] **C-2. `firmware_header_data.c` の post-build 埋め込み検証** — `size = 0` / `crc32 = 0xFFFFFFFF` のプレースホルダ（`TODO: post-buildで更新`）がリリースビルドパイプラインで確実に埋まるか確認。

## D. デザイン切替システムの成熟度

- [ ] **D-1. `tools/switch_design.ps1` の Windows Junction 依存の扱い** — Windows 専用。macOS/Linux 開発者・CI での扱いを判断し、ドキュメント整備。
- [ ] **D-2. eez003 の UI 確定と実機表示確認** — 直近までバーフィル / フォント / データバインディングを調整中。UI 確定・実機検証。
- [ ] **D-3. 各 variant のデータバインディング整合確認**（eez001 / eez002 / eez003）を実機で検証。

## E. クリーンアップ（リリース品質）

- [ ] **E-1. `tools/` の使い捨てスクリプト除去/隔離** — `_tmp_check*.py`, `_rebuild_eez003_v2..v5.py`, `dump_*.py`, `apply_aw002_*.py`, `parse_appwizard.py`, `inspect_fonts.py` 等。
- [ ] **E-2. `docs/` の作業中間物整理** — `_fuel_area_crop.png`, `test_font_N.png`, `aw002_objects.txt` 等。
- [ ] **E-3. コミット済みバイナリの扱い** — `HostApp/.../FULLMONI-WIDE-Android-v1.0.1.apk` 等、リリース資産管理方針を確認。

## F. 既知課題 / ホストアプリ

- [ ] **F-1. `speaker_play_warning` のパターン制御** — 警告種別ごとのパターン（OVERHEAT=無限、LOW_FUEL=2回 等）が emWin 時代から未実装。v2.0.0 に含めるか判断。
- [ ] **F-2. HostApp の変種 ID 検証との整合** — `#105` で追加された variant ID 検証が新 eez 変種 ID と整合するか。Android `StartupImageService.kt` の変更・起動画像サイズ（765×256）整合を確認。

## G. PR 作成

- [ ] **G-1. PR 本文の作成** — `.github/PULL_REQUEST_TEMPLATE.md` 準拠（概要 / 変更点 / 影響範囲 / テスト）。
- [ ] **G-2. レビュー戦略の決定** — 約 618 ファイルの巨大 PR をどう扱うか（分割 vs 一括 + 丁寧な説明）。

---

## 参考: 関連ドキュメント

- `docs/lvgl_migration_design.md` — LVGL 移行設計書
- `docs/eez_design_switching.md` — EEZ デザイン切替システム
- `docs/eez003_prodrive_design.md` — eez003 Prodrive デザイン
- `docs/release_procedure.md` — リリース手順書（要 v2.0.0 / eez 反映更新）
