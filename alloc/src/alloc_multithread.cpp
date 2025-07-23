#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <cstring>
#include <functional>
#include <array>

#ifdef USE_JEMALLOC
#include <jemalloc/jemalloc.h>
#endif

using Duration = std::chrono::duration<double>;

constexpr size_t THREADS = 8;
constexpr size_t OPERATIONS_PER_THREAD = 500'000;
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

void worker_thread() {
  std::vector<void*> ptrs;
  ptrs.reserve(OPERATIONS_PER_THREAD);

  auto func = [&ptrs]() {
    for (void* ptr: ptrs) {
      free(ptr);
    }
  };

  for (size_t i = 0; i < OPERATIONS_PER_THREAD; ++i) {
    size_t size = 16 + (i % 1024); // Разные размеры
    void* ptr = fmalloc(size, func);
    memset(ptr, 0xAA, size); // Заполнение памяти (антимёртвый код)
    ptrs.emplace_back(ptr);
  }

  func();
}

int main() {
  auto start = std::chrono::high_resolution_clock::now();

  std::array<std::thread, THREADS> threads;
  for (size_t i = 0; i < THREADS; ++i) {
    threads[i] = std::thread(worker_thread);
  }
  for (auto& t : threads) t.join();

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