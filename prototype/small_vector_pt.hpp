#include <cstddef>

template <typename T, size_t N>
class SmallVector {
  alignas(T) char stack_buffer[N * sizeof(T)];
  T* heap_data = nullptr;
  size_t size_ = 0;
  size_t capacity_ = N;

  T* data_ptr() {
    return heap_data ? heap_data : reinterpret_cast<T*>(stack_buffer);
  }

  void grow() {
    size_t new_capacity = capacity_ * 2;
    T* new_heap = static_cast<T*>(std::aligned_alloc(alignof(T), new_capacity * sizeof(T)));
    std::uninitialized_copy(data_ptr(), data_ptr() + size_, new_heap);
    if (heap_data) std::free(heap_data);
    heap_data = new_heap;
    capacity_ = new_capacity;
  }

public:
  void push_back(const T& value) {
    if (size_ >= capacity_) grow();
    new (data_ptr() + size_) T(value);
    size_++;
  }

  ~SmallVector() {
    for (size_t i = 0; i < size_; ++i) {
      data_ptr()[i].~T();
    }
    if (heap_data) std::free(heap_data);
  }
};