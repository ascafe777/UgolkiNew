@echo off
set BUILD_DIR=build

REM Удаляем папку build, если она существует
if exist %BUILD_DIR% (
    echo Удаляем существующую папку %BUILD_DIR%...
    rd /s /q %BUILD_DIR%
)

REM Создаём папку build заново
mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Генерация файлов сборки и сборка проекта
cmake ..
cmake --build .

pause
