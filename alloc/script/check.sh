#!/bin/bash

# =============================================
# Настройки
# =============================================
TEST_DIR="alloc/test"
BUILD_DIR="${TEST_DIR}/build"

# Тестируемые программы и их описания
declare -A PROGRAMS=(
  ["single_system"]="Singlethread SYSTEM MALLOC"
  ["single_jemalloc"]="Singlethread JEMALLOC"
  ["single_tcmalloc"]="Singlethread TCMALLOC"
  ["single_mimalloc"]="Singlethread MIMALLOC"

  ["multi_system"]="Multithread SYSTEM MALLOC"
  ["multi_jemalloc"]="Multithread JEMALLOC"
  ["multi_tcmalloc"]="Multithread TCMALLOC"
  ["multi_mimalloc"]="Multithread MIMALLOC"
)

# =============================================
# Проверка существования программ
# =============================================
echo -e "\n${INF}Проверка доступности программ...${NC}"
MISSING_PROGRAMS=0
for program in "${!PROGRAMS[@]}"; do
  if [[ ! -f "${BUILD_DIR}/${program}" ]]; then
    echo -e "${WARN}⚠ Программа ${program} не найдена${NC}"
    ((MISSING_PROGRAMS++))
  fi
done

if [[ ${MISSING_PROGRAMS} -gt 0 ]]; then
  echo -e "${ERR}Найдено ${MISSING_PROGRAMS} отсутствующих программ${NC}"
  exit 1
fi

echo -e "${SUC}✓ Все программы доступны${NC}"