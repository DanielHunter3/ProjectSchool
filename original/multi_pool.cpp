#include <boost/pool/pool.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <cstring>

using Duration = std::chrono::duration<double>;

constexpr size_t OBJECT_SIZE = 256;
constexpr size_t ALLOCATIONS_PER_THREAD = 1'000'000;
constexpr size_t THREAD_COUNT = 8;

struct TestObject {
  char data[OBJECT_SIZE];
};

constexpr size_t SIZE = sizeof(TestObject);

// Thread-local пулы для каждого потока
thread_local boost::pool<> local_pool(SIZE);

void test_pool() {
  std::vector<TestObject*> objects;
  objects.reserve(ALLOCATIONS_PER_THREAD);
  
  // Аллокация
  for (size_t i = 0; i < ALLOCATIONS_PER_THREAD; ++i) {
    auto ptr = static_cast<TestObject*>(local_pool.malloc());
    memset(ptr, 0xAA, SIZE); // Заполнение памяти (антимёртвый код)
    objects.emplace_back(ptr);
  }
  
  // Освобождение (в обратном порядке для лучшей локалиности)
  for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    local_pool.free(*it);
  }
}

void test_new_delete() {
  std::vector<std::unique_ptr<TestObject>> objects;
  objects.reserve(ALLOCATIONS_PER_THREAD);
  
  // Аллокация
  for (size_t i = 0; i < ALLOCATIONS_PER_THREAD; ++i) {
    auto ptr = new TestObject;
    memset(ptr, 0xAA, SIZE); // Заполнение памяти (антимёртвый код)
    objects.emplace_back(ptr);
  }
  
  // Освобождение через unique_ptr (автоматически)
}

template<typename TestFunc>
void run_benchmark(TestFunc&& test_func, const std::string& name) {
  std::vector<std::thread> threads;
  threads.reserve(THREAD_COUNT);
  
  auto total_start = std::chrono::high_resolution_clock::now();
  
  for (int i = 0; i < THREAD_COUNT; ++i) {
    threads.emplace_back(test_func);
  }
  
  for (auto& t : threads) {
    t.join();
  }
  
  auto total_end = std::chrono::high_resolution_clock::now();
  Duration total_duration = total_end - total_start;
  
  std::cout << "TOTAL " << name << " time: " << total_duration.count() 
       << " sec \n";
}

int main() {
  // Тестируем оба варианта
  run_benchmark(test_pool, "boost::pool");
  run_benchmark(test_new_delete, "new/delete");
  
  return 0;
}