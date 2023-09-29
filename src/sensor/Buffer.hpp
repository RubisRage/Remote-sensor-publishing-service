#include <array>
#include <cstddef>

template <typename T, size_t capacity> class Buffer {
public:
  ~Buffer();

  Buffer() : buffer(), buffer_start(0), buffer_end(0) {}

  bool push(const T &value) {
    if (size() == capacity) {
      return false;
    }

    buffer[buffer_end] = value;
    increment(buffer_end);

    return true;
  }

  T *operator[](size_t offset) {
    if (offset >= size()) {
      return nullptr;
    }

    size_t real_index = buffer_start;
    increment(real_index, offset);
    return &buffer[real_index];
  }

  T *peek() {
    if (buffer_start == buffer_end) {
      return nullptr;
    }

    return &buffer[buffer_start];
  }

  void pop() {
    if (buffer_start == buffer_end) {
      return;
    }

    increment(buffer_start);
  }

  size_t size() const {
    size_t diff = buffer_end - buffer_start;
    if (diff < 0) {
      return buffer.size() - diff - 1;
    }

    return diff;
  }

  void increment(size_t &index, size_t inc = 1) {
    index = (index + inc) % buffer.size();
  }

  std::array<T, capacity> buffer;
  size_t buffer_start;
  size_t buffer_end;
};
