from visual import *
from constant import *

if __name__ == "__main__":
    raise ImportError("Must be not __main__")

# =============================================
# Пути
# =============================================
ROOT_DIR = get_log_path("boost")

# =============================================
# Составные части имени файлов
# =============================================
POST_BOOST = "_boost"
POST_STD = "_std"

MAP = "map"
POOL = "pool"
VEC = "vec"

# =============================================
# Имена файлов с путями
# =============================================
MULTI_MAP_BOOST_PATH = f"{ROOT_DIR}/{PRE_MULTI}{MAP}{POST_BOOST}{EXT}"
MULTI_MAP_STD_PATH = f"{ROOT_DIR}/{PRE_MULTI}{MAP}{POST_STD}{EXT}"

MULTI_POOL_BOOST_PATH = f"{ROOT_DIR}/{PRE_MULTI}{POOL}{POST_BOOST}{EXT}"
MULTI_POOL_STD_PATH = f"{ROOT_DIR}/{PRE_MULTI}{POOL}{POST_STD}{EXT}"

MULTI_VEC_BOOST_PATH = f"{ROOT_DIR}/{PRE_MULTI}{VEC}{POST_BOOST}{EXT}"
MULTI_VEC_STD_PATH = f"{ROOT_DIR}/{PRE_MULTI}{VEC}{POST_STD}{EXT}"

SINGLE_MAP_BOOST_PATH = f"{ROOT_DIR}/{PRE_SINGLE}{MAP}{POST_BOOST}{EXT}"
SINGLE_MAP_STD_PATH = f"{ROOT_DIR}/{PRE_SINGLE}{MAP}{POST_STD}{EXT}"

SINGLE_POOL_BOOST_PATH = f"{ROOT_DIR}/{PRE_SINGLE}{POOL}{POST_BOOST}{EXT}"
SINGLE_POOL_STD_PATH = f"{ROOT_DIR}/{PRE_SINGLE}{POOL}{POST_STD}{EXT}"

SINGLE_VEC_BOOST_PATH = f"{ROOT_DIR}/{PRE_SINGLE}{VEC}{POST_BOOST}{EXT}"
SINGLE_VEC_STD_PATH = f"{ROOT_DIR}/{PRE_SINGLE}{VEC}{POST_STD}{EXT}"

# =============================================
# Части названий графиков
# =============================================
G_MAP = "MAP"
G_POOL = "POOL"
G_VEC = "VEC"

G_BOOST = "BOOST"
G_STD = "STD"

# =============================================
# Названия графиков
# =============================================
G_MULTI_MAP_BOOST = f"{G_MULTI} {G_MAP} {G_BOOST}"
G_MULTI_MAP_STD = f"{G_MULTI} {G_MAP} {G_STD}"

G_MULTI_POOL_BOOST = f"{G_MULTI} {G_POOL} {G_BOOST}"
G_MULTI_POOL_STD = f"{G_MULTI} {G_POOL} {G_STD}"

G_MULTI_VEC_BOOST = f"{G_MULTI} {G_VEC} {G_BOOST}"
G_MULTI_VEC_STD = f"{G_MULTI} {G_VEC} {G_STD}"

G_SINGLE_MAP_BOOST = f"{G_SINGLE} {G_MAP} {G_BOOST}"
G_SINGLE_MAP_STD = f"{G_SINGLE} {G_MAP} {G_STD}"

G_SINGLE_POOL_BOOST = f"{G_SINGLE} {G_POOL} {G_BOOST}"
G_SINGLE_POOL_STD = f"{G_SINGLE} {G_POOL} {G_STD}"

G_SINGLE_VEC_BOOST = f"{G_SINGLE} {G_VEC} {G_BOOST}"
G_SINGLE_VEC_STD = f"{G_SINGLE} {G_VEC} {G_STD}"

# =============================================
# Трансляторы
# =============================================
MULTI_MAP_TRANSLATE = {
    MULTI_MAP_BOOST_PATH : G_MULTI_MAP_BOOST,
    MULTI_MAP_STD_PATH : G_MULTI_MAP_STD,
}

MULTI_POOL_TRANSLATE = {
    MULTI_POOL_BOOST_PATH : G_MULTI_POOL_BOOST,
    MULTI_POOL_STD_PATH : G_MULTI_POOL_STD,
}

MULTI_VEC_TRANSLATE = {
    MULTI_VEC_BOOST_PATH : G_MULTI_VEC_BOOST,
    MULTI_VEC_STD_PATH : G_MULTI_VEC_STD,
}

SINGLE_MAP_TRANSLATE = {
    SINGLE_MAP_BOOST_PATH : G_SINGLE_MAP_BOOST,
    SINGLE_MAP_STD_PATH : G_SINGLE_MAP_STD,
}

SINGLE_POOL_TRANSLATE = {
    SINGLE_POOL_BOOST_PATH : G_SINGLE_POOL_BOOST,
    SINGLE_POOL_STD_PATH : G_SINGLE_POOL_STD,
}

SINGLE_VEC_TRANSLATE = {
    SINGLE_VEC_BOOST_PATH : G_SINGLE_VEC_BOOST,
    SINGLE_VEC_STD_PATH : G_SINGLE_VEC_STD,
}

# =============================================
# Класс VISUALIZER
# =============================================
def create_visualizer() -> BenchmarkVisualizer:
    """Создает визуализатор для тестов boost vs std"""

    multi_map_congig = GraphConfig(
        title="Multithread Map Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )

    multi_pool_congig = GraphConfig(
        title="Multithread Pool Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )

    multi_vec_congig = GraphConfig(
        title="Multithread Vector Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )

    single_map_congig = GraphConfig(
        title="Singlethread Map Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )

    single_pool_congig = GraphConfig(
        title="Singlethread Pool Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )

    single_vec_congig = GraphConfig(
        title="Singlethread Vector Benchmark",
        x_label="Test Iteration",
        y_label="Time (seconds)"
    )

    configs = (
        (MULTI_MAP_TRANSLATE, multi_map_congig),
        (MULTI_POOL_TRANSLATE, multi_pool_congig),
        (MULTI_VEC_TRANSLATE, multi_vec_congig),
        (SINGLE_MAP_TRANSLATE, single_map_congig),
        (SINGLE_POOL_TRANSLATE, single_pool_congig),
        (SINGLE_VEC_TRANSLATE, single_vec_congig),
    )

    return BenchmarkVisualizer(configs)