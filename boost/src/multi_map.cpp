#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <mutex>
#include <cmath>

#include <boost/pool/pool_alloc.hpp>

using Duration = std::chrono::duration<double>;

constexpr size_t OBJECT_SIZE = 1024;
constexpr size_t ALLOCATIONS_PER_THREAD = 250'000;
constexpr size_t THREAD_COUNT = 4;
constexpr size_t TOTAL_ALLOCATIONS = ALLOCATIONS_PER_THREAD * THREAD_COUNT;

struct TestObject {
    alignas(64) char data[OBJECT_SIZE]; // Выравнивание по кэш-линии
    TestObject() {
        for (size_t i = 0; i < OBJECT_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

template<typename T>
using TS_StdAlloc = std::allocator<T>;

template<typename T>
using TS_BoostPoolAlloc = boost::fast_pool_allocator<T,
    boost::default_user_allocator_new_delete,
    std::mutex>;

struct ThreadResult {
    Duration time;
    size_t allocations;
};

template<typename Alloc>
void thread_task(Alloc& alloc, ThreadResult& result) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<TestObject*, Alloc> objects(alloc);
    objects.reserve(ALLOCATIONS_PER_THREAD);

    for (size_t i = 0; i < ALLOCATIONS_PER_THREAD; ++i) {
        objects.push_back(alloc.allocate(1));
    }

    for (auto obj : objects) {
        alloc.deallocate(obj, 1);
    }

    result.time = std::chrono::high_resolution_clock::now() - start;
    result.allocations = ALLOCATIONS_PER_THREAD;
}

void run_benchmark(const std::string& name, auto alloc_factory) {
    std::vector<std::thread> threads;
    std::vector<ThreadResult> results(THREAD_COUNT);
    auto alloc = alloc_factory();

    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&, i] {
            thread_task(alloc, results[i]);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    Duration total_time = end - start;

    // Статистика по потокам
    double avg_time = 0;
    size_t total_allocs = 0;
    for (auto& res : results) {
        avg_time += res.time.count();
        total_allocs += res.allocations;
    }
    avg_time /= THREAD_COUNT;

    double stddev = 0;
    for (auto& res : results) {
        stddev += std::pow(res.time.count() - avg_time, 2);
    }
    stddev = std::sqrt(stddev / THREAD_COUNT);

    std::cout << "\n=== " << name << " ===\n"
              << "Threads: " << THREAD_COUNT << "\n"
              << "Total operations: " << TOTAL_ALLOCATIONS * 2 << "\n"
              << "Total time: " << total_time.count() << " sec\n"
              << "Throughput: " << int(TOTAL_ALLOCATIONS / total_time.count()) 
              << " allocs/sec\n"
              << "Thread time (avg±stddev): " << avg_time << " ± " << stddev << " sec\n";
}

int main() {
    std::cout << "Multithreaded Allocator Benchmark\n"
              << "Object size: " << OBJECT_SIZE << " bytes\n"
              << "Threads: " << THREAD_COUNT << "\n"
              << "Allocations per thread: " << ALLOCATIONS_PER_THREAD << "\n"
              << "Total allocations: " << TOTAL_ALLOCATIONS << "\n\n";

    // Тестируем в обратном порядке для минимизации влияния кэша
    run_benchmark("boost::fast_pool", [] { return TS_BoostPoolAlloc<TestObject>(); });
    run_benchmark("std::allocator", [] { return TS_StdAlloc<TestObject>(); });

    boost::singleton_pool<boost::fast_pool_allocator_tag, 
                         sizeof(TestObject)>::purge_memory();

    return 0;
}