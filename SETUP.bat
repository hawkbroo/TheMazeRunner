@echo off
chcp 65001 >nul
title The Maze Runner — установка SFML
echo.
echo  === The Maze Runner: установка SFML ===
echo.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\setup_sfml.ps1"
if errorlevel 1 (
    echo.
    pause
    exit /b 1
)
echo.
pause
