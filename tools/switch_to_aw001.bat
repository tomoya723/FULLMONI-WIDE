@echo off
REM ========================================
REM  Switch variant to aw001 (Blue Theme)
REM ========================================
cd /d "%~dp0..\Firmware"
if exist aw (
    rmdir aw
)
mklink /J aw aw001
echo.
echo *** Switched to aw001 (Blue Theme) ***
echo.
echo Next: e2 studio で Clean → Build → Debug
echo.
pause
