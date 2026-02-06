@echo off
echo === Starting Android Emulator ===

REM Kill and restart adb server
C:\Users\tomoy\AppData\Local\Android\Sdk\platform-tools\adb.exe kill-server
timeout /t 2 /nobreak > nul
C:\Users\tomoy\AppData\Local\Android\Sdk\platform-tools\adb.exe start-server
timeout /t 2 /nobreak > nul

echo === Checking devices ===
C:\Users\tomoy\AppData\Local\Android\Sdk\platform-tools\adb.exe devices

echo === Available AVDs ===
C:\Users\tomoy\AppData\Local\Android\Sdk\emulator\emulator.exe -list-avds

echo.
echo Please check if emulator is running.
echo If not, start it from Android Studio or run:
echo   emulator -avd [AVD_NAME]
pause
