#include "tape_sorter/tim_sort.hpp"
#include "tape_sorter/file_tape.hpp"
#include "tape_sorter/temp_file_tape.hpp"
#include "tape_sorter/utility.hpp"
#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <vector>

namespace tape_sorter {
  TimSortAlgorithm::TimSortAlgorithm(const Config& config, const FileTape::Config& tape_config) : m_config(config), m_tape_config(tape_config) {
    if (m_config.max_ram_elements == 0) {
      throw std::invalid_argument("Ram elements must be at least 1");
    }
  }

  void TimSortAlgorithm::sort(const ITape<uint32_t>& input, ITape<uint32_t>& output) const {
    namespace fs = std::filesystem;

    if (input.size() != output.size()) {
      throw std::logic_error("Input and output tape sizes don't match");
    }

    if (input.size() <= 1) return;

    std::vector<uint32_t> ram_storage{};
    ram_storage.reserve(m_config.max_ram_elements);

    TempFileTape temp_tape1(output.size(), m_tape_config);
    TempFileTape temp_tape2(output.size(), m_tape_config);

    input.rewind_backward();
    output.rewind_backward();

    // Step 1 - sort arrays of fixed size (max_ram_elements)
    for (size_t start = 0; start < input.size(); start += m_config.max_ram_elements) {
      const size_t end = std::min(input.size(), start + m_config.max_ram_elements);

      // copy elements to ram
      ram_storage.clear();
      for (size_t i = start; i < end; i++) {
        ram_storage.push_back(input.read());
        input.shift_forward();
      }

      // sort elements
      // this should be an insertion sort but nah
      std::sort(ram_storage.begin(), ram_storage.end());

      // write to output tape
      for (size_t i = start; i < end; i++) {
        output.write(ram_storage[i - start]);
        output.shift_forward();
      }
    }

    // Step 2 - merge sorted parts
    size_t merge_width = m_config.max_ram_elements;
    while (merge_width < output.size()) {
      // copy data to both temp tapes (in chess order)
      output.rewind_backward();
      temp_tape1.rewind_backward();
      temp_tape2.rewind_backward();

      for (size_t i = 0; i < output.size(); i++) {
        const auto value = output.read();

        if ((i % (merge_width * 2)) < merge_width) {
          temp_tape1.write(value);
        } else {
          temp_tape2.write(value);
        }

        output.shift_forward();
        temp_tape1.shift_forward();
        temp_tape2.shift_forward();
      }

      // merge
      output.rewind_backward();
      for (size_t start1 = 0; start1 < output.size(); start1 += 2 * merge_width) {
        const size_t start2 = start1 + merge_width;

        const size_t end1 = std::min(start1 + merge_width, output.size());
        const size_t end2 = std::min(start2 + merge_width, output.size());

        utility::shift_to(temp_tape1, start1);
        utility::shift_to(temp_tape2, start2);

        size_t i = start1;
        size_t j = start2;

        uint32_t value1 = temp_tape1.read();
        uint32_t value2 = temp_tape2.read();

        // pick from both tapes
        while ((i < end1) && (j < end2)) {
          if (value1 <= value2) {
            output.write(value1);
            temp_tape1.shift_forward();
            value1 = temp_tape1.read();
            i++;
          } else {
            output.write(value2);
            temp_tape2.shift_forward();
            value2 = temp_tape2.read();
            j++;
          }
          output.shift_forward();
        }

        // copy the remainder from the first tape
        while (i < end1) {
          output.write(temp_tape1.read());
          temp_tape1.shift_forward();
          output.shift_forward();
          i++;
        }

        // copy the remainder from the second tape
        while (j < end2) {
          output.write(temp_tape2.read());
          temp_tape2.shift_forward();
          output.shift_forward();
          j++;
        }
      }

      // double the merge width
      merge_width *= 2;
    }
  }
} // namespace tape_sorter
