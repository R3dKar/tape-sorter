#pragma once

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
} // namespace tape_sorter::utility
