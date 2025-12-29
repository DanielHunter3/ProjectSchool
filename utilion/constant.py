if __name__ == "__main__":
    raise ImportError("Must be not __main__")

# =============================================
# Пути
# =============================================
def get_log_path(root: str) -> str:
    ROOT_DIR = root
    TEST_DIR = f"{ROOT_DIR}/test"
    LOG_DIR = f"{TEST_DIR}/logs"
    return LOG_DIR

# =============================================
# Составные части имени файлов
# =============================================
PRE_MULTI = "multi_"
PRE_SINGLE = "single_"

EXT = ".json"

# =============================================
# Части названий графиков
# =============================================
G_MULTI = "Multithread"
G_SINGLE = "Singlethread"