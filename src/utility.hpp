#pragma once

#include "tape.hpp"
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>

namespace tape_sorter::utility {
  template <typename T>
  size_t count_file_items(const std::filesystem::path& filename) {
    std::ifstream file(filename);

    T item;
    size_t count = 0;

    while (file >> item) {
      count++;
    }

    return count;
  }

  template <typename T>
  void shift_to(const ITape<T>& tape, size_t index) {
    if (tape.pos() == index) return;

    if (index >= tape.size()) {
      tape.rewind_forward();
      return;
    }

    // Strategy 1: shift_backward() or shift_forward() abs(tape.pos() - index) times
    // Strategy 2: rewind_backward() + index times shift_forward()
    // Strategy 3: rewind_forward() + size - 1 - index times shift_backward()

    const size_t cost1 = std::abs(static_cast<ptrdiff_t>(tape.pos()) - index);
    const size_t cost2 = index;
    const size_t cost3 = tape.size() - 1 - index;

    if (cost1 >= cost2 && cost1 >= cost3) {
      // Strategy 1
      for (size_t i = 0; i < cost1; i++) {
        if (tape.pos() < index)
          tape.shift_forward();
        else
          tape.shift_backward();
      }
    } else if (cost2 >= cost1 && cost2 >= cost3) {
      // Strategy 2
      tape.rewind_backward();
      for (size_t i = 0; i < cost2; i++) {
        tape.shift_forward();
      }
    } else {
      // Strategy 3
      tape.rewind_forward();
      for (size_t i = 0; i < cost3; i++) {
        tape.shift_backward();
      }
    }
  }
} // namespace tape_sorter::utility
