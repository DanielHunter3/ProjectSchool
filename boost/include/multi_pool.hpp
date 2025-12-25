#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <array>
#include <boost/pool/pool.hpp>

constexpr size_t OBJECT_SIZE = 512;
constexpr size_t ALLOCATIONS_PER_THREAD = 1'000'000;
constexpr size_t THREAD_COUNT = 8;

struct TestObject {
    char data[OBJECT_SIZE];
};

constexpr size_t SIZE = sizeof(TestObject);

void test_pool() {
    thread_local boost::pool<> local_pool(SIZE);
    
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

template <bool is_std>
void loop() {
    std::array<std::thread, THREAD_COUNT> threads;
    
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        threads[i] = std::thread([]() {
            if constexpr (is_std) {
                test_malloc();
            } else {
                test_pool();
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}