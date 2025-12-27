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
pip install --user matplotlib numpy
echo "Зависимости установлены!"