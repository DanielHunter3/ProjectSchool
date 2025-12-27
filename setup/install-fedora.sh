#!/bin/bash
set -e
echo "Установка зависимостей для Fedora/RHEL..."
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    boost-devel \
    jemalloc-devel \
    gperftools-devel \
    hyperfine \
    python3-pip
pip3 install matplotlib numpy
echo "Зависимости установлены!"