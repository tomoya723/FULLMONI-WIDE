# FULLMONI-WIDE v2.0.0 開発再開プラン（LVGL + EEZ Studio 移行を軸に）

作成日: 2026-08-11

`dev_LVGL_EEZ` ブランチを唯一の統合ベースとして開発を再開するための、ブランチ整理・リリース安定化・Issue消化の総合プラン。

---

## 0. 現状サマリ（2026-08-11 時点）

- `main` は `81a5792`（2026-03-22, v1.0.2）で停止。以降の実開発は `dev_LVGL_EEZ` 系および `claude/*` 枝に移行。
- **LVGL移行の実体**: 描画バックエンドを emWin / AppWizard / SquareLine Studio から **LVGL 8.3 + EEZ Studio** へ全面移行（main比 +37 / −3、約 618 ファイル）。破壊的変更のため **v2.0.0**（メジャー更新）。
- **variant 構成**: `aw001/aw002`（AppWizard）→ **`eez001`(Standard) / `eez002`(Racing) / `eez003`(Prodrive)**。切替は `tools/switch_design.ps1`（Windows Directory Junction 方式）。
- 既存の詳細チェックリストが `claude/lvgl-eez-pr-release-8ifkfd` 上に存在（本プランのベース）。

### 枝の親子関係
- `dev_LVGL` ⊂ `dev_LVGL_EEZ` ⊂ `claude/lvgl-eez-pr-release-8ifkfd`（差分はリリースチェックリスト doc 1コミットのみ）
- `feature/eez-flow-integration`（EEZ Flow 統合, +24）は別系統・未統合 → **廃案・削除予定**
- `dev_EPSdrvPLSgen`（#115, +1）は **chaketek 独自開発**。当面 main 統合予定なし → 別枝で維持

---

## Phase 0. ブランチ整理（錯綜の解消）

`dev_LVGL_EEZ` を唯一の統合ベースに一本化する。

