#include <cstddef>
#include <unistd.h>
#include <sys/mman.h>

// Выравнивает `size` до ближайшего `alignment` (степень двойки!)
inline size_t align_up(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

// Реальный malloc должен учитывать максимальное выравнивание (например, для AVX-512 — 64 байта)
constexpr size_t MAX_ALIGN = alignof(std::max_align_t);

struct BlockHeader {
  size_t size;
  BlockHeader* next;
};

static BlockHeader* free_list = nullptr;

void* my_malloc(size_t size) {
  if (size == 0) return nullptr;

  // Выравниваем размер блока и добавляем место под заголовок
  size_t total_size = align_up(size + sizeof(BlockHeader), MAX_ALIGN);
  BlockHeader* block = free_list;
  BlockHeader** prev = &free_list;

  // Поиск подходящего блока в свободном списке
  while (block) {
    if (block->size >= total_size) {
      *prev = block->next; // Удаляем из списка свободных
      return reinterpret_cast<char*>(block) + sizeof(BlockHeader);
    }
    prev = &block->next;
    block = block->next;
  }

  // Если подходящего блока нет — запрашиваем у ОС
  void* mem = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, 
           MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (mem == MAP_FAILED) return nullptr;

  block = reinterpret_cast<BlockHeader*>(mem);
  block->size = total_size;
  return reinterpret_cast<char*>(block) + sizeof(BlockHeader);
}

void my_free(void* ptr) {
  if (!ptr) return;
  BlockHeader* block = reinterpret_cast<BlockHeader*>(
    reinterpret_cast<char*>(ptr) - sizeof(BlockHeader)
  );
  block->next = free_list;
  free_list = block;
}