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
# Настройки
# =============================================
TEST_DIR="test"
BUILD_DIR="${TEST_DIR}/build"

# =============================================
# Сборка
# =============================================
echo -e "\n${INF}Конфигурация CMake...${NC}"
cd "${BUILD_DIR}" || exit 1

if ! cmake ../.. -DCMAKE_BUILD_TYPE=Debug; then #if ! cmake ../.. -DCMAKE_BUILD_TYPE=Release; then
  echo -e "${ERR}Ошибка конфигурации CMake${NC}" | tee -a "${LOG_FILE}"
  exit 1
fi

echo -e "\n${INF}Сборка проектов...${NC}"
if ! cmake --build . --target all_benchmarks -j"$(nproc)"; then
  echo -e "${ERR}Ошибка сборки${NC}" | tee -a "${LOG_FILE}"
  exit 1
fi

cd - > /dev/null || exit 1

echo -e "${SUC}✓ Сборка завершена успешно${NC}"