#include <vector>
#include <chrono>
#include <iostream>
#include <boost/container/small_vector.hpp>

constexpr size_t ITERATIONS = 1'000'000'000; // Общее количество операций
constexpr size_t SMALL_SIZE = 64;      // Размер буфера

template<typename VectorType>
void test_container(const std::string& name) {
  VectorType vec;
  if constexpr (std::is_same_v<VectorType, std::vector<int>>) {
    vec.reserve(SMALL_SIZE);
  }

  auto start = std::chrono::high_resolution_clock::now();
  
  for (size_t i = 0; i < ITERATIONS; ++i) {
    vec.emplace_back(i);
    if (vec.size() >= SMALL_SIZE) vec.clear();
  }

  auto duration = std::chrono::duration<double>(
    std::chrono::high_resolution_clock::now() - start
  ).count();

  std::cout << name << ": " << duration << " sec\n";
}

int main() {
  test_container<std::vector<int>>("std::vector");
  test_container<boost::container::small_vector<int, SMALL_SIZE>>("small_vector");
  return 0;
}