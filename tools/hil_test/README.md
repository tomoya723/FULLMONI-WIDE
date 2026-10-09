# HIL test framework (CAN → camera → VLM)

PCAN USB アダプタで `debug_env/fullmoni_MX5_can.dbc` ベースのテストパターンを
物理CANバスに流し、実機LCDをUSBカメラで撮影、Claude Code デスクトップ(`claude` CLI)
に期待内容と突き合わせてもらう回帰テストの枠組みです。

## 前提

- PCAN USBアダプタがこのPCに接続済み、`PCANBasic.dll`がインストール済み
  (確認済み: `C:\Windows\System32\PCANBasic.dll`)
- ファームウェアが書き込まれ、実機がCANバス・LCDとも動作中
- USBカメラがLCDを撮影できる位置に固定されている
- `claude` CLIがPATHに通っていてログイン済み(動作確認済み)。VLM判定は
  `claude -p ... --output-format json --permission-mode bypassPermissions`を
  サブプロセスとして都度呼び出す方式。ANTHROPIC_API_KEYは不要な代わりに、
  **シナリオ1件ごとにClaude Codeの新規セッションが立ち上がる**(会話は
  継続しない)ため、呼び出しごとにプロジェクトコンテキスト読み込み分の
  レイテンシ・コストがかかる点は留意(6シナリオ連続実行で数十秒〜数分、
  実コストは数十円〜百円規模/回を想定)。

```
pip install -r tools/hil_test/requirements.txt
```

## 使い方

1. カメラのデバイス番号とクロップ範囲を決める:
   ```
   cd tools/hil_test
   python calibrate.py --design eez002
   ```
   `test-results/eez002/calibration.png` を開いてLCDの矩形(x,y,w,h)を読み取り、
   `Firmware/eez/eez002/hil_test/camera.yaml` の `crop:` に書く。

2. シナリオ一覧を確認:
   ```
   python runner.py --design eez002 --list
   ```

3. 全シナリオ実行:
   ```
   python runner.py --design eez002
   ```
   結果は `test-results/eez002/<タイムスタンプ>/report.html` に出力される
   (キャプチャ画像・VLM判定・不一致点のサマリ)。

4. 一部のシナリオだけ実行:
   ```
   python runner.py --design eez002 --scenario oil_press_low_regression water_cold
   ```

## 構成管理の方針

- `tools/hil_test/`: テストエンジン本体(CAN送信・カメラ撮影・VLM判定・レポート生成)。
  デザイン(eez001/002/003)に依存しないロジックのみ。
- `Firmware/eez/<design>/hil_test/`: デザイン固有の設定
  (`camera.yaml`=カメラキャリブレーション、`scenarios.yaml`=CAN値と期待される表示内容)。
  `trackmode_screen_design/`と同じく「デザイン固有の資材はデザインと一緒に置く」方針。
  デザインを切り替えても(`switch_design.ps1`)このディレクトリ自体は`Firmware/eez/<design>/`
  配下に残るので、デザインごとに別々のシナリオ・カメラ設定を持てる。
- `debug_env/fullmoni_MX5_can.dbc`: 既存のDBCをそのまま単一ソースとして再利用。
  シナリオのCAN値はこのDBCの物理単位(°C, kPa, rpm, AFRなど)でそのまま書く。
- `test-results/`: 実行結果(キャプチャ画像・JSON・HTMLレポート)。gitignore対象、コミットしない。

## 既知の制約

- **TRACK画面(Screen1_1)はCAN経由で到達できない**。物理SW(PD0)で切り替える
  仕様のため、このリグではデフォルトのScreen1のみ検証できる。
- **バッテリー電圧(VOLT)・ブレーキ/排気温/シートベルトの各警告灯はCAN非経由**。
  `Firmware/src/ui_binding/ui_dashboard.c`を見ると、バッテリーはAD6、警告灯3種は
  AD2/AD3/AD4の生アナログ入力を直接読んでおり、CAN値を流しても変化しない。
  ベンチ電源やスイッチ治具など別の刺激が必要。
- `fullmoni_MX5_can.dbc`のIntakeAirTemperature/EngineTemp/OilTempは符号無し
  (`+`)として定義されているが、ファームウェア側(`param_storage.c`)はこれらを
  符号付き(`data_type=1`)として解釈している。正値のみ使う分には問題ないが、
  負値(氷点下IAT等)を正しく送受信できるかは別途確認した方がよい
  (DBCの符号属性の修正、または符号付きシグナルとしての再定義が必要かもしれない)。
