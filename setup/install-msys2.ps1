Write-Host "=== Установка зависимостей для Windows через MSYS2 ===" -ForegroundColor Green
Write-Host "Этот скрипт должен запускаться из MSYS2 UCRT64/MINGW64!" -ForegroundColor Yellow
Write-Host ""
Write-Host "Если MSYS2 не установлен:"
Write-Host "1. Скачайте с https://www.msys2.org/"
Write-Host "2. Установите и запустите MSYS2 UCRT64 из меню Пуск"
Write-Host "3. Запустите этот скрипт снова"
Write-Host ""

$choice = Read-Host "Продолжить установку? (y/n)"
if ($choice -ne 'y') { 
    Write-Host "Отмена установки." -ForegroundColor Yellow
    exit 1 
}

# Проверяем, что мы в MSYS2
if (-not $env:MSYSTEM) {
    Write-Host "❌ ОШИБКА: Этот скрипт должен запускаться из MSYS2!" -ForegroundColor Red
    Write-Host "   Запустите MSYS2 UCRT64 и попробуйте снова." -ForegroundColor Yellow
    exit 1
}

Write-Host "Обновление пакетов MSYS2..." -ForegroundColor Cyan
pacman -Syu --noconfirm

Write-Host "Установка зависимостей..." -ForegroundColor Cyan

# Список пакетов
$packages = @(
    "mingw-w64-ucrt-x86_64-toolchain",
    "mingw-w64-ucrt-x86_64-cmake", 
    "git",
    "mingw-w64-ucrt-x86_64-boost",
    "mingw-w64-ucrt-x86_64-jemalloc",
    "mingw-w64-ucrt-x86_64-hyperfine",
    "mingw-w64-ucrt-x86_64-python-pip"
)

# Устанавливаем все пакеты
foreach ($pkg in $packages) {
    Write-Host "Установка $pkg..." -ForegroundColor Gray
    pacman -S --needed --noconfirm $pkg
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ Ошибка установки $pkg" -ForegroundColor Red
        exit 1
    }
}

Write-Host "Установка Python библиотек..." -ForegroundColor Cyan
pip install matplotlib numpy
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Ошибка установки Python библиотек" -ForegroundColor Red
    exit 1
}

Write-Host "`n✅ Зависимости установлены!" -ForegroundColor Green
Write-Host "Для сборки проекта запустите:" -ForegroundColor Cyan
Write-Host "  mkdir build && cd build" -ForegroundColor White
Write-Host "  cmake -G 'Ninja' -DCMAKE_BUILD_TYPE=Release .." -ForegroundColor White
Write-Host "  cmake --build . --parallel" -ForegroundColor White