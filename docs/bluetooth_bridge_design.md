# Bluetoothブリッジデバイス 設計書

## 概要

Windows/AndroidホストアプリとFULLMONI-WIDE本体の通信を、USB直結からBluetooth経由に変更する。
**本体ハードウェア・ファームウェアは一切変更しない**（USB CDC / VID 0x1209, PID 0x7230 のまま）。
代わりに、USBホストとして本体に接続し、Bluetooth SPPでホストアプリと通信する
**ブリッジデバイス**（マイコンボード1枚）を間に追加する。

```
【現行】
  [Windows PC / Android] ──USBケーブル── [FULLMONI-WIDE (RX72N, USB CDCデバイス)]

【本設計】
  [Windows PC / Android] ~~Bluetooth SPP~~ [ブリッジ (Pico 2 W)] ──USBケーブル── [FULLMONI-WIDE]
                                             USBホスト(CDC-ACM)      ※現行ケーブル流用
```

ブリッジは**完全に透過的なバイトパイプ**として動作する。既存のプロトコル
（パラメータコンソール、CAN設定、ファームウェア更新、起動画像転送）に
プロトコル変更を加えない。

## 方式決定の経緯

### Bluetooth方式: BT Classic SPP（RFCOMM）を採用

| 観点 | BT Classic SPP（採用） | BLE (GATT/NUS) |
|------|------------------------|-----------------|
| Windowsホスト改修 | **ほぼ不要**（仮想COMポートに見える。既存 `SerialPortService` がそのまま動く） | 大（WinRT BLE APIの組み込み、接続UI刷新が必要） |
| Androidホスト改修 | 中（`BluetoothSocket` によるSPPトランスポート追加） | 中（GATTクライアント追加） |
| スループット | 実効 30〜60KB/s 程度（要実測） | 同等〜やや劣る（2M PHY時は上回る場合あり） |
| ストリーム性 | RFCOMMは信頼性・フロー制御付きストリーム。既存の「ストリーミング転送＋ACK」プロトコルと相性が良い | パケット指向。ストリーム化の実装が別途必要 |
| iOS対応 | 不可（MFi必須） | 可 |

