#!/bin/bash
# Автоматическое определение ОС и установка зависимостей

set -euo pipefail  # ✅ Исправлено

echo "=== Установка зависимостей для проекта ==="

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

detect_os() {
    if [[ -f /etc/os-release ]]; then
        . /etc/os-release
        echo "$ID"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$MSYSTEM" == "MINGW"* ]] || [[ "$MSYSTEM" == "UCRT"* ]]; then
        echo "msys2"
    else
        echo "unknown"
    fi
}

OS=$(detect_os)

echo "Обнаружена ОС: $OS"

case $OS in
    ubuntu|debian|linuxmint|pop)
        echo "Запуск установки для Debian-based систем..."
        exec "$SCRIPT_DIR/install-debian.sh"
        ;;
    fedora|centos|rhel|rocky)
        echo "Запуск установки для Fedora/RHEL-based систем..."
        exec "$SCRIPT_DIR/install-fedora.sh"
        ;;
    arch|manjaro|endeavouros)
        echo "Запуск установки для Arch-based систем..."
        exec "$SCRIPT_DIR/install-arch.sh"
        ;;
    msys2)
        echo "Запуск установки для MSYS2 (Windows)..."
        echo "В MSYS2 используйте bash-скрипт:"
        echo "  ./scripts/install-deps.sh"
        echo "Или PowerShell скрипт:"
        echo "  powershell -ExecutionPolicy Bypass -File \"$SCRIPT_DIR/install-msys2.ps1\""
        exit 1
        ;;
    macos)
        echo "Запуск установки для macOS..."
        exec "$SCRIPT_DIR/install-macos.sh"
        ;;
    *)
        echo "ОС не распознана или не поддерживается."
        echo "Пожалуйста, выберите скрипт вручную из папки scripts/:"
        echo "  install-debian.sh   - Debian/Ubuntu/Mint"
        echo "  install-fedora.sh   - Fedora/RHEL/CentOS"
        echo "  install-arch.sh     - Arch/Manjaro"
        echo "  install-macos.sh    - macOS"
        echo "  install-msys2.ps1   - Windows (только из PowerShell в MSYS2)"
        exit 1
        ;;
esac