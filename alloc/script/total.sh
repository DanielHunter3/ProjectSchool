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
# Утилиты
# =============================================
hypertest() {
  local program=$1
  local description=$2
  
  echo -e "\n${INF}▶ Тестирование: ${description}${NC}" | tee -a "${LOG_FILE}"
  echo -e "Программа: ${program}" | tee -a "${LOG_FILE}"
  
  # Извлекаем только имя файла из пути программы
  local program_name=$(basename "${program}")
  
  # Запуск hyperfine с обработкой ошибок
  if hyperfine "${program}" --warmup "${WARMUP}" --runs "${RUNS}" --export-json "${LOG_DIR}/${program_name}.json" 2>&1 | tee -a "${LOG_FILE}"; then
    echo -e "${SUC}✓ Успешно${NC}" | tee -a "${LOG_FILE}"
    return 0
  else
    echo -e "${ERR}✗ Ошибка${NC}" | tee -a "${LOG_FILE}"
    return 1
  fi
  
  echo "------------------------" | tee -a "${LOG_FILE}"
}

# =============================================
# Настройки
# =============================================
TEST_DIR="test"
BUILD_DIR="${TEST_DIR}/build"
LOG_DIR="${TEST_DIR}/logs"
LOG_FILE="${LOG_DIR}/benchmark.log"
RUNS=5
WARMUP=5

# Тестируемые программы и их описания
declare -A PROGRAMS=(
  ["single_system"]="Singlethread SYSTEM MALLOC"
  ["single_jemalloc"]="Singlethread JEMALLOC"
  ["single_tcmalloc"]="Singlethread TCMALLOC"
  ["single_mimalloc"]="Singlethread MIMALLOC"

  ["single_system"]="Multithread SYSTEM MALLOC"
  ["multi_jemalloc"]="Multithread JEMALLOC"
  ["multi_tcmalloc"]="Multithread TCMALLOC"
  ["multi_mimalloc"]="Multithread MIMALLOC"
)

# =============================================
# Основное тестирование
# =============================================
echo -e "\n${INF}Начинаем тестирование...${NC}"
echo "Начинаем тестирование..." >> "${LOG_FILE}"
echo "Количество прогонов: ${RUNS}" >> "${LOG_FILE}"
echo "Прогрев: ${WARMUP} итераций" >> "${LOG_FILE}"
echo "Дата: $(date)" >> "${LOG_FILE}"

FAILED_TESTS=0
for program in "${!PROGRAMS[@]}"; do
  if ! hypertest "./${BUILD_DIR}/${program}" "${PROGRAMS[$program]}"; then
    ((FAILED_TESTS++))
  fi
done

# =============================================
# Вывод итогов
# =============================================
echo -e "\n${INF}========== ИТОГИ ==========${NC}"
echo -e "Всего тестов: ${#PROGRAMS[@]}"
echo -e "Успешно: $(( ${#PROGRAMS[@]} - FAILED_TESTS ))"
echo -e "Неудачно: ${FAILED_TESTS}"

if [[ ${FAILED_TESTS} -eq 0 ]]; then
  echo -e "${SUC}✓ Все тесты выполнены успешно${NC}"
else
  echo -e "${WARN}⚠ Некоторые тесты завершились с ошибками${NC}"
fi

echo -e "\n${INF}Результаты:${NC}"
echo -e "  Лог: ${LOG_FILE}"
echo -e "  JSON результаты: ${LOG_DIR}/*.json"

echo -e "\n${SUC}Тестирование завершено.${NC}"

# Возвращаем код выхода
exit ${FAILED_TESTS}