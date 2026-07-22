# device-observe — スマホ単体で実機LCDを観測する

RX72N 実機メーター（800x256 フルカラーLCD）を、**Androidスマホ1台で完結**して観測・
静的検証するための最小キット。Termux 上で「撮る → Claude が読む」ループを回す。

> **スコープ（重要）**
> これは**静的検証**（今この画面が正しいか）専用。**遷移・fps・応答遅延などの時間評価は
> スマホ単体では測れない**（カメラstreamingは遅延が大きくfps上限もある）。時間評価は
> フォトダイオード＋ロジアナ／ファームのVSyncログなど、別ノードにオフロードする。
> ここでは「まずスマホ単体でできること」に絞る。

## 全体像

```
[スマホをリグに固定] → IP Webcam アプリ(ローカル配信) or termux-camera-photo
        │
   Termux: capture.py  ── 任意で homography.json により 800x256 に矯正
        │
   observe_agent.py ── Claude(vision) が checklist と照合し PASS/FAIL を JSON 出力
```

頭脳・目・処理すべてが同じスマホ内。ネットは Claude API 呼び出しのみ（撮影は localhost）。

## 段階的に立ち上げる

### Level 0: 手動（コード不要・今すぐ）
スマホで実機を撮り、Claude アプリに貼って「意図どおりか」を聞く。感触を掴む用。

### Level 1: スクリプトで撮る
Termux で 1 フレームを PNG 化して手元に残す。

```bash
# Termux 準備
pkg install python ffmpeg termux-api
pip install pillow numpy          # 矯正を使う場合

# IP Webcam アプリを起動（露出/WB/フォーカスをロック推奨）してから:
python capture.py --source ipwebcam -o frame.png
# または追加アプリ無しで:
python capture.py --source termux -o frame.png
```

### Level 2: Claude が自動判定
```bash
pip install anthropic
export ANTHROPIC_API_KEY=...        # 判定モデルは OBSERVE_MODEL で上書き可

cp checklist.example.md checklist.md   # 実際の検証項目に編集
python observe_agent.py --checklist checklist.md
# ゴールデン参照（emWinシミュレータ画像など）と比較する場合:
python observe_agent.py --checklist checklist.md --golden golden.png
```

## 矯正（差分検証の生命線）

固定リグでも斜め・歪みが残ると比較がぶれる。四隅を一度校正して正準 800x256 に矯正する。

```bash
python capture.py -o ref.png                 # 基準フレーム
# ref.png を見て LCD 四隅の座標を読み、TL TR BR BL の順で:
python calibrate.py --corners "TLx,TLy TRx,TRy BRx,BRy BLx,BLy"
python capture.py --homography homography.json -o frame.png   # 以降は矯正済み
```

## 決定論性のための必須設定

自動補正が毎フレーム画を変えると差分がノイズだらけになる。撮影アプリ側で:

- **露出(ISO/シャッター)固定・オートホワイトバランスOFF・オートフォーカスOFF・HDR/自動補正OFF**
- 固定マウント＋遮光フード＋定常光（拡散LED）で映り込み・光変動を殺す
- バックライトPWMの縞が出る場合は露光をPWM周期の整数倍側に寄せる

## ファイル

| ファイル | 役割 |
|---|---|
| `capture.py` | 1フレーム取得（IP Webcam / termux）＋任意で矯正 |
| `calibrate.py` | パネル四隅から `homography.json` を生成 |
| `observe_agent.py` | 撮影→Claudeがchecklistで静的判定（JSON出力） |
| `checklist.example.md` | 検証項目のテンプレート |

## この先（スマホ単体を超える時）

- **時間評価**（遷移/fps/応答）→ フォトダイオード＋ロジアナ、ファームのVSyncログ
- **HIL**（状態を作って検証）→ USB-CAN で CAN 注入（`debug_env/NODESIM` のロジックを実CANへ）
- どちらも「実機を叩く手」として PC か Raspberry Pi をもう1ノード足すのが現実解。
  本キットの `capture.py`/`observe_agent.py` はそのノードでもそのまま使える。
