#pragma once

#include <Arduino.h>

template <typename T, size_t N>
class CircularArray {
  public:
    CircularArray() : head(0), tail(0) {}

    size_t push(const T& value) {
      array[head] = value;
      size_t index = head;
      head = (head + 1) % N;
      if (head == tail) {
        tail = (tail + 1) % N;
      }
      return index;
    }

    T pop() {
      if (head == tail) {
        return T(); // Return default value if empty
      }
      T value = array[tail];
      tail = (tail + 1) % N;
      return value;
    }

    bool isEmpty() const {
      return head == tail;
    }

    bool isFull() const {
      return (head + 1) % N == tail;
    }

    size_t size() const {
      return (head - tail + N) % N;
    }

    T& operator[](size_t index) {
      return array[(tail + index) % N];
    }

    const T& operator[](size_t index) const {
      return array[(tail + index) % N];
    }

    T array[N];
    size_t head;
    size_t tail;
};