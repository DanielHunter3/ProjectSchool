#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <numeric>
#include <cmath>

#include <boost/pool/pool.hpp>

using Duration = std::chrono::duration<double>;

constexpr size_t OBJECT_SIZE = 32;
constexpr size_t ALLOCATIONS = 1'000'000;
constexpr size_t WARMUP_ITERATIONS = ALLOCATIONS / 10;
constexpr size_t BENCHMARK_RUNS = 5;

struct TestObject {
  char data[OBJECT_SIZE];
};

constexpr size_t SIZE = sizeof(TestObject);

boost::pool<> global_pool(SIZE); // Теперь просто global pool вместо thread_local

void warmup_pool() noexcept {
  void* ptr = global_pool.malloc();
  global_pool.free(ptr);
}

void warmup_malloc() noexcept {
  void* ptr = malloc(SIZE);
  free(ptr);
}

template<typename TestFunc, typename WarmupFunc>
Duration measure(TestFunc&& test_func, WarmupFunc&& warmup) {
  for (size_t i = 0; i < WARMUP_ITERATIONS; ++i) {
    warmup();
  }

  auto start = std::chrono::high_resolution_clock::now();
  test_func();
  auto end = std::chrono::high_resolution_clock::now();
  return Duration(end - start);
}

void test_pool() {
  std::vector<TestObject*> objects;
  objects.reserve(ALLOCATIONS);

  for (size_t i = 0; i < ALLOCATIONS; ++i) {
    auto ptr = static_cast<TestObject*>(global_pool.malloc());
    memset(ptr, 0xAA, SIZE);
    objects.emplace_back(ptr);
  }

  for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    global_pool.free(*it);
  }
  global_pool.purge_memory();
}

void test_malloc() {
  std::vector<TestObject*> objects;
  objects.reserve(ALLOCATIONS);

  for (size_t i = 0; i < ALLOCATIONS; ++i) {
    auto ptr = static_cast<TestObject*>(malloc(SIZE));
    memset(ptr, 0xAA, SIZE);
    objects.emplace_back(ptr);
  }

  for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
    free(*it);
  }
}

template<typename TestFunc, typename WarmupFunc>
void run_benchmark(TestFunc&& test_func, const std::string& name, WarmupFunc&& warmup) {
  std::vector<double> timings(BENCHMARK_RUNS);

  for (size_t i = 0; i < BENCHMARK_RUNS; ++i) {
    timings[i] = measure(
      test_func,
      warmup
    ).count();
    
    std::cerr << "Run " << (i+1) << ": " << timings[i] << " sec\n";
  }

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