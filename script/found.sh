#!/bin/bash

# =============================================
# Цвета
# =============================================
ERR='\033[0;31m'
SUC='\033[0;32m'
WARN='\033[0;33m'
INF='\033[36m'
NC='\033[0m' # No Color - без цвета

# =============================================
# Проверка наличия программ
# =============================================
PROGRAMS=("cmake" "hyperfine" "python3" "pip3")

for program in "${PROGRAMS[@]}"; do
    if command -v "$program" &> /dev/null; then
        echo -e "${SUC}✓ $program доступен"
    else
        echo -e "${ERR}✗ $program не найден"
    fi
done