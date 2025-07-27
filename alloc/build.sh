#!/bin/bash

# =============================================
# Настройки
# =============================================
BUILD_DIR="test/build"
LOG_FILE="test/benchmark.log"

declare -A PROGRAMS=(
  ["std_st"]="[ST] System malloc"
  ["je_st"]="[ST] Jemalloc"
  ["tc_st"]="[ST] Tcmalloc"
  ["mi_st"]="[ST] Mimalloc"
  ["std_mt"]="[MT] System malloc"
  ["je_mt"]="[MT] Jemalloc"
  ["tc_mt"]="[MT] Tcmalloc"
  ["mi_mt"]="[MT] Mimalloc"
)

# =============================================
# Инициализация
# =============================================
echo "Подготовка окружения..."
mkdir -p "$BUILD_DIR"
> "$LOG_FILE"

# =============================================
# Компиляция
# =============================================
echo -e "\nСборка проектов..." | tee -a "$LOG_FILE"
if ! make -j$(nproc); then
  echo "ОШИБКА: Сборка не удалась!" | tee -a "$LOG_FILE"
  exit 1
fi

# =============================================
# Функция запуска теста
# =============================================
run_test() {
  local program=$1
  local description=$2
  
  echo -e "\n$description" | tee -a "$LOG_FILE"
  
  if [ ! -f "$BUILD_DIR/$program" ]; then
    echo "ОШИБКА: Файл $program не найден!" | tee -a "$LOG_FILE"
    echo "------------------------" | tee -a "$LOG_FILE"
    return 1
  fi
  
  "$BUILD_DIR/$program" | while read -r line; do
    echo "$line" | tee -a "$LOG_FILE"
  done
  
  echo "------------------------" | tee -a "$LOG_FILE"
}

# =============================================
# Основное тестирование
# =============================================
echo -e "\nЗапуск тестов..." | tee -a "$LOG_FILE"

for program in "${!PROGRAMS[@]}"; do
  run_test "$program" "${PROGRAMS[$program]}"
done

# =============================================
# Итоги
# =============================================
echo -e "\nТестирование завершено. Результаты в $LOG_FILE"