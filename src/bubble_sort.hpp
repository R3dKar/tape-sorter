#pragma once

#include "sorting_algorithm.hpp"
#include "utility.hpp"
#include <cstddef>

namespace tape_sorter {
  template <typename T>
  class BubbleSortAlgorithm : public ISortingAlgorithm<T> {
  public:
    BubbleSortAlgorithm() = default;

    void sort(const ITape<T>& input, ITape<T>& output) const override {
      input.rewind_backward();
      output.rewind_backward();

      for (size_t i = 0; i < input.size(); i++) {
        output.write(intput.read());
        input.shift_forward();
        output.shift_forward();
      }

      for (size_t i = 0; i < output.size() - 1; i++) {
        utility::shift_to(output, i);
        T value_i = output.read();

        for (size_t j = i + 1; j < output.size(); j++) {
          utility::shift_to(output, j);
          T value_j = output.read();

          if (value_i > value_j) {
            output.write(value_i);
            utility::shift_to(output, i);
            output.write(value_j);
            value_i = value_j;
          }
        }
      }
    }
  };
} // namespace tape_sorter
