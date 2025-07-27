#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <array>
#include <numeric>
#include <cmath>

#include <boost/pool/pool.hpp>

using Duration = std::chrono::duration<double>;

constexpr size_t OBJECT_SIZE = 512;
constexpr size_t ALLOCATIONS_PER_THREAD = 1'000'000;
constexpr size_t THREAD_COUNT = 8;
constexpr size_t WARMUP_ITERATIONS = ALLOCATIONS_PER_THREAD / 10;
constexpr size_t BENCHMARK_RUNS = 5;

struct TestObject {
  char data[OBJECT_SIZE];
};

constexpr size_t SIZE = sizeof(TestObject);

thread_local boost::pool<> local_pool(SIZE);

void warmup_pool() noexcept {
  void* ptr = local_pool.malloc();
  local_pool.free(ptr);
}

void warmup_malloc() noexcept {
  void* ptr = malloc(SIZE);
  free(ptr);
}

// Замер времени с прогревом
template<typename TestFunc, typename WarmupFunc>
Duration measure(TestFunc&& test_func, WarmupFunc&& warmup) {
  for (size_t i = 0; i < WARMUP_ITERATIONS; ++i) {
    warmup();
  }

  auto start = std::chrono::high_resolution_clock::now();
  test_func(); // Основной тест
  auto end = std::chrono::high_resolution_clock::now();
  return Duration(end - start);
}

void test_pool() {
  std::vector<TestObject*> objects;
  objects.reserve(ALLOCATIONS_PER_THREAD);

  for (size_t i = 0; i < ALLOCATIONS_PER_THREAD; ++i) {
    auto ptr = static_cast<TestObject*>(local_pool.malloc());
    memset(ptr, 0xAA, SIZE);
    objects.emplace_back(ptr);
  }

  for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    local_pool.free(*it);
  }
  local_pool.purge_memory();
}

void test_malloc() {
  std::vector<TestObject*> objects;
  objects.reserve(ALLOCATIONS_PER_THREAD);

  for (size_t i = 0; i < ALLOCATIONS_PER_THREAD; ++i) {
    auto ptr = static_cast<TestObject*>(malloc(SIZE));
    memset(ptr, 0xAA, SIZE);
    objects.emplace_back(ptr);
  }

  for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    free(*it);
  }
}

template<typename TestFunc, typename WarmupFunc>
void run_benchmark
(
  TestFunc&& test_func, 
  const std::string& name, 
  WarmupFunc&& warmup
) 
{
  std::vector<double> timings(BENCHMARK_RUNS);

  for (size_t i = 0; i < BENCHMARK_RUNS; ++i) {
    std::array<std::thread, THREAD_COUNT> threads;
    std::vector<Duration> thread_times(THREAD_COUNT);

    // Запускаем потоки и замеряем время для каждого
    for (size_t j = 0; j < THREAD_COUNT; ++j) {
      threads[j] = std::thread([&thread_times, &test_func, &warmup, j]() {
        thread_times[j] = measure(
          test_func,
          warmup
        );
      });
    }

    // Дожидаемся завершения и суммируем время (максимальное среди потоков)
    for (auto& t : threads) {
      t.join();
    }

    timings[i] = std::max_element(
      thread_times.begin(), thread_times.end()
    )->count();
    std::cerr << "Time " << (i+1) << ": " << timings[i] << '\n';
  }

  // Статистика
  double avg_time = std::accumulate(timings.begin(), timings.end(), 0.0) / BENCHMARK_RUNS;
  double stddev = 0.0;
  for (double t : timings) {
    stddev += (t - avg_time) * (t - avg_time);
  }
  stddev = std::sqrt(stddev / BENCHMARK_RUNS);

  std::cout << "[" << name << "] "
       << "Average: " << avg_time << " sec, "
       << "StdDev: ±" << stddev << "\n";
}

int main() {
  run_benchmark(test_pool, "boost::pool", warmup_pool);
  run_benchmark(test_malloc, "malloc/free", warmup_malloc);
  return 0;
}