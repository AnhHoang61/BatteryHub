@echo off
rem Lightweight offline single-file build: zh-CN/zh-TW resources, no exceptions/RTTI, no updater/reporter/network libs
setlocal enabledelayedexpansion
cd /d "%~dp0"
if not exist build mkdir build
set "APP_VERSION=%~1"
if not defined APP_VERSION set "APP_VERSION=1.1.0"
set "APP_CHANNEL=%~2"
if not defined APP_CHANNEL set "APP_CHANNEL=dev"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts\generate-version.ps1 -Version "%APP_VERSION%" -Channel "%APP_CHANNEL%" || exit /b 1

rem Pack only zh-CN and zh-TW (English is built into the code as fallback)
(for %%f in (lang\zh-CN.txt lang\zh-TW.txt) do (echo @file %%~nf& type "%%f"& echo.)) > build\languages.txt
windres -i res\app.rc -o build\app_res.o --include-dir res --include-dir build || exit /b 1

set SOURCES=
for %%f in (src\*.cpp src\drivers\*.cpp) do (
    set "FNAME=%%~nxf"
    if /i not "!FNAME!"=="report.cpp" if /i not "!FNAME:~0,7!"=="update_" set SOURCES=!SOURCES! %%f
)

set COMMON_FLAGS=-std=c++17 -Os -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -fno-unwind-tables -municode -mwindows -static -s -ffunction-sections -fdata-sections -Wl,--gc-sections -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -Wall -Wno-missing-field-initializers
set LIBS=-lhid -lsetupapi -lcfgmgr32 -lole32 -loleaut32 -lcomctl32 -lgdi32 -lruntimeobject -lws2_32 -lversion -lbluetoothapis

g++ %COMMON_FLAGS% !SOURCES! build\app_res.o -o BatteryHub.exe %LIBS% || exit /b 1
echo Built lightweight BatteryHub.exe
