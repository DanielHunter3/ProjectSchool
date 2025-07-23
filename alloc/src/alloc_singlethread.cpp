#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <functional>

#ifdef USE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

using Duration = std::chrono::duration<double>;

constexpr size_t OPERATIONS_TOTAL = 4'000'000; // 8 потоков * 500'000 операций
constexpr size_t MIN_ALLOC_SIZE = 256;
constexpr size_t MAX_ALLOC_SIZE = 8192;

void* fmalloc(size_t Size, std::function<void(void)> func) {
  void* ptr = malloc(Size);
  if (!ptr) {
    std::cerr << "Alloc failed!\n";
    func();
    exit(1);
  }
  return ptr;
}

void single_thread_test() {
  std::vector<void*> ptrs;
  ptrs.reserve(OPERATIONS_TOTAL);

 auto cleanup = [&ptrs]() {
    for (void* ptr : ptrs) {
      free(ptr);
    }
  };

  for (size_t i = 0; i < OPERATIONS_TOTAL; ++i) {
    size_t size = 16 + (i % 1024);
      void* ptr = fmalloc(size, cleanup);
      memset(ptr, 0xAA, size);
      ptrs.push_back(ptr);
    }
    
  cleanup();
}

int main() {
  auto start = std::chrono::high_resolution_clock::now();
 
  single_thread_test();
 
  auto end = std::chrono::high_resolution_clock::now();

#if defined(USE_JEMALLOC)
  std::cout << "Jemalloc: ";
#elif defined(USE_TCMALLOC)
  std::cout << "Tcmalloc: ";
#elif defined(USE_MIMALLOC)
  std::cout << "Mimalloc: ";
#else
  std::cout << "Malloc: ";
#endif

  std::cout << Duration(end - start).count() << " sec\n";
 
  return 0;
}