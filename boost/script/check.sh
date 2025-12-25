#!/bin/bash

# =============================================
# Настройки
# =============================================
TEST_DIR="test"
BUILD_DIR="${TEST_DIR}/build"

# Тестируемые программы и их описания
declare -A PROGRAMS=(
  ["single_pool_std"]="Singlethread POOL STD"
  ["single_pool_boost"]="Singlethread POOL BOOST"
  ["multi_pool_std"]="Multithread POOL STD" 
  ["multi_pool_boost"]="Multithread POOL BOOST" 

  ["single_vec_std"]="Singlethread VECTOR STD"
  ["single_vec_boost"]="Singlethread VECTOR BOOST"
  ["multi_vec_std"]="Multithread VECTOR STD"
  ["multi_vec_boost"]="Multithread VECTOR BOOST"

  ["single_map_std"]="Singlethread FAST-POOL-ALLOCATOR STD"
  ["single_map_boost"]="Singlethread FAST-POOL-ALLOCATOR BOOST"
  ["multi_map_std"]="Multithread FAST-POOL-ALLOCATOR STD"
  ["multi_map_boost"]="Multithread FAST-POOL-ALLOCATOR BOOST"
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