#!/bin/bash
set -e
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
pip install matplotlib numpy
echo "Зависимости установлены!"