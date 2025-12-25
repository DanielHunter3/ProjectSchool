#pragma once

#include <vector>
#include <chrono>
#include <cstring>
#include <boost/pool/pool.hpp>

constexpr size_t OBJECT_SIZE = 32;
constexpr size_t ALLOCATIONS = 1'000'000;

struct TestObject {
    char data[OBJECT_SIZE];
};

constexpr size_t SIZE = sizeof(TestObject);

void test_pool() {
    boost::pool<> local_pool(SIZE);

    std::vector<TestObject*> objects;
    objects.reserve(ALLOCATIONS);

     for (size_t i = 0; i < ALLOCATIONS; ++i) {
        auto ptr = static_cast<TestObject*>(local_pool.malloc());
        memset(ptr, 0xAA, SIZE);
        objects.emplace_back(ptr);
    }

    for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
        local_pool.free(*it);
    }
}

// Для теста malloc/free:
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

template <bool is_std>
void loop() {
    if constexpr (is_std) {
        test_malloc();
    } else {
        test_pool();
    }
}