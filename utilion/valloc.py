from visual import *
from constant import *

if __name__ == "__main__":
    raise ImportError("Must be not __main__")

# =============================================
# Пути
# =============================================
LOG_DIR = get_log_path("alloc")

# =============================================
# Составные части имени файлов
# =============================================
JEMALLOC = "jemalloc"
TCMALLOC = "tcmalloc"
MIMALLOC = "mimalloc"
SYS_MALLOC = "system"

# =============================================
# Имена файлов с путями
# =============================================
MULTI_JEMALLOC_PATH = f"{LOG_DIR}/{PRE_MULTI}{JEMALLOC}{EXT}"
MULTI_TCMALLOC_PATH = f"{LOG_DIR}/{PRE_MULTI}{TCMALLOC}{EXT}"
MULTI_MIMALLOC_PATH = f"{LOG_DIR}/{PRE_MULTI}{MIMALLOC}{EXT}"
MULTI_SYS_MALLOC_PATH = f"{LOG_DIR}/{PRE_MULTI}{SYS_MALLOC}{EXT}"

SINGLE_JEMALLOC_PATH = f"{LOG_DIR}/{PRE_SINGLE}{JEMALLOC}{EXT}"
SINGLE_TCMALLOC_PATH = f"{LOG_DIR}/{PRE_SINGLE}{TCMALLOC}{EXT}"
SINGLE_MIMALLOC_PATH = f"{LOG_DIR}/{PRE_SINGLE}{MIMALLOC}{EXT}"
SINGLE_SYS_MALLOC_PATH = f"{LOG_DIR}/{PRE_SINGLE}{SYS_MALLOC}{EXT}"

# =============================================
# Части названий графиков
# =============================================
G_JEMALLOC = "JEMALLOC"
G_TCMALLOC = "TCMALLOC"
G_MIMALLOC = "MIMALLOC"
G_SYS_MALLOC = "SYSTEM MALLOC"

# =============================================
# Названия графиков
# =============================================
G_MULTI_JEMALLOC = f"{G_MULTI} {G_JEMALLOC}"
G_MULTI_TCMALLOC = f"{G_MULTI} {G_TCMALLOC}"
G_MULTI_MIMALLOC = f"{G_MULTI} {G_MIMALLOC}"
G_MULTI_SYS_MALLOC = f"{G_MULTI} {G_SYS_MALLOC}"

G_SINGLE_JEMALLOC = f"{G_SINGLE} {G_JEMALLOC}"
G_SINGLE_TCMALLOC = f"{G_SINGLE} {G_TCMALLOC}"
G_SINGLE_MIMALLOC = f"{G_SINGLE} {G_MIMALLOC}"
G_SINGLE_SYS_MALLOC = f"{G_SINGLE} {G_SYS_MALLOC}"

# =============================================
# Трансляторы
# =============================================
MULTI_TRANSLATE = {
    MULTI_JEMALLOC_PATH : G_MULTI_JEMALLOC,
    MULTI_TCMALLOC_PATH : G_MULTI_TCMALLOC,
    MULTI_MIMALLOC_PATH : G_MULTI_MIMALLOC,
    MULTI_SYS_MALLOC_PATH : G_MULTI_SYS_MALLOC,
}

SINGLE_TRANSLATE = {
    SINGLE_JEMALLOC_PATH : G_SINGLE_JEMALLOC,
    SINGLE_TCMALLOC_PATH : G_SINGLE_TCMALLOC,
    SINGLE_MIMALLOC_PATH : G_SINGLE_MIMALLOC,
    SINGLE_SYS_MALLOC_PATH : G_SINGLE_SYS_MALLOC,
}

# =============================================
# Класс VISUALIZER
# =============================================
def create_visualizer() -> BenchmarkVisualizer:
    """Создает визуализатор для тестов аллокаторов"""
    
    # Конфигурации графиков
    multi_config = GraphConfig(
        title="Multithread Performance Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )
    
    single_config = GraphConfig(
        title="Singlethread Performance Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )
    
    # Конфигурации визуализатора
    configs = [
        (MULTI_TRANSLATE, multi_config),
        (SINGLE_TRANSLATE, single_config)
    ]
    
    return BenchmarkVisualizer(configs)