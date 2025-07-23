#!/bin/bash

# =============================================
# Настройки
# =============================================
BUILD_DIR="test/build"    # Папка для сборки
LOG_FILE="test/benchmark.log" # Файл для результатов
SRC_DIR="src"        # Папка с исходниками
RUNS=5           # Количество запусков

# Создаем необходимые директории
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR" || exit 1

# =============================================
# Компиляция
# =============================================

# Jemalloc
g++ -O3 -pthread -DUSE_JEMALLOC "../../$SRC_DIR/alloc_multithread.cpp" -ljemalloc -o je_mt
g++ -O3 -DUSE_JEMALLOC "../../$SRC_DIR/alloc_singlethread.cpp" -ljemalloc -o je_st

# Tcmalloc
g++ -O3 -pthread -DUSE_TCMALLOC "../../$SRC_DIR/alloc_multithread.cpp" -ltcmalloc -o tc_mt
g++ -O3 -DUSE_TCMALLOC "../../$SRC_DIR/alloc_singlethread.cpp" -ltcmalloc -o tc_st

# Mimalloc
g++ -O3 -pthread -DUSE_MIMALLOC "../../$SRC_DIR/alloc_multithread.cpp" -lmimalloc -o mi_mt
g++ -O3 -DUSE_MIMALLOC "../../$SRC_DIR/alloc_singlethread.cpp" -lmimalloc -o mi_st

# Стандартный malloc
g++ -O3 -pthread "../../$SRC_DIR/alloc_multithread.cpp" -o std_mt
g++ -O3 "../../$SRC_DIR/alloc_singlethread.cpp" -o std_st

# =============================================
# Тестирование
# =============================================

# Очищаем предыдущий лог
: > "../../$LOG_FILE"

run_test() {
  local prefix=$1
  local test_name=$2
  
  echo -e "\n=== $test_name ===" | tee -a "../../$LOG_FILE"
  
  for i in $(seq 1 $RUNS); do
    echo -e "\nПопытка $i:" | tee -a "../../$LOG_FILE"
    
    if [ -f "./${prefix}_st" ]; then
      echo -n "Singlethread: " | tee -a "../../$LOG_FILE"
      { time "./${prefix}_st"; } 2>&1 | grep real | awk '{print " " $2}' | tee -a "../../$LOG_FILE"
    fi
    
    if [ -f "./${prefix}_mt" ]; then
      echo -n "Multithread: " | tee -a "../../$LOG_FILE"
      { time "./${prefix}_mt"; } 2>&1 | grep real | awk '{print " " $2}' | tee -a "../../$LOG_FILE"
    fi
    
    echo "------------------------" | tee -a "../../$LOG_FILE"
  done
}

echo "Начинаем тестирование..." | tee -a "../../$LOG_FILE"

# Тестируем все варианты
run_test "std" "Стандартный malloc"
run_test "je" "Jemalloc"
run_test "tc" "TCMalloc"
run_test "mi" "Mimalloc"

# =============================================
# Завершение
# =============================================
cd - > /dev/null
echo -e "\nТестирование завершено. Результаты в $LOG_FILE"