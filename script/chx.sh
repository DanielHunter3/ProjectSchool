#!/bin/bash
# Использует самый эффективный метод с параллельной обработкой

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Используем find с -exec + для максимальной эффективности
# -exec + передает ВСЕ файлы в ОДИН вызов команды
find "$PROJECT_ROOT" \
    -type f \
    -name "*.sh" \
    ! -executable \
    ! -path "$PROJECT_ROOT/script/chx.sh" \
    ! -path "*/test/*" \
    ! -path "*/logs/*" \
    ! -path "*/.git/*" \
    ! -path "*/build/*" \
    -exec chmod +x {} + \
    2>/dev/null && \
    echo "✅ All non-executable .sh files now have +x permission"

# Это самый быстрый метод, потому что:
# 1. find сам отфильтровывает уже исполняемые файлы
# 2. -exec + передает максимальное количество файлов за вызов
# 3. Минимальное количество вызовов chmod