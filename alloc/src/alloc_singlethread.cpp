#include <vector>
#include <cstring>
#include <random>
#include <algorithm>
#include <iostream>

// Конфигурация (убрали THREADS, оставили только общее количество операций)
constexpr size_t TOTAL_OPERATIONS = 4'000'000;  // THREADS * OPERATIONS_PER_THREAD
constexpr size_t MIN_ALLOC_SIZE = 512;
constexpr size_t MAX_ALLOC_SIZE = 8192;
constexpr size_t MAX_LIVE_OBJECTS = 5000;
constexpr uint64_t SEED = 0x123456789ABCDEF0ULL;

// Глобальный предвычисленный массив размеров
struct GlobalData {
    std::vector<size_t> sizes;          // размеры для аллокаций
    std::vector<uint8_t> alloc_decisions; // аллоцировать или освобождать?
    std::vector<size_t> free_indices;   // индексы для освобождения
    
    GlobalData(size_t total_ops) {
        // 1. Подготовить ОДИН генератор
        std::mt19937_64 gen(SEED);
        
        // 2. Логнормальное распределение размеров (как в реальности)
        std::lognormal_distribution<double> size_log_dist(
            std::log(256),  // медиана ~256 байт
            1.0             // разброс
        );
        
        sizes.reserve(total_ops);
        for (size_t i = 0; i < total_ops; ++i) {
            double log_size = size_log_dist(gen);
            size_t size = static_cast<size_t>(std::exp(log_size));
            // Ограничить и выровнять
            size = std::clamp(size, MIN_ALLOC_SIZE, MAX_ALLOC_SIZE);
            size = (size + 15) & ~15;  // Выравнивание на 16
            sizes.push_back(size);
        }
        
        // 3. Решения об аллокации/освобождении (детерминированные)
        alloc_decisions.reserve(total_ops);
        std::uniform_int_distribution<int> op_dist(0, 99);
        for (size_t i = 0; i < total_ops; ++i) {
            alloc_decisions.push_back(op_dist(gen) < 60 ? 1 : 0);
        }
        
        // 4. Предвычисленные индексы для освобождения
        free_indices.reserve(total_ops / 2);  // Примерно половина - освобождения
        std::uniform_int_distribution<size_t> idx_dist(0, MAX_LIVE_OBJECTS - 1);
        for (size_t i = 0; i < total_ops / 2; ++i) {
            free_indices.push_back(idx_dist(gen) % MAX_LIVE_OBJECTS);
        }
    }
};

// Глобальные данные
static GlobalData* global_data = nullptr;

// Оптимизированная функция использования памяти
inline void use_memory_optimized(void* ptr, size_t size) noexcept {
    char* cptr = static_cast<char*>(ptr);
    size_t processed = 0;
    
    // Обрабатываем по 64 байта
    while (processed + 64 <= size) {
        // Принудительная работа с памятью
        for (int i = 0; i < 64; i += 8) {
            uint64_t word;
            std::memcpy(&word, cptr + processed + i, sizeof(uint64_t));
            word ^= 0x9e3779b97f4a7c15ULL;
            std::memcpy(cptr + processed + i, &word, sizeof(uint64_t));
        }
        processed += 64;
    }
    
    // Остаток
    if (processed < size) {
        for (size_t i = processed; i < size; ++i) {
            cptr[i] ^= 0x55;
        }
    }
}

void run_single_thread() {
    // Локальное состояние (раньше было ThreadState)
    std::vector<std::pair<void*, size_t>> allocations;
    uint64_t local_hash = 0;
    size_t size_index = 0;
    size_t free_index = 0;
    
    allocations.reserve(MAX_LIVE_OBJECTS);
    
    // Выполняем все операции в одном потоке
    for (size_t i = 0; i < TOTAL_OPERATIONS; ++i) {
        size_t global_op_idx = i % global_data->sizes.size();
        size_t global_size_idx = (i * 7) % global_data->sizes.size();
        
        if (allocations.size() < 10 || 
            (allocations.size() < MAX_LIVE_OBJECTS && 
             global_data->alloc_decisions[global_op_idx])) {
            
            // АЛЛОКАЦИЯ
            size_t size = global_data->sizes[global_size_idx];
            void* ptr = malloc(size);
            if (!ptr) throw std::bad_alloc();
            
            // Инициализация
            std::memset(ptr, (i & 0xFF), std::min(size, 64ul));
            
            // Использование памяти (симуляция реальной работы)
            use_memory_optimized(ptr, size);
            
            // Обновляем хэш для предотвращения оптимизаций
            char* cptr = static_cast<char*>(ptr);
            uint64_t word;
            std::memcpy(&word, cptr, sizeof(uint64_t));
            local_hash ^= word * 0x9e3779b97f4a7c15ULL;
            
            allocations.emplace_back(ptr, size);
            size_index = (size_index + 1) % global_data->sizes.size();
            
        } else {
            // ОСВОБОЖДЕНИЕ
            size_t free_idx = global_data->free_indices[free_index] % 
                              allocations.size();
            free_index = (free_index + 1) % global_data->free_indices.size();
            
            auto& [ptr, size] = allocations[free_idx];
            
            // Использование перед освобождением
            use_memory_optimized(ptr, size);
            
            // Хэширование перед освобождением
            char* cptr = static_cast<char*>(ptr);
            uint64_t word;
            std::memcpy(&word, cptr, sizeof(uint64_t));
            local_hash ^= word * 0x9e3779b97f4a7c15ULL;
            
            free(ptr);
            
            // Быстрое удаление: заменяем удаляемый на последний
            if (free_idx != allocations.size() - 1) {
                allocations[free_idx] = allocations.back();
            }
            allocations.pop_back();
        }
    }
    
    // Освобождение оставшихся объектов
    for (auto& [ptr, size] : allocations) {
        use_memory_optimized(ptr, size);
        free(ptr);
    }
    allocations.clear();
    
    // Предотвращаем оптимизацию
    asm volatile("" : "+r"(local_hash));
    std::cout << "Final hash: " << local_hash << std::endl;
}

int main() {
    // Инициализация глобальных данных
    global_data = new GlobalData(TOTAL_OPERATIONS * 2);  // x2 для запаса
    
    // Запускаем в одном потоке
    run_single_thread();
    
    delete global_data;
    
    return 0;
}