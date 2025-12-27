#!/bin/bash
set -e
echo "Установка зависимостей для Debian/Ubuntu..."
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    git \
    libboost-all-dev \
    libjemalloc-dev \
    libtcmalloc-minimal4 \
    hyperfine \
    python3-pip
pip3 install matplotlib numpy  # Для графиков, если нужно
echo "Зависимости установлены!"