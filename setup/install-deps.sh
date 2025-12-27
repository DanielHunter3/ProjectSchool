#!/bin/bash
# Автоматическое определение ОС и установка зависимостей

echo "=== Установка зависимостей для проекта ==="

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
        exec "$(dirname "$0")/install-debian.sh"
        ;;
    fedora|centos|rhel|rocky)
        echo "Запуск установки для Fedora/RHEL-based систем..."
        exec "$(dirname "$0")/install-fedora.sh"
        ;;
    arch|manjaro|endeavouros)
        echo "Запуск установки для Arch-based систем..."
        exec "$(dirname "$0")/install-arch.sh"
        ;;
    msys2)
        echo "Запуск установки для MSYS2 (Windows)..."
        exec "$(dirname "$0")/install-msys2.ps1"
        ;;
    macos)
        echo "Запуск установки для macOS..."
        exec "$(dirname "$0")/install-macos.sh"
        ;;
    *)
        echo "ОС не распознана или не поддерживается."
        echo "Пожалуйста, выберите скрипт вручную из папки scripts/"
        exit 1
        ;;
esac