#pragma once

#include "tape_sorter/sorting_algorithm.hpp"
#include "tape_sorter/file_tape.hpp"
#include "tape_sorter/tape.hpp"
#include <cstdint>

namespace tape_sorter {
  struct TimSortConfig {
    size_t max_ram_elements = 1;
  };

  class TimSortAlgorithm : public ISortingAlgorithm<uint32_t> {
    public:
      TimSortAlgorithm(const TimSortConfig& config = TimSortConfig{}, const FileTapeConfig& tape_config = FileTapeConfig{});
      
      void sort(const ITape<uint32_t>& input, ITape<uint32_t>& output) const override;
    
    private:
      TimSortConfig m_config;
      FileTapeConfig m_tape_config;
  };
}
