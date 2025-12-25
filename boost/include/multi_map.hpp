#pragma once

#include <vector>
#include <thread>
#include <cstring>
#include <boost/pool/pool_alloc.hpp>

constexpr size_t OBJECT_SIZE = 512;
constexpr size_t ALLOCATIONS_PER_THREAD = 250'000;
constexpr size_t THREAD_COUNT = 4;

struct TestObject {
    char data[OBJECT_SIZE];
    TestObject() { 
        std::memset(data, 0, OBJECT_SIZE);
    }
};

// 1. Стандартный аллокатор
struct StdAlloc {
    static TestObject* allocate() { 
        return new TestObject; 
    }
    static void deallocate(TestObject* p) { 
        delete p; 
    }
};

// 3. Thread-local аллокатор
struct BoostPoolAlloc {
    static TestObject* allocate() { 
        thread_local boost::fast_pool_allocator<TestObject> alloc;
        return alloc.allocate(1);
    }
    
    static void deallocate(TestObject* p) { 
        thread_local boost::fast_pool_allocator<TestObject> alloc;
        alloc.deallocate(p, 1);
    }
};

// Основной бенчмарк
template<typename Alloc>
void loop() {
    std::thread threads[THREAD_COUNT];
    
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads[i] = std::thread([]() {
            std::vector<TestObject*> objects;
            objects.reserve(ALLOCATIONS_PER_THREAD);
            
            // Аллокация
            for (size_t j = 0; j < ALLOCATIONS_PER_THREAD; ++j) {
                objects.push_back(Alloc::allocate());
            }
            
            // Деаллокация
            for (auto ptr : objects) {
                Alloc::deallocate(ptr);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}