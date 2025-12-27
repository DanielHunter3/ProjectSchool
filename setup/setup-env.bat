@echo off
echo ========================================
echo   Настройка окружения для Windows
echo ========================================
echo.
echo Выберите действие:
echo 1 - Установить MSYS2 и зависимости (рекомендуется)
echo 2 - Только собрать проект (если зависимости уже установлены)
echo 3 - Только запустить тесты
echo.

set /p choice="Ваш выбор (1-3): "

if "%choice%"=="1" (
    echo Установка MSYS2...
    powershell -ExecutionPolicy Bypass -File "scripts\install-msys2.ps1"
) else if "%choice%"=="2" (
    call build-project.bat
) else if "%choice%"=="3" (
    call run-benchmarks.bat
) else (
    echo Неверный выбор
    pause
)