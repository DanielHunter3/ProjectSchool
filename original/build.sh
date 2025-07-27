#!/bin/bash

# =============================================
# Настройки
# =============================================
BUILD_DIR="test/build"
LOG_FILE="test/benchmark.log"
RUNS=5

# Тестируемые программы и их описания
declare -A PROGRAMS=(
  ["single_pool"]="Singlethread POOL"
  ["multi_pool"]="Multithread POOL" 
  ["single_vec"]="Singlethread VECTOR"
  ["multi_vec"]="Multithread VECTOR"
)

# =============================================
# Компиляция 
# =============================================
echo "Сборка проекта..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1
cmake ../.. || { echo "Ошибка CMake!"; exit 1; }
cmake --build . -- -j$(nproc) || { echo "Ошибка сборки!"; exit 1; }
cd - > /dev/null || exit 1

# =============================================
# Тестирование
# =============================================
echo "Начинаем тестирование..." | tee "$LOG_FILE"

run_test() {
  local program=$1
  local description=$2
  
  echo -e "\n$description" | tee -a "$LOG_FILE"
  
  "./$BUILD_DIR/$program" | while read -r line; do
    echo "$line" | tee -a "$LOG_FILE"
  done
  
  echo "------------------------" | tee -a "$LOG_FILE"
}

# Запускаем все тесты
for program in "${!PROGRAMS[@]}"; do
  run_test "$program" "${PROGRAMS[$program]}"
done

echo -e "\nТестирование завершено. Результаты в $LOG_FILE"