Windows側の改修コストが支配的なため、SPPを採用する。
CYW43439はデュアルモード（Classic + BLE）なので、将来iOS対応が必要になった場合は
**同一ハードウェアのままBLE(NUS)サービスを追加**できる（[将来拡張](#将来拡張)参照）。

### ブリッジMCU: Raspberry Pi Pico 2 W を採用

必須要件は「**USBホスト（CDC-ACMホストクラス）**」と「**Bluetooth**」の両立。
本体側がUSB CDC**デバイス**であるため、ブリッジは必ずUSB**ホスト**になる点が選定を制約する。

| 候補 | USBホスト | BT Classic SPP | 評価 |
|------|-----------|----------------|------|
| **Raspberry Pi Pico 2 W**（採用） | ○ RP2350内蔵コントローラ + TinyUSBホスト（`CFG_TUH_CDC`） | ○ CYW43439 + BTstack（pico-sdk同梱、SPPサーバ実装例あり） | 市販ボード1枚で完結。約¥1,500 |
| ESP32-S3 | ○ USB OTG + `cdc_acm_host` | × BLEのみ | Windows側BLE改修が重い |
| ESP32（無印） | ×（MAX3421E等の外付けIC必須） | ○ | 回路が複雑化。ボード1枚で済まない |
| Raspberry Pi Zero 2 W | ○ | ○ | Linux起動時間・SDカード信頼性が車載に不向き |

Pico W（RP2040版）でも同一設計で動作するが、RP2350の余裕（メモリ520KB）と
今後の入手性から Pico 2 W を第一候補とする。

## システム構成

### ハードウェア構成

```
  車両 12V (ACC)
       │
  [12V→5V DC/DCコンバータ]  ※市販の車載USB電源でも可
       │ 5V
       ├────────────→ Pico 2 W  VBUSピン(40番)へ給電
       │                   │
       │                   │ micro-USB (OTGアダプタ経由、ホストモード)
       │                   │ VBUS 5V出力（VBUSピンから連通）
       │                   ▼
       │             [現行USBケーブル]
       │                   │
       ▼                   ▼
  [FULLMONI-WIDE本体] ←──USBコネクタ (CDCデバイス, セルフパワー)
   (本体は従来通り車両12Vから給電)
```

- **給電**: Pico 2 W はVBUSピン（micro-USBコネクタのVBUSに直結）へ5Vを供給して駆動する。
  ホストモード時はこの5Vがそのまま本体側VBUSに出力され、本体のVBUS検出に使われる
  （本体はセルフパワーなので電力はほぼ流れない）。
- **接続**: Pico の micro-USB に OTGアダプタ（micro-B プラグ → USB-A レセプタクル）を挿し、
  現行のUSBケーブルをそのまま接続する。
- **LED**: Pico オンボードLED（CYW43経由）でステータス表示
  （消灯=USB未接続 / 点滅=BT待受中 / 点灯=BT接続中）。
- デバッグはUART0（GP0/GP1）またはSWDで行う（USBポートはホストとして占有されるため）。

### BOM（部品表）

| 品目 | 数量 | 参考価格 | 備考 |
|------|------|----------|------|
| Raspberry Pi Pico 2 W | 1 | 約¥1,500 | Pico W でも可 |
| micro-USB OTGアダプタ | 1 | 約¥300 | micro-Bプラグ → USB-Aメス |
| 12V→5V USB電源（車載用） | 1 | 約¥1,000 | 5V/1Aで十分 |
| USBケーブル（本体接続用） | - | - | **現行品を流用** |

合計 約¥3,000。専用PCB・ケースは動作実証後に検討（Phase 4）。

## ブリッジファームウェア設計

### ソフトウェアスタック

- **SDK**: pico-sdk（C/C++）
- **USBホスト**: TinyUSB ホストスタック（`tuh_*`）、CDC-ACMホストドライバ（`CFG_TUH_CDC=1`）
- **Bluetooth**: BTstack（pico-sdk同梱、`pico_btstack_classic` + `pico_btstack_cyw43`）
  - RFCOMM上にSPPサーバを立てる（SDPでSerial Port Profileを広告）
  - デバイス名: `FULLMONI-WIDE BT`（インクワイアリで発見可能）

### アーキテクチャ

```
            ┌────────────── Pico 2 W ──────────────┐
 BT SPP     │  ┌──────────┐   ┌──────────────┐    │   USB Host
 (RFCOMM)   │  │ BTstack  │   │ TinyUSB Host │    │   (CDC-ACM)
◄──────────►│  │ SPPサーバ │   │  tuh_cdc     │    │◄──────────► FULLMONI-WIDE
            │  └────┬─────┘   └──────┬───────┘    │
            │       │    リングバッファ │            │
            │       │  ┌────────────┐│            │
            │       ├─►│ bt→usb 16KB├┼─►         │
            │       │  └────────────┘│            │
            │       │  ┌────────────┐│            │
            │      ◄┼──┤ usb→bt 16KB│◄┼──         │
            │       │  └────────────┘│            │
            │  メインループ（ポーリング型、RTOSなし）  │
            └──────────────────────────────────────┘
```

- **単一メインループ**（`btstack_run_loop` + `tuh_task()` ポーリング）。RTOS不使用でシンプルに保つ。
- **リングバッファ**: 双方向 各16KB（RP2350のSRAM 520KBに対し余裕十分。必要なら64KBまで拡大可）。

### フロー制御（データ落ち防止）— 本設計の要

ブリッジ内でデータを落とさないため、**エンドツーエンドのバックプレッシャ**を両方向に成立させる。

| 方向 | 速い側 | 遅い側 | 制御方法 |
|------|--------|--------|----------|
| ホスト→本体 | BT (30〜60KB/s) | — | USB側(12Mbps)が常に速いため通常は溢れない。万一 `tuh_cdc_write` が詰まった場合は **RFCOMMクレジットの付与を停止**し、ホスト側送信をブロックする（RFCOMMのcredit-based flow controlを利用） |
| 本体→ホスト | USB (実効268KB/s) | BT | usb→btバッファに空きがない間は **CDCホストの受信(IN転送)を発行しない**。本体側USBスタックはNAKにより自然に送信待ちとなり、ファームウェアの送信処理がブロックする |

RFCOMMは再送を含む信頼性付きストリームのため、無線区間でのビット化けやパケットロスは
リンク層で回復される。ブリッジがバッファを溢れさせない限り、**バイト列の完全性は保証**される。
これにより「中間ACKなしストリーミング」のファームウェア転送プロトコルもそのまま成立する。

### USB再列挙への対応（ファームウェア更新時に必須）

`fwupdate` 実行時、本体はリブートしてBootloaderとして**USB再列挙**する（数秒間デバイス消失）。
現行ホストアプリはこの間「COMポート消失→再出現待ち→再接続」を行うが、
BT経由ではSPPリンクは切れないため、以下の挙動差を吸収する。

**ブリッジの動作仕様:**

1. `tuh_umount_cb`（デバイス切断）を検出したら、**bt→usbバッファへの蓄積を継続**する
   （上限4KB・タイムアウト30秒。超過分は破棄しカウントのみ記録）。
2. `tuh_mount_cb`（再列挙完了）で、蓄積データをBootloaderへ**順次フラッシュ**する。
3. デバイス切断中もSPPリンクは維持する。
4. SPP側が切断→再接続された場合（Windowsの `ReconnectAsync` がBT仮想COMを
   閉じて開き直すケース）は、**usb→btバッファを破棄してから**新規接続に応じる
   （古い受信データの混入防止）。

これにより既存ホストアプリのシーケンス
（`fwupdate`→`yes`→1秒待ち→再接続→改行送信→バナー待ち10秒→`U`→…）は
以下の通り無改修で成立する:

- Windowsの `ReconnectAsync`: BT仮想COMポートは消えないため即座に「再接続成功」する。
  再接続後に送る改行はブリッジがバッファし、Bootloader列挙後（実測1〜2秒）に届く。
  バナー待ち10秒以内に応答が返るため成立。
- Androidの `waitForDeviceAndConnect`: USB前提の実装のため、トランスポート抽象化の際に
  「BTでは接続維持のままバナー待ちへ進む」動作に差し替える（[ホストアプリ改修方針](#ホストアプリ改修方針)参照）。

### CDC制御信号の扱い

- 本体ファームウェア/Bootloaderは `SET_CONTROL_LINE_STATE`（DTR/RTS）を**ACKするのみで機能的に未使用**
  （`usb_cdc.c:171`, `main_fit_cdc.c:561`）。よってブリッジはBT側のモデム信号を転送しない。
- CDCマウント時にライン設定 115200bps 8N1 を一度送出する（本体は無視するが慣例として）。
- ボーレートはBT区間には存在しない。ホストアプリのボーレート設定値は実質無意味になる（害もない）。

### Bluetoothペアリング・セキュリティ

車載環境では「近傍の第三者が勝手にペアリングして設定変更できる」ことがリスクになる。

| 項目 | 仕様 |
|------|------|
| ペアリング方式 | SSP "Just Works"（Picoに表示・入力手段がないため） |
| ペアリング受付 | **電源投入後60秒間のみ** discoverable/connectable。以降は新規ペアリング拒否（ペアリング済みデバイスの再接続は常時許可） |
| 登録上限 | リンクキー保存は最大4台（フラッシュに保存、`pico_btstack_flash_bank` 利用） |
| 同時接続 | SPP接続は**1本のみ**。接続中は新規接続を拒否 |
| 消去手段 | BOOTSELボタン長押し起動などでペアリング情報全消去（実装時に確定） |

> 補足: 通信内容は車両メーター設定であり機密性は低いが、走行中の意図しない
> パラメータ書き換えは安全に関わるため、上記の「ペアリング時間窓」を必須仕様とする。

### ブリッジ自身の設定・診断

ブリッジは透過パイプに徹し、**インバンドのエスケープコマンドは設けない**
（バイナリ転送との衝突リスクを避けるため）。診断はLEDとデバッグUARTログのみ。
将来必要になれば、BT側に2本目のRFCOMMチャネル（診断用SPP）を追加する。

## プロトコル透過性の検証（ユースケース別）

既存の全ユースケースについて、BT経由での成立性と所要時間見積りを示す。
（BT実効スループットは保守的に30KB/s、RTT 50msと仮定。要実測）

| ユースケース | プロトコル | データ量 | USB直結 | BT経由（見積） | 成立性 |
|--------------|-----------|----------|---------|----------------|--------|
| パラメータコンソール | 行単位テキスト | 〜数KB | 即時 | 即時（体感差なし） | ○ 完全透過 |
| CAN設定読み書き | テキスト（`can_list` 等） | 〜数KB | 即時 | 即時 | ○ 完全透過 |
| ファームウェア更新 | サイズ+ストリーミング（中間ACKなし） | 約232KB | 転送約1秒+消去 | **転送約8秒**+消去 | ○ RFCOMMの信頼性+バックプレッシャで成立。USB再列挙はブリッジが吸収 |
| 起動画像 書き込み | チャンク毎ACK(0x06) | 約392KB (765×256×2) | 数秒 | **約13秒+ACK往復約5秒** | ○ チャンクACK型はレイテンシの影響を受けるが許容範囲 |
| 起動画像 読み出し | 本体→ホスト ストリーミング | 約392KB | 約1.5秒 | **約13秒** | ○ USB側NAKフロー制御で成立 |
| Bootloader復旧モード | 直接Bootloaderと対話 | - | - | 同上 | ○ ブリッジは列挙されたCDCデバイスを区別なく中継 |

**注意点:**

- ホストアプリの `DiscardBuffers()` はホストローカルのバッファしか消せない。
  ブリッジ内バッファの古いデータ混入は「SPP再接続時にusb→btバッファ破棄」で対処する
  （通常運用ではプロトコル上問題にならないことを実機で確認する）。
- タイムアウト値の再確認: `ChunkAckTimeoutMs=5000` 等はBTレイテンシ込みでも十分。
  唯一 `AckTimeoutMs`（サイズACK 10秒）は消去時間＋BT遅延でも余裕あり。**既存値のまま変更不要**の見込み。

## ホストアプリ改修方針

> 本セクションは次フェーズ（実装時）の方針。今回は設計のみ。

### Windows（FULLMONI-WIDE Terminal）— 原則無改修

BT Classic SPPをWindowsでペアリングすると**仮想COMポート**が生成され、
既存の `SerialPortService`（System.IO.Ports）がそのまま使える。

- ユーザー手順: Windows設定でペアリング → 生成された**発信（Outgoing）側COMポート**をアプリで選択。
- 既知の注意点:
  - Windowsは着信/発信の2つのCOMポートを作る。ユーザーが誤選択しやすいため、
    READMEに手順を記載する（任意改善: ポート一覧にBTフレンドリー名を併記する小改修）。
  - BT仮想COMのOpenは物理COMより遅い（2〜5秒）。`ReconnectAsync` のリトライ
    （500ms×20回=10秒）でカバーされるため改修不要。
  - `DtrEnable`/`RtsEnable` はBT COMでは実質無効だが害なし。

### Android（FULLMONI-WIDE Terminal）— トランスポート抽象化 + SPP追加

`UsbSerialService` には既にUSB/シミュレータ/TCPブリッジの3モードが同居しており、
これを機に**トランスポート層を抽象化**する。

```kotlin
interface SerialTransport {
    suspend fun connect(): Boolean
    fun disconnect()
    fun write(data: ByteArray)
    val incoming: Flow<ByteArray>
    /** fwupdate時の再接続。USB=デバイス再列挙待ち、BT=接続維持のためno-op */
    suspend fun awaitReconnect(timeoutMs: Long): Boolean
}
// 実装: UsbCdcTransport / BluetoothSppTransport / TcpBridgeTransport / SimulatorTransport
```

- `BluetoothSppTransport`: `BluetoothDevice.createRfcommSocketToServiceRecord(SPP_UUID)`
  （UUID: `00001101-0000-1000-8000-00805F9B34FB`）。
- 権限: `BLUETOOTH_CONNECT`（API 31+、実行時パーミッション）。`AndroidManifest.xml` に追加。
- `FirmwareUpdateService.kt` の `waitForDeviceAndConnect` 呼び出しを
  `transport.awaitReconnect()` に置換（BTではソケット維持のまま即true）。
- UI: 接続画面に「Bluetooth」タブを追加し、ペアリング済みデバイス一覧から選択。

## リスクと対策

| # | リスク | 影響 | 対策 |
|---|--------|------|------|
| 1 | BT実効スループットが見積りを下回る | 転送時間増 | Phase 1で `spp_streamer` 相当のスループット実測を最初に実施。30KB/s未満ならバッファ/MTU調整、最悪BLE 2M PHY方式へ転換判断 |
| 2 | USB再列挙タイミングとホスト送信の競合 | FW更新失敗 | ブリッジのbt→usbバッファリング（4KB/30秒）で吸収。Phase 2でfwupdateを10回連続成功するまで反復試験 |
| 3 | 車載ノイズ環境でのBT切断 | 操作中断 | RFCOMM再接続で復旧可能。FW更新はBootloaderが待ち続けるため、再接続後に最初からやり直せば復旧する（Bootloader復旧モードは既存機能） |
| 4 | 第三者による不正ペアリング | 設定改ざん | ペアリング時間窓（電源投入後60秒）+ 接続1本制限 |
| 5 | Pico 2 W 供給不安定 | 製作不可 | Pico W (RP2040)でも同一設計で動作可能なようSDK構成を保つ |
| 6 | Windowsの2重COMポートによる誤選択 | 接続失敗 | README手順書化。必要ならポート名にBT名併記の小改修 |

## 実装フェーズ計画

| Phase | 内容 | 完了条件 |
|-------|------|----------|
| **1. スループット実証** | Pico 2 W + BTstack SPPサーバ + TinyUSB CDCホストの疎通。ループバック/実機でスループット・RTT実測 | Windows/AndroidからBT経由でパラメータコンソールが操作できる。実効速度の実測値取得 |
| **2. ブリッジFW完成** | リングバッファ・フロー制御・再列挙バッファリング・ペアリング時間窓・LED実装 | fwupdate 10回連続成功、起動画像read/write成功（Windows無改修で） |
| **3. Androidトランスポート抽象化** | `SerialTransport` 導入 + `BluetoothSppTransport` 実装 + UI追加 | Android実機で全機能（コンソール/CAN設定/FW更新/画像転送）成功 |
| **4. 製品化検討** | 専用キャリアPCB（12V入力DC/DC統載）、ケース、リポジトリへ `BridgeDevice/` ディレクトリ追加 | 車載実装例の公開 |

リポジトリ構成（Phase 2以降）:

```
BridgeDevice/
├── firmware/          # Pico 2 W ファームウェア (pico-sdk, CMake)
│   ├── CMakeLists.txt
│   └── src/
│       ├── main.c             # メインループ
│       ├── bt_spp.c/.h        # BTstack SPPサーバ・ペアリング管理
│       ├── usb_cdc_host.c/.h  # TinyUSB CDCホスト・再列挙処理
│       └── bridge_buffer.c/.h # リングバッファ・フロー制御
├── hardware/          # (Phase 4) キャリアPCB
└── README.md          # 組み立て・ペアリング手順
```

## テスト計画（Phase 2完了判定用）

1. **基本疎通**: パラメータコンソール全コマンドの入出力一致（USB直結時とバイト列比較）
2. **FW更新耐久**: `fwupdate` 10回連続成功（Windows/各1回はBootloader復旧モードから）
3. **画像転送**: 起動画像 write→read→バイナリ一致検証
4. **フロー制御**: 画像read中にホスト側読み出しを意図的に遅延させ、データ欠落なしを確認
5. **切断復旧**: FW転送中にBT切断→再接続→リトライで復旧できること
6. **セキュリティ**: 電源投入61秒後に新規ペアリングが拒否されること
7. **長時間**: コンソール接続24時間放置でリンク維持（またはLED表示通りの自動再接続）

## 将来拡張

- **BLE併設（iOS対応）**: CYW43439はデュアルモードのため、BTstackでSPPと並行して
  BLE NUS（Nordic UART Service互換）を広告可能。iOSアプリ開発時にブリッジFWへ追加する。
  ホストプロトコルは同一バイトストリームのため、トランスポート追加のみで対応可能。
- **ブリッジ自身の無線書き換え**: BTstack経由でPicoのフラッシュを更新するDFU機能
  （優先度低。当面はUSBのBOOTSELで書き換え）。
- **診断チャネル**: 2本目のRFCOMMチャネルでブリッジのログ・統計（バッファ使用率、
  再列挙回数、破棄バイト数）を取得。

## 参考情報

- 本体USB識別子: VID `0x1209` (pid.codes) / PID `0x7230`（Firmware/Bootloader共通、`FULLMONI-WIDE.inf`）
- ファームウェアサイズ: 約232KB（`release-manifest.json`）
- 起動画像サイズ: 765×256×2byte ≒ 392KB
- 既存プロトコル実装:
  - Windows: `HostApp/FULLMONI-WIDE-Terminal/Services/FirmwareUpdateService.cs`（サイズ+ストリーミング）、
    `StartupImageService.cs`（チャンク+ACK 0x06）
  - Android: `HostApp/FULLMONI-WIDE-Android/app/src/main/java/com/fullmoni/terminal/service/`
  - 本体: `Firmware/src/usb_cdc.c`、`Bootloader/src/main_fit_cdc.c`
- BTstack SPPサーバ例: pico-examples `pico_w/bt/standalone`、BTstack `spp_streamer`
- TinyUSB CDCホスト例: `examples/host/cdc_msc_hid`
