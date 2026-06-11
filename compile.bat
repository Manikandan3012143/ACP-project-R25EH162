@echo off
title Vector-CLI 2D Graphics Editor Compiler
echo ===================================================
echo   Compiling Vector-CLI 2D Graphics Editor in C...
echo ===================================================
echo.

REM Set PATH to include MinGW bin directory
set PATH=C:\MinGW\bin;%PATH%

REM Compilation command
gcc.exe -std=c99 -O2 graphics_editor.c -o graphics_editor.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Compilation failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [SUCCESS] Compilation completed successfully!
echo Launching graphics_editor.exe...
echo.
graphics_editor.exe
