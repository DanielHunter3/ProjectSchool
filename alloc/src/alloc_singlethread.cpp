#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <random>
#include <numeric>
#include <memory>

using Duration = std::chrono::duration<double>;

// Конфигурация
constexpr size_t TOTAL_OPERATIONS = 4'000'000; // 8 потоков * 500'000 операций
constexpr size_t MIN_ALLOC_SIZE = 256;
constexpr size_t MAX_ALLOC_SIZE = 8192;
constexpr size_t BENCHMARK_RUNS = 5;

// Глобальные генераторы
namespace {
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<size_t> size_dist(
    MIN_ALLOC_SIZE, MAX_ALLOC_SIZE
  );
}

double run_bench() {
  auto start = std::chrono::high_resolution_clock::now();
  size_t checksum = 0;
  std::vector<std::unique_ptr<void, void(*)(void*)>> ptrs;
  ptrs.reserve(TOTAL_OPERATIONS);

  for (size_t i = 0; i < TOTAL_OPERATIONS; ++i) {
    size_t size = size_dist(gen);
    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc();
    
    memset(ptr, 0xAA, size);
    checksum += *(static_cast<unsigned char*>(ptr));
    ptrs.emplace_back(ptr, free);
  }

  // Гарантируем, что checksum не будет оптимизирован
  asm volatile("" : "+r"(checksum));
  
  return Duration(std::chrono::high_resolution_clock::now() - start).count();
}

int main() {
  std::vector<double> timings;
  timings.reserve(BENCHMARK_RUNS);

  for (size_t i = 0; i < BENCHMARK_RUNS; ++i) {
    timings.push_back(run_bench());
    std::cerr << "Run " << i+1 << ": " << timings.back() << " sec\n";
  }

  double avg = std::accumulate(timings.begin(), timings.end(), 0.0) / BENCHMARK_RUNS;
  double stddev = 0.0;
  for (double t : timings) {
    stddev += (t - avg) * (t - avg);
  }
  stddev = std::sqrt(stddev / BENCHMARK_RUNS);

  std::cout << "Average: " << avg << " sec\n"
       << "StdDev: ±" << stddev << "\n"
       << "Throughput: " << (TOTAL_OPERATIONS / avg) << " allocs/sec\n";

  return 0;
}