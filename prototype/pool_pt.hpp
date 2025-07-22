#include <memory>
#include <vector>

template <size_t BlockSize, size_t Alignment = 16>
class MemoryPool {
  static_assert(Alignment >= alignof(void*), "Alignment too small");
  union Block {
    Block* next;
    alignas(Alignment) char data[BlockSize];
  };

  Block* free_list = nullptr;
  std::vector<void*> chunks;

  void allocate_chunk() {
    constexpr size_t ChunkSize = 4096;
    void* chunk = std::aligned_alloc(Alignment, ChunkSize);
    chunks.push_back(chunk);
    
    Block* blocks = reinterpret_cast<Block*>(chunk);
    size_t num_blocks = ChunkSize / sizeof(Block);
    for (size_t i = 0; i < num_blocks; ++i) {
      blocks[i].next = free_list;
      free_list = &blocks[i];
    }
  }

public:
  void* allocate() {
    if (!free_list) allocate_chunk();
    Block* block = free_list;
    free_list = free_list->next;
    return block->data;
  }

  void deallocate(void* ptr) {
    Block* block = reinterpret_cast<Block*>(ptr);
    block->next = free_list;
    free_list = block;
  }

  ~MemoryPool() {
    for (void* chunk : chunks) std::free(chunk);
  }
};