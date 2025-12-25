#pragma once

#include <map>
#include <chrono>
#include <cstring>
#include <vector>
#include <boost/pool/pool_alloc.hpp>

constexpr size_t OBJECT_SIZE = 512;
constexpr size_t ALLOCATIONS = 1'000'000;

struct TestObject {
    char data[OBJECT_SIZE];
    TestObject() { 
        for (size_t i = 0; i < OBJECT_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

template<typename Alloc>
void benchmark_map() {
    using MapType = std::map<int, TestObject*>;
    MapType map;
    
    for (size_t i = 0; i < ALLOCATIONS; ++i) {
        auto ptr = Alloc::allocate();
        map.emplace(i, ptr);
    }
    
    for (auto& [key, ptr] : map) {
        Alloc::deallocate(ptr);
    }
}

struct StdAlloc {
    static TestObject* allocate() { return new TestObject; }
    static void deallocate(TestObject* p) { delete p; }
};

struct BoostPoolAlloc {
    static TestObject* allocate() { 
        return boost::fast_pool_allocator<TestObject>().allocate(1); 
    }
    static void deallocate(TestObject* p) { 
        boost::fast_pool_allocator<TestObject>().deallocate(p, 1); 
    }
};

template <typename T>
void loop() {
    benchmark_map<T>();
    
    if constexpr (std::is_same_v<BoostPoolAlloc, T>) {
        boost::singleton_pool<boost::fast_pool_allocator_tag, 
            sizeof(TestObject)>::purge_memory();
    }
}