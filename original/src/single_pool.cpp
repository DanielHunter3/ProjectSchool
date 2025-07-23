#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <cstring>

#include <boost/pool/pool.hpp>

using Duration = std::chrono::duration<double>;

constexpr size_t OBJECT_SIZE = 256;
constexpr size_t ALLOCATIONS = 8'000'000; // 8 потоков × 1M операций

struct TestObject { char data[OBJECT_SIZE]; };

constexpr size_t SIZE = sizeof(TestObject);

void test_pool() {
  boost::pool<> pool(SIZE);

  std::vector<TestObject*> objects;
  objects.reserve(ALLOCATIONS);

  // Аллокация
  for (size_t i = 0; i < ALLOCATIONS; ++i) {
    auto ptr = static_cast<TestObject*>(pool.malloc());
    memset(ptr, 0xAA, OBJECT_SIZE); // Заполнение памяти (антимёртвый код)
    objects.push_back(ptr);
  }

  // Освобождение
  for (auto obj : objects) {
    pool.free(obj);
  }
}

void test_new_delete() {
  std::vector<std::unique_ptr<TestObject>> objects;
  objects.reserve(ALLOCATIONS);

  // Аллокация
  for (size_t i = 0; i < ALLOCATIONS; ++i) {
    auto ptr = new TestObject;
    memset(ptr, 0xAA, OBJECT_SIZE); // Заполнение памяти (антимёртвый код)
    objects.emplace_back(new TestObject);
  }

  // Освобождение (автоматически через unique_ptr)
}

template<typename TestFunc>
void run_benchmark(TestFunc&& test_func, const std::string& name) {
  auto total_start = std::chrono::high_resolution_clock::now();
  
  test_func();
  
  auto total_end = std::chrono::high_resolution_clock::now();
  Duration total_duration = total_end - total_start;
  
  std::cout << "TOTAL " << name << " time: " << total_duration.count() 
       << " sec \n";
}

int main() {
  run_benchmark(test_pool, "boost::pool");
  run_benchmark(test_new_delete, "new/delete");

  return 0;
}