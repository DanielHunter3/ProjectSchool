#!/bin/bash
set -e
echo "Установка зависимостей для macOS..."

# Проверка Homebrew
if ! command -v brew &> /dev/null; then
    echo "Установка Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

brew install \
    cmake \
    boost \
    jemalloc \
    gperftools \
    hyperfine

pip3 install matplotlib numpy

echo "Зависимости установлены!"