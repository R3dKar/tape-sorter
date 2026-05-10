#pragma once

#include "tape.hpp"

namespace tape_sorter {
  template <typename T>
  class ISortingAlgorithm {
  public:
    void sort(const ITape<T>& input, ITape<T>& output) const = 0;

    virtual ~ISortingAlgorithm() = default;
  };
} // namespace tape_sorter
