#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <boost/container/small_vector.hpp>

// Конфигурация теста
constexpr size_t ITERATIONS = 1'000'000'000; // Операций на поток
constexpr size_t SMALL_SIZE = 64;     // Размер буфера
constexpr size_t THREADS = 8;     // Количество потоков

template<typename VectorType>
void thread_task() { // Параметр id не используется, можно удалить
  VectorType vec;
  
  // Оптимизация только для std::vector
  if constexpr (std::is_same_v<VectorType, std::vector<int>>) {
    vec.reserve(SMALL_SIZE); // Предвыделение памяти
  }
  
  // Основной рабочий цикл
  for (size_t i = 0; i < ITERATIONS; ++i) {
    vec.emplace_back(i);   // Эффективная вставка
    
    // Циклический буфер
    if (vec.size() >= SMALL_SIZE) { // >= для надёжности
      vec.clear();
    }
  }
}

template<typename VectorType>
void run_benchmark(const std::string& name) {
  auto start = std::chrono::high_resolution_clock::now();
  
  std::vector<std::thread> threads;
  threads.reserve(THREADS); // Оптимизация выделения памяти

  // Запуск потоков
  for (size_t i = 0; i < THREADS; ++i) {
    threads.emplace_back(thread_task<VectorType>);
  }
  
  // Ожидание завершения
  for (auto& t : threads) {
    t.join();
  }
  
  // Расчёт и вывод результатов
  auto duration = std::chrono::duration<double>(
    std::chrono::high_resolution_clock::now() - start
  ).count();
  
  std::cout << name << ": "
       << duration << " sec \n";
}

int main() {
  run_benchmark<std::vector<int>>("std::vector");
  run_benchmark<boost::container::small_vector<int, SMALL_SIZE>>("small_vector");
  
  return 0;
}