#include <iostream>
#include <map>
#include <chrono>
#include <cstring>
#include <numeric>
#include <cmath>
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iomanip>

#include <boost/pool/pool_alloc.hpp>
#include <boost/pool/singleton_pool.hpp>

using Duration = std::chrono::duration<double>;

constexpr size_t OBJECT_SIZE = 32*16*2*2*2;
constexpr size_t ALLOCATIONS = 1'000'000;
constexpr size_t BENCHMARK_RUNS = 7;
constexpr size_t WARMUP_RUNS = 3;

struct TestObject {
    char data[OBJECT_SIZE];
    TestObject() { 
        for (size_t i = 0; i < OBJECT_SIZE; ++i) {
            data[i] = static_cast<char>(i % 256);
        }
    }
};

constexpr size_t SIZE = sizeof(TestObject);

class MemoryTracker {
public:
    static size_t current_usage() {
        #ifdef __linux__
        std::ifstream statm("/proc/self/statm");
        if (statm) {
            size_t resident;
            statm >> resident >> resident;
            return resident * 4;
        }
        #endif
        return 0;
    }
};

template<typename Alloc>
struct BenchmarkResult {
    Duration time;
    size_t peak_memory;
};

template<typename Alloc>
BenchmarkResult<Alloc> benchmark_map() {
    using MapType = std::map<int, TestObject*>;
    MapType map;
    size_t peak_mem = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < ALLOCATIONS; ++i) {
        auto ptr = Alloc::allocate(1);
        map.emplace(i, ptr);
        
        if (i % 10000 == 0) {
            peak_mem = std::max(peak_mem, MemoryTracker::current_usage());
        }
    }
    
    for (auto& [key, ptr] : map) {
        Alloc::deallocate(ptr, 1);
    }
    
    return {std::chrono::high_resolution_clock::now() - start, peak_mem / 1024};
}

struct StdAlloc {
    static TestObject* allocate(size_t) { return new TestObject; }
    static void deallocate(TestObject* p, size_t) { delete p; }
};

struct BoostPoolAlloc {
    static TestObject* allocate(size_t) { 
        return boost::fast_pool_allocator<TestObject>().allocate(1); 
    }
    static void deallocate(TestObject* p, size_t) { 
        boost::fast_pool_allocator<TestObject>().deallocate(p, 1); 
    }
};

void run_test(const std::string& name, auto alloc_func) {
    std::vector<double> timings;
    std::vector<size_t> memory_usages;
    timings.reserve(BENCHMARK_RUNS);
    memory_usages.reserve(BENCHMARK_RUNS);

    std::cout << "\n=== " << name << " ===\n";
    
    // Warmup
    for (size_t i = 0; i < WARMUP_RUNS; ++i) {
        alloc_func();
    }
    
    // Benchmark
    for (size_t i = 0; i < BENCHMARK_RUNS; ++i) {
        auto result = alloc_func();
        timings.push_back(result.time.count());
        memory_usages.push_back(result.peak_memory);
        
        std::cout << "Run " << i+1 << ": " 
                  << std::fixed << std::setprecision(6) 
                  << result.time.count() << " sec | "
                  << "Peak mem: " << result.peak_memory << " MB\n";
    }

    // Calculate statistics
    double avg_time = std::accumulate(timings.begin(), timings.end(), 0.0) / timings.size();
    double stddev_time = 0.0;
    for (double t : timings) stddev_time += (t - avg_time) * (t - avg_time);
    stddev_time = sqrt(stddev_time / timings.size());

    size_t avg_mem = std::accumulate(memory_usages.begin(), memory_usages.end(), 0) / memory_usages.size();

    std::cout << "\n[RESULTS] " << name << "\n"
              << "Average time: " << avg_time << " sec (±" << stddev_time << ")\n"
              << "Average peak memory: " << avg_mem << " MB\n\n";
}

int main() {
    std::cout << "Benchmarking allocators with " << ALLOCATIONS 
              << " objects of size " << OBJECT_SIZE << " bytes (" 
              << (ALLOCATIONS*OBJECT_SIZE)/1024/1024 << " MB total)\n";

    boost::singleton_pool<boost::fast_pool_allocator_tag, SIZE>::purge_memory();
    
    run_test("std::allocator", []{ return benchmark_map<StdAlloc>(); });
    run_test("boost::fast_pool", []{ return benchmark_map<BoostPoolAlloc>(); });

    std::cout << "Benchmark completed.\n";
    return 0;
}