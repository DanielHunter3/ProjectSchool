#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstring>
#include <random>
#include <numeric>
#include <memory>

using Duration = std::chrono::duration<double>;

// Конфигурация
constexpr size_t THREADS = 8;
constexpr size_t OPERATIONS_PER_THREAD = 250'000;
constexpr size_t MIN_ALLOC_SIZE = 256;
constexpr size_t MAX_ALLOC_SIZE = 8192;
constexpr size_t BENCHMARK_RUNS = 5;

// Глобальные thread-local генераторы
namespace {
  thread_local std::mt19937 gen(std::random_device{}());
  thread_local std::uniform_int_distribution<size_t> size_dist(
    MIN_ALLOC_SIZE, MAX_ALLOC_SIZE
  );
}

void worker_thread(size_t& checksum) {
  std::vector<std::unique_ptr<void, void(*)(void*)>> ptrs;
  ptrs.reserve(OPERATIONS_PER_THREAD);

  for (size_t i = 0; i < OPERATIONS_PER_THREAD; ++i) {
    size_t size = size_dist(gen);
    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc();
    
    memset(ptr, 0xAA, size);
    checksum += *(static_cast<unsigned char*>(ptr));
    ptrs.emplace_back(ptr, free); // Автоматическое освобождение
  }
  // Явный clear не нужен - деструкторы unique_ptr сделают всё автоматически
}

double run_bench() {
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<std::thread> threads;
  std::vector<size_t> checksums(THREADS, 0);

  for (auto& checksum : checksums) {
    threads.emplace_back(worker_thread, std::ref(checksum));
  }

  for (auto& t : threads) {
    t.join();
  }

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
       << "Throughput: " << (THREADS * OPERATIONS_PER_THREAD / avg) << " allocs/sec\n";

  return 0;
}