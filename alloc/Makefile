# Компилятор и флаги
CXX = g++
CXXFLAGS = -O3 -pthread -march=native
SRC_DIR = src
BUILD_DIR = test/build

# Исходные файлы
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
EXECUTABLES = je_mt je_st tc_mt tc_st mi_mt mi_st std_mt std_st

# Правило по умолчанию: собираем всё
all: $(EXECUTABLES)

# Создаем папку build, если её нет
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Jemalloc
je_mt: $(SRC_DIR)/alloc_multithread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_JEMALLOC $< -ljemalloc -o $(BUILD_DIR)/$@

je_st: $(SRC_DIR)/alloc_singlethread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_JEMALLOC $< -ljemalloc -o $(BUILD_DIR)/$@

# Tcmalloc
tc_mt: $(SRC_DIR)/alloc_multithread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_TCMALLOC $< -ltcmalloc -o $(BUILD_DIR)/$@

tc_st: $(SRC_DIR)/alloc_singlethread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_TCMALLOC $< -ltcmalloc -o $(BUILD_DIR)/$@

# Mimalloc
mi_mt: $(SRC_DIR)/alloc_multithread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_MIMALLOC $< -lmimalloc -o $(BUILD_DIR)/$@

mi_st: $(SRC_DIR)/alloc_singlethread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_MIMALLOC $< -lmimalloc -o $(BUILD_DIR)/$@

# Стандартный malloc (без флагов)
std_mt: $(SRC_DIR)/alloc_multithread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $(BUILD_DIR)/$@

std_st: $(SRC_DIR)/alloc_singlethread.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $(BUILD_DIR)/$@

# Очистка (удаляем только содержимое build, но не саму папку)
clean:
	rm -f $(BUILD_DIR)/*

# Защита от интерпретации как файлов
.PHONY: all clean