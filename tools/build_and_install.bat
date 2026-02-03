@echo off
echo === Building and Installing APK ===
cd /d C:\Users\tomoy\git\FULLMONI-WIDE\HostApp\FULLMONI-WIDE-Android

echo Building...
call gradlew.bat assembleDebug --quiet
if errorlevel 1 (
    echo Build FAILED
    pause
    exit /b 1
)

set ADB=C:\Users\tomoy\AppData\Local\Android\Sdk\platform-tools\adb.exe

echo Installing...
%ADB% install -r app\build\outputs\apk\debug\app-debug.apk
if errorlevel 1 (
    echo Install FAILED
    pause
    exit /b 1
)

echo Restarting app...
%ADB% shell am force-stop com.fullmoni.terminal
timeout /t 1 /nobreak > nul
%ADB% shell am start -n com.fullmoni.terminal/.MainActivity

echo === Done ===
pause
