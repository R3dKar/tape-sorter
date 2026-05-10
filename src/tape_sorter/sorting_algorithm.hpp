#pragma once

#include "tape_sorter/tape.hpp"

namespace tape_sorter {
  template <typename T>
  class ISortingAlgorithm {
  public:
    virtual void sort(const ITape<T>& input, ITape<T>& output) const = 0;

    virtual ~ISortingAlgorithm() = default;
  };
} // namespace tape_sorter
