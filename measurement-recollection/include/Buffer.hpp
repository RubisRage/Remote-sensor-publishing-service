#pragma once

#include "Logger.hpp"
#include <array>
#include <cstddef>
#include <iterator>

template <typename T, size_t capacity> class Buffer {
public:
  ~Buffer() = default;

  Buffer() : buffer(), buffer_start(0), buffer_end(0) {}

  bool push(const T &value) {
    if (size() == capacity) {
      return false;
    }

    buffer[buffer_end] = value;
    increment(buffer_end);
    size_++;

    return true;
  }

  T *operator[](size_t offset) {
    if (offset >= size()) {
      serial.log(LogLevel::error, __FUNCTION__,
                 " returned nullptr! offset: ", offset, ", size: ", size());
      return nullptr;
    }

    size_t real_index = buffer_start;
    increment(real_index, offset);
    return &buffer[real_index];
  }

  T *peek() {
    if (size() == 0) {
      serial.log(LogLevel::error, __FUNCTION__, "returned nullptr!");
      return nullptr;
    }

    return &buffer[buffer_start];
  }

  bool pop(size_t count = 1) {
    if (size() < count) {
      return false;
    }

    increment(buffer_start, count);
    size_ -= count;
    return true;
  }

  T *allocate(size_t count = 1) {
    if (capacity - size() < count) {
      serial.log(LogLevel::error, __FUNCTION__, " returned nullptr!");
      return nullptr;
    }

    size_t alloc_start = buffer_end;
    increment(buffer_end, count);
    size_ += count;

    return &buffer[alloc_start];
  }

  inline size_t size() const { return size_; }

  void increment(size_t &index, size_t inc = 1) {
    index = (index + inc) % buffer.size();
  }

  std::array<T, capacity> buffer;
  size_t buffer_start;
  size_t buffer_end;
  size_t size_;
};
