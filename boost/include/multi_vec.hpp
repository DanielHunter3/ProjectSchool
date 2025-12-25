#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include <boost/container/small_vector.hpp>

constexpr size_t ITERATIONS = 100'000'000;
constexpr size_t SMALL_SIZE = 128;
constexpr size_t THREADS = 8;

using value_type = uint32_t;

using SmallVector = boost::container::small_vector<value_type, SMALL_SIZE>;
using StdVector = std::vector<value_type>;

template<typename VectorType>
size_t thread_task() {
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

template <typename T>
size_t loop() {
    std::array<std::thread, THREADS> threads;
    std::array<size_t, THREADS> results;
    
    for (size_t i = 0; i < THREADS; ++i) {
        threads[i] = std::thread([&results, i]() {
            results[i] = thread_task<T>();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Защита от оптимизации
    volatile size_t total = 0;
    for (auto r : results) total += r;

    return total;
}