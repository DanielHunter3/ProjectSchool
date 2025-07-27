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
  local program="$1"
  local description="$2"
  local log_file="$3"
  
  echo -e "\n$description" | tee -a "$log_file"
  
  if [ ! -f "$BUILD_DIR/$program" ]; then
    echo "ОШИБКА: Файл $program не найден!" | tee -a "$log_file"
    echo "---------------------" | tee -a "$log_file"
    return 1
  fi
  
  if [ $status -eq 124 ]; then
    echo "ПРЕРВАНО: Тест превысил время выполнения" | tee -a "$log_file"
  elif [ $status -ne 0 ]; then
    echo "ОШИБКА: Тест завершился с кодом $status" | tee -a "$log_file"
  fi
  
  echo "---------------------" | tee -a "$log_file"
  return $status
}

# =============================================
# Основное тестирование
# =============================================
echo -e "\nЗапуск тестов..." | tee -a "$LOG_FILE"

for program in "${!PROGRAMS[@]}"; do
  run_test "$program" "${PROGRAMS[$program]}" "$LOG_FILE"
done

# =============================================
# Итоги
# =============================================
echo -e "\nРезультаты тестирования:" | tee -a "$LOG_FILE"
grep -E "(\[ST\]|\[MT\])|(Malloc|Jemalloc|Tcmalloc|Mimalloc)" "$LOG_FILE" | tee -a "$LOG_FILE"

echo -e "\nТестирование завершено. Полные результаты в $LOG_FILE"