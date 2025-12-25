#pragma once

#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include <boost/pool/pool_alloc.hpp>

constexpr size_t OBJECT_SIZE = 1024;
constexpr size_t ALLOCATIONS_PER_THREAD = 250'000;
constexpr size_t THREAD_COUNT = 8;

struct TestObject {
    alignas(64) char data[OBJECT_SIZE];
    TestObject() {
        for (size_t i = 0; i < OBJECT_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

using BoostPoolAlloc = boost::fast_pool_allocator<TestObject>;
using StdAlloc = std::allocator<TestObject>;

template<typename Alloc>
void thread_task(Alloc& alloc) {
    // Создаем аллокатор для указателей на основе оригинального аллокатора
    using PtrAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<TestObject*>;
    PtrAlloc ptr_alloc(alloc);
    
    std::vector<TestObject*, PtrAlloc> objects(ptr_alloc);
    objects.reserve(ALLOCATIONS_PER_THREAD);

    for (size_t i = 0; i < ALLOCATIONS_PER_THREAD; ++i) {
        TestObject* obj = alloc.allocate(1);
        new(obj) TestObject(); // Вызов конструктора
        objects.push_back(obj);
    }

    for (auto obj : objects) {
        obj->~TestObject(); // Вызов деструктора
        alloc.deallocate(obj, 1);
    }
}

template <typename tallocator>
void loop() {
    tallocator alloc;
    
    std::array<std::thread, THREAD_COUNT> threads;
    
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        threads[i] = std::thread([&alloc]() {
            thread_task<tallocator>(alloc);
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    if constexpr (
        std::is_same_v<
            BoostPoolAlloc, 
            tallocator
        >
    ) {
        boost::singleton_pool<boost::fast_pool_allocator_tag, 
            sizeof(TestObject)>::purge_memory();
    }
}