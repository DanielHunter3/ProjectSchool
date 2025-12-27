#!/bin/bash
set -euo pipefail
echo "Установка зависимостей для Debian/Ubuntu..."
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libjemalloc-dev \
    libgoogle-perftools-dev \
    hyperfine \
    python3-pip

pip3 install --user matplotlib numpy

echo "✅ Зависимости установлены!"
exit 0