| ブランチ | main比 (ahead/behind) | 処置 |
|---|---|---|
| **dev_LVGL_EEZ** | +37 / −3 | ★ **新統合ベース** |
| claude/lvgl-eez-pr-release-8ifkfd | +38 / −3 | 差分はチェックリスト doc のみ → ベースへ取込みクローズ |
| dev_LVGL | +11 / −3 | dev_LVGL_EEZ に完全内包 → **削除** |
| feature/eez-flow-integration | +24 / −3 | **廃案・削除予定**（v2.0.0 に含めない） |
| dev_EPSdrvPLSgen | +1 / −3 | #115。**chaketek 独自開発**。当面 main 統合予定なし → 別枝で維持（整理・削除の対象外） |
| dev_dbcupdate (PR #110) | +2 / −7 | GroundSpeed LSB=0.1 修正だけ拾ってベースへ、PR クローズ |
| chore/remove-desktop-tool | merged 済 | **削除** |
| test-aw003 | +110 / −148 | 旧 aw 系・1年放置 → 削除 / アーカイブ |
| dev_dataregister_refactoring | +77 / −148 | 旧・pre-LVGL → 削除（#24 は新ベースで再実施） |
| devenv_docupdate | +2 / −88 | 旧 → 内容確認後削除 |
| feature/android-ui-refresh | +2 / −67 | 旧 → 内容確認後削除 |

> リモート枝の削除は不可逆のため、実行前に個別確認する。

### Phase 0 想定結果

現状 **リモート 14 ブランチ** → 整理後は **実質 3〜4 本** に集約される見込み。

```
14本 → 残す4本（main / dev_LVGL_EEZ / dev_EPSdrvPLSgen / 作業枝）
      + 取込後クローズ2本（claude/lvgl-eez-pr-release / dev_dbcupdate）
      + 削除7本
      + このプラン枝1本（claude/repository-dev-status-lqyz0j：処遇要判断）
```

**残す（3〜4本）**

| ブランチ | 役割 |
|---|---|
| main | リリース済み安定版（v2.0.0 マージ先） |
| dev_LVGL_EEZ | ★ 唯一の統合ベース（v2.0.0 開発本流） |
| dev_EPSdrvPLSgen | chaketek 独自ライン（別管理・対象外） |
| （作業用の一時枝） | 実装時に dev_LVGL_EEZ から切る feature 枝（随時） |

**ベースへ取り込んでからクローズ（2本）**

| ブランチ | 処置 |
|---|---|
| claude/lvgl-eez-pr-release-8ifkfd | チェックリスト doc を dev_LVGL_EEZ へ取込み → 削除 |
| dev_dbcupdate (PR #110) | GroundSpeed LSB=0.1 修正だけ拾う → PR クローズ・削除 |

**削除（7本）**

| ブランチ | 削除理由 |
|---|---|
| dev_LVGL | dev_LVGL_EEZ に完全内包 |
| feature/eez-flow-integration | 廃案 |
| chore/remove-desktop-tool | main にマージ済み |
| test-aw003 | 旧 aw 系・1年放置（+110 / −148） |
| dev_dataregister_refactoring | 旧 pre-LVGL（#24 は新ベースで再実施） |
| devenv_docupdate | 旧・大幅遅延（−88）※内容確認後 |
| feature/android-ui-refresh | 旧・大幅遅延（−67）※内容確認後 |

**Phase 0 実行前の要判断ポイント（3点）**

1. `devenv_docupdate` / `feature/android-ui-refresh` に拾うべき差分（+2）が無いかの内容確認 → 無ければ削除。
2. `dev_dbcupdate` の GroundSpeed LSB=0.1 修正を取り込む前提でよいか（実バグ修正のため推奨）。
3. 本プラン文書 `docs/v2_0_0_development_plan.md` を dev_LVGL_EEZ 側へ移すか（新ベースで参照しやすくなる）。

---

## Phase 1. v2.0.0 を出せる状態にする（リリースブロッカー優先）

技術ブロッカーを先頭に並べ替えた実行順。

1. 🔴 **FPUフリーズ修正（#120）のフォワードポート** — v1.0.2 の目玉修正（走行→停車 約25秒でメインループがフリーズ、FPSW DN=1 / Flush to Zero 設定）が **dev_LVGL_EEZ に未反映**。最優先で移植。#118（AppWizard ヘッダ 12→16バイト）は AppWizard 撤去済みのため影響確認のみ。
2. 🔴 **CI/CD 刷新** — `.github/workflows/build-check.yml` / `release.yml` が削除済みの `aw001/aw002` を前提のまま陳腐化。eez 変種＋LVGL submodule（`Firmware/src/lvgl` release/v8.3、`submodules: recursive`）対応へ書換えないと自動ビルド/リリースが通らない。
3. 🟠 **バージョン横断更新** — `Firmware/src/firmware_version.h`（現 1.0.1）/ WPF AssemblyInfo・MainWindow.xaml / Android / `release-manifest.json` を v2.0.0＋eez 変種へ。`firmware_header_data.c` の post-build 埋め込み（size / crc32 プレースホルダ）検証。
4. 🟠 **eez003 UI 確定＋実機表示検証** — バーフィル / フォント / データバインディングを実機確認。eez001 / eez002 / eez003 のデータバインディング整合確認。
5. 🟡 **クリーンアップ** — `tools/` の使い捨てスクリプト（`_tmp_check*.py`, `_rebuild_eez003_v*.py`, `dump_*.py`, `apply_aw002_*.py`, `parse_appwizard.py`, `inspect_fonts.py` 等）と `docs/` の中間物（`_fuel_area_crop.png`, `test_font_N.png`, `aw002_objects.txt` 等）を除去。コミット済みバイナリ（Android APK 等）の管理方針確認。
6. 🟢 **巨大PR戦略** — 約 618 ファイルを分割 vs 一括＋丁寧な説明で提示するか決定。`.github/PULL_REQUEST_TEMPLATE.md` 準拠で PR 本文作成。

---

## Phase 2. Issue 消化プラン（オープン 25 件を移行に紐づけて分類）

### A. v2.0.0 で解決済み・検証してクローズ
- **#122** Bootloader FW書き込み領域不足 → `6a0e20f`（Bootloader 領域拡大＋リンカ修正）で対応済み
- **#38** Windows フォントを使わない → DenkiChip ネイティブフォント採用で解消見込み
- **#24** dataregister リファクタ → LVGL 移行で描画層再構成済み（残スコープ確認）

### B. 表示系 → 必ず新 LVGL/EEZ ベースで直す（旧 aw 基盤で直さない）
- **#37** 回転計ピーク針が太い／針と同色で見えづらい（EEZ デザイン調整）
- **#39** 燃料インジケータの震え（ローパス見直し）

### C. 警告しきい値クラスタ（共通根治）
明滅系はヒステリシス不足が共通原因。しきい値のHOSTカスタム化として一括対応が効率的。
- **#124** ブレーキ警告灯が明滅
- **#123** 排気温警告灯の異常値表示が明滅
- **#77** ロードスター警告灯判定しきい値カスタム

### D. カスタム設定クラスタ（FW param + HOST UI の同型作業、まとめて）
- **#83** バックライト輝度カスタム
- **#84** FPS 表示有無カスタム
- **#75** ローパス定数 HOST カスタム
- **#51** シフトインジケーター点灯パターンカスタム
- **#70** HOST パラメータ PC 保存（ini 形式）
- **#92** HOST アプリ自動アップデート

### E. 機能追加（v2.1 以降へ）
- **#59** ウィンカー制御（鼓動ウィンカー、要 DI/DO ハード）
- **#115** EPS 駆動パルス生成 — chaketek 独自開発（枝 `dev_EPSdrvPLSgen`）。当面 main 統合予定なし、本プランの対象外として別管理
- **#40** 平均燃費計算
- **#32** 空燃比計算値オーバーフロー対応
- **#35** スイッチ操作で実装する低レベル操作（Trip / 輝度 / 時計 / ピークホールド）
- **#117** 油温・油圧センサー未搭載車への対応

### F. ハード起因（FW/移行と独立に別トラック管理）
- **#79** SE 出力アンプ S/D ピン論理間違い（プルダウンへ）
- **#47** FINE 書き込み不可（JTAG は可、ハード疑い）
- **#119** メーター固定ネジの穴径が狭い
- **#34** 警告灯アクリルパネル（つや消し・半透明・ブラックアウト）

---

## 推奨シーケンス

```
Phase 0（枝一本化）
  → Phase 1-①②（FPU移植＋CI刷新）
  → v2.0.0 リリース
  → Phase 2-A（検証クローズ）
  → Phase 2-C（警告しきい値の根治）
  → Phase 2-D（カスタム設定）
  → Phase 2-E / F（機能追加・ハード）
```

まず土台（統合ベース＋リリースブロッカー）を固め、その後に表示・警告系の実 Issue を新ベース上で確実に潰す。

---

## 参考ドキュメント（dev_LVGL_EEZ 上）
- `docs/lvgl_migration_design.md` — LVGL 移行設計書
- `docs/eez_design_switching.md` — EEZ デザイン切替システム
- `docs/eez003_prodrive_design.md` — eez003 Prodrive デザイン
- `docs/release_procedure.md` — リリース手順書（v2.0.0 / eez 反映で要更新）
