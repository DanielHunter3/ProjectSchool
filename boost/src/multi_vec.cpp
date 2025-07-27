#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <array>
#include <numeric>
#include <cmath>

#include <boost/container/small_vector.hpp>

// Конфигурация
constexpr size_t ITERATIONS = 100'000'000; // Операций на поток
constexpr size_t SMALL_SIZE = 128;
constexpr size_t THREADS = 8;
constexpr size_t BENCHMARK_RUNS = 5;

template<typename VectorType>
void thread_task(size_t& counter) {
  VectorType vec;
  if constexpr (std::is_same_v<VectorType, std::vector<int>>) {
    vec.reserve(SMALL_SIZE);
  }

  for (size_t i = 0; i < ITERATIONS; ++i) {
    vec.emplace_back(i);
    counter += vec.back(); // Защита от оптимизации
    
    if (vec.size() >= SMALL_SIZE) {
      vec.clear();
    }
  }
}

template<typename VectorType>
double run_benchmark_session() {
  auto start = std::chrono::high_resolution_clock::now();
  std::array<std::thread, THREADS> threads;
  std::array<size_t, THREADS> counters{};

  for (auto& t : threads) {
    t = std::thread(thread_task<VectorType>, std::ref(counters.back()));
  }
  for (auto& t : threads) t.join();

  return std::chrono::duration<double>(
    std::chrono::high_resolution_clock::now() - start
  ).count();
}

template<typename VectorType>
void run_benchmark(const std::string& name) {
  std::vector<double> timings;
  timings.reserve(BENCHMARK_RUNS);

  for (size_t i = 0; i < BENCHMARK_RUNS; ++i) {
    timings.push_back(run_benchmark_session<VectorType>());
    std::cerr << name << " run " << (i+1) << ": " << timings.back() << " sec\n";
  }

  // Статистика
  double avg = std::accumulate(timings.begin(), timings.end(), 0.0) / BENCHMARK_RUNS;
  double stddev = 0.0;
  for (double t : timings) stddev += (t - avg) * (t - avg);
  stddev = std::sqrt(stddev / BENCHMARK_RUNS);

  std::cout << "[" << name << "]\n"
       << " Time (avg):  " << avg << " sec\n"
       << " StdDev:    ±" << stddev << "\n"
       << " Throughput:  " << (THREADS * ITERATIONS / avg) << " ops/sec\n";
}

int main() {
  run_benchmark<std::vector<int>>("std::vector");
  run_benchmark<boost::container::small_vector<int, SMALL_SIZE>>("small_vector");
  return 0;
}