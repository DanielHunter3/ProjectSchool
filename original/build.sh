#!/bin/bash

# =============================================
# Настройки
# =============================================
BUILD_DIR="test/build"          # Папка для сборки
EXECUTABLES=(                  # Тестируемые программы
    "single_pool"
    "multi_pool"
    "single_vec"
    "multi_vec"
)
TEST_NAMES=(                   # Их читаемые названия
    "Singlethread pool"
    "Multithread pool"
    "Singlethread vector"
    "Multithread vector"
)
RUNS=5                         # Количество запусков каждого теста
LOG_FILE="test/benchmark.log"  # Файл для логов

# =============================================
# Функции
# =============================================

check_dependencies() {
    local deps=("cmake" "g++")
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            echo "Ошибка: $dep не установлен!" >&2
            exit 1
        fi
    done
}

run_test() {
    local test_name="$1"
    local executable="$BUILD_DIR/$2"
    
    # Первый тест начинается без пустой строки
    if [ "$3" == "first" ]; then
        echo -n "→ Тест: $test_name ($executable)" | tee "$LOG_FILE"
    else
        echo -e "\n→ Тест: $test_name ($executable)" | tee -a "$LOG_FILE"
    fi
    
    for ((i=1; i<=RUNS; i++)); do
        echo -e "\n  Попытка $i:" | tee -a "$LOG_FILE"
        { time "$executable"; } 2>&1 | tee -a "$LOG_FILE"
    done
}

cleanup() {
    if [ -d "$BUILD_DIR" ]; then
        echo "Удаляю старую папку сборки..."
        rm -rf "$BUILD_DIR"
    fi
    # Создаем пустой файл без лишних переносов
    : > "$LOG_FILE"
}

build_project() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1
    cmake ../.. || { echo "Ошибка CMake!" >&2; exit 1; }
    cmake --build . -- -j$(nproc) || { echo "Ошибка сборки!" >&2; exit 1; }
    cd - > /dev/null || exit 1
}

# =============================================
# Главный скрипт
# =============================================

# Инициализация
check_dependencies
cleanup
build_project

# Запуск тестов
echo "Начинаю тестирование..."
for idx in "${!EXECUTABLES[@]}"; do
    if [ $idx -eq 0 ]; then
        run_test "${TEST_NAMES[idx]}" "${EXECUTABLES[idx]}" "first"
    else
        run_test "${TEST_NAMES[idx]}" "${EXECUTABLES[idx]}"
    fi
done

echo -e "\nТестирование завершено. Результаты сохранены в $LOG_FILE"