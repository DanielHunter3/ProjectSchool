#include <vector>
#include <chrono>
#include <iostream>
#include <numeric>
#include <cmath>
#include <boost/container/small_vector.hpp>

// Конфигурация
constexpr size_t ITERATIONS = 100'000'000; // Общее количество операций
constexpr size_t SMALL_SIZE = 128;
constexpr size_t BENCHMARK_RUNS = 5;

template<typename VectorType>
void benchmark_task(size_t& counter) {
  VectorType vec;
  if constexpr (std::is_same_v<VectorType, std::vector<int>>) {
    vec.reserve(SMALL_SIZE);
  }

  for (size_t i = 0; i < ITERATIONS; ++i) {
    vec.emplace_back(i);
    counter += vec.back();
    
    if (vec.size() >= SMALL_SIZE) {
      vec.clear();
    }
  }
}

template<typename VectorType>
double run_benchmark_session() {
  auto start = std::chrono::high_resolution_clock::now();
  size_t counter = 0;
  
  benchmark_task<VectorType>(counter);
  
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
  for (double t : timings) {
    stddev += (t - avg) * (t - avg);
  }
  stddev = std::sqrt(stddev / BENCHMARK_RUNS);

  std::cout << "[" << name << "]\n"
       << " Time (avg):  " << avg << " sec\n"
       << " StdDev:    ±" << stddev << "\n"
       << " Throughput:  " << (ITERATIONS / avg) << " ops/sec\n";
}

int main() {
  run_benchmark<std::vector<int>>("std::vector");
  run_benchmark<boost::container::small_vector<int, SMALL_SIZE>>("small_vector");
  return 0;
}