Write-Host "=== Установка зависимостей для Windows через MSYS2 ===" -ForegroundColor Green
Write-Host "Этот скрипт должен запускаться из MSYS2 UCRT64/MINGW64!" -ForegroundColor Yellow
Write-Host ""
Write-Host "Если MSYS2 не установлен:"
Write-Host "1. Скачайте с https://www.msys2.org/"
Write-Host "2. Установите и запустите MSYS2 UCRT64 из меню Пуск"
Write-Host "3. Запустите этот скрипт снова"
Write-Host ""

$choice = Read-Host "Продолжить установку? (y/n)"
if ($choice -ne 'y') { exit }

# Команды для MSYS2
bash -c "pacman -Syu --needed `
    mingw-w64-ucrt-x86_64-toolchain `
    mingw-w64-ucrt-x86_64-cmake `
    git `
    mingw-w64-ucrt-x86_64-boost `
    mingw-w64-ucrt-x86_64-jemalloc `
    mingw-w64-ucrt-x86_64-hyperfine `
    mingw-w64-ucrt-x86_64-python-pip`

pip install matplotlib numpy"

Write-Host "`nЗависимости установлены!" -ForegroundColor Green
Write-Host "Для сборки проекта запустите: ./build-project.sh" -ForegroundColor Cyan