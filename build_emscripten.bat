@echo off
echo Checking for Emscripten installation...

where emcc >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Emscripten not found in PATH!
    echo Please install Emscripten from https://emscripten.org/docs/getting_started/downloads.html
    echo After installation, run 'emsdk_env.bat' from your emsdk directory
    pause
    exit /b 1
)

echo Emscripten found, proceeding with build...
echo Building Space Invaders for Emscripten...
mingw32-make PLATFORM=PLATFORM_WEB CFLAGS="-DEMSCRIPTEN_BUILD -Wall -std=c++14 -D_DEFAULT_SOURCE -Wno-missing-braces -s -O1" all

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo Build complete!
pause 