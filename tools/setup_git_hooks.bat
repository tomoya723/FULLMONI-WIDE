@echo off
REM Git hooksをセットアップして linker_script.ld をe2 studioの破損から保護
REM 
REM このスクリプトは初回クローン後に1回実行してください

cd /d "%~dp0\.."

echo Setting up Git hooks...
git config core.hooksPath tools/git-hooks
echo Git hooks path set to: tools/git-hooks

REM 現在のlinker_script.ldを読み取り専用に設定
if exist "Firmware\src\linker_script.ld" (
    attrib +r "Firmware\src\linker_script.ld"
    echo Protected: Firmware\src\linker_script.ld (read-only)
)

echo.
echo Setup complete!
echo.
echo NOTE: linker_script.ld is now protected from e2 studio modifications.
echo       To edit it, temporarily remove read-only: attrib -r Firmware\src\linker_script.ld
echo       After editing, restore protection: attrib +r Firmware\src\linker_script.ld
pause
