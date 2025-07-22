#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <algorithm>
#include <pool_pt.hpp>

constexpr size_t MAX_SMALL_SIZE = 4096;
constexpr size_t ARENA_COUNT = 16;

class Jemalloc {
  struct Arena {
    std::mutex mutex;
    std::vector<void*> large_blocks;
    MemoryPool<64> small_pool; // Пул для маленьких объектов
  };

  Arena arenas[ARENA_COUNT];
  static thread_local Arena* local_arena;

  Arena* get_arena() {
    if (!local_arena) {
      static std::atomic<size_t> next_arena = 0;
      local_arena = &arenas[next_arena++ % ARENA_COUNT];
    }
    return local_arena;
  }

public:
  void* allocate(size_t size) {
    if (size <= MAX_SMALL_SIZE) {
      return get_arena()->small_pool.allocate();
    } else {
      void* block = std::aligned_alloc(64, size); // Выравнивание под AVX
      std::lock_guard<std::mutex> lock(get_arena()->mutex);
      get_arena()->large_blocks.push_back(block);
      return block;
    }
  }

  void deallocate(void* ptr, size_t size) {
    if (size <= MAX_SMALL_SIZE) {
      get_arena()->small_pool.deallocate(ptr);
    } else {
      std::lock_guard<std::mutex> lock(get_arena()->mutex);
      auto& blocks = get_arena()->large_blocks;
      blocks.erase(std::remove(blocks.begin(), blocks.end(), ptr), blocks.end());
      std::free(ptr);
    }
  }
};

thread_local Jemalloc::Arena* Jemalloc::local_arena = nullptr;