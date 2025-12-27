#!/bin/bash
set -euo pipefail
echo "Установка зависимостей для Arch Linux..."
sudo pacman -Syu --needed \
    base-devel \
    cmake \
    git \
    boost \
    jemalloc \
    gperftools \
    hyperfine \
    python-pip

# Используем явно python или pip3
if command -v python &> /dev/null; then
    python -m pip install --user matplotlib numpy
elif command -v pip3 &> /dev/null; then
    pip3 install --user matplotlib numpy
else
    pip install --user matplotlib numpy
fi

echo "✅ Зависимости установлены!"
exit 0