#pragma once

#include <vector>
#include <chrono>
#include <numeric>
#include <boost/container/small_vector.hpp>

constexpr size_t ITERATIONS = 100'000'000;
constexpr size_t SMALL_SIZE = 128;

using value_type = uint32_t;

using SmallVector = boost::container::small_vector<value_type, SMALL_SIZE>;
using StdVector = std::vector<value_type>;

template<typename VectorType>
size_t benchmark_task() {
    VectorType vec;
    if constexpr (std::is_same_v<VectorType, StdVector>) {
        vec.reserve(SMALL_SIZE);
    }
    
    size_t counter = 0;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        vec.emplace_back(i);
        counter += vec.back();
        if (vec.size() >= SMALL_SIZE) {
            vec.clear();
        }
    }
    return counter;
}

template<typename VectorType>
size_t loop() {
    volatile size_t result = benchmark_task<VectorType>();
    return result;
}