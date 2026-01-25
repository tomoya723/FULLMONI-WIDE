# GitHub Copilot ルール

このワークスペースで作業する際の必須ルール：

## 禁止事項

### 1. Smart Configuratorで管理するファイルを直接編集しない
以下のファイル/フォルダは Smart Configurator (e2 studio) が自動生成・管理するため、**絶対に直接編集しないこと**：
- `src/smc_gen/` 配下のすべてのファイル
- `src/smc_gen/r_config/` 配下の設定ファイル
- `qe_emwin_config.h`
- `*.scfg` ファイル

これらのパラメータを変更する必要がある場合は、ユーザーに「Smart Configuratorで変更してください」と伝えること。

### 2. 勝手にGIT操作をしない
以下のGITコマンドは**ユーザーの明示的な許可なく実行しないこと**：
- `git checkout`
- `git reset`
- `git revert`
- `git stash`
- `git merge`
- `git rebase`
- `git cherry-pick`
- `git restore`

`git status`, `git diff`, `git log` などの読み取り専用コマンドは可。

### 3. リンカスクリプトのメモリ領域変更は慎重に
`linker_script.ld` のメモリ領域（RAM, RAM2, ROM等）を変更する場合は、LCDフレームバッファ等の既存配置と重複しないことを必ず確認すること。

## 開発環境
- MCU: Renesas RX72N
- IDE: e2 studio (Smart Configurator)
- GUI: emWin (AppWizard)
- ツールチェーン: GCC-RX
