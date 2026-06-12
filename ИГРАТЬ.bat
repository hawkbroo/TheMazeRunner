@echo off
chcp 65001 >nul
title The Maze Runner
cd /d "%~dp0"

if not exist "third_party\SFML\bin\sfml-graphics-d-2.dll" (
    echo SFML не установлен. Запускаю SETUP.bat...
    call "%~dp0SETUP.bat"
)

set "EXE=x64\Debug\TheMazeRunner.exe"
if exist "%EXE%" (
    start "" "%EXE%"
    exit /b 0
)

echo.
echo  Игра ещё не собрана.
echo  1. Дважды щёлкните SETUP.bat  ^(если не делали^)
echo  2. Откройте TheMazeRunner.sln в Visual Studio
echo  3. Выберите x64 и Debug, нажмите Ctrl+F5
echo.
start "" "%~dp0TheMazeRunner.sln"
pause
