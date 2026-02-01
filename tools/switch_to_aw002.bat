@echo off
REM ========================================
REM  Switch variant to aw002 (Red Theme)
REM ========================================
cd /d "%~dp0..\Firmware"
if exist aw (
    rmdir aw
)
mklink /J aw aw002
echo.
echo *** Switched to aw002 (Red Theme) ***
echo.
echo Next: e2 studio で Clean → Build → Debug
echo.
pause
