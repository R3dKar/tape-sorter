#pragma once

#include "tape_sorter/sorting_algorithm.hpp"
#include "tape_sorter/file_tape.hpp"
#include "tape_sorter/tape.hpp"
#include <cstdint>

namespace tape_sorter {
  class TimSortAlgorithm : public ISortingAlgorithm<uint32_t> {
    public:
      struct Config {
        size_t max_ram_elements = 1;
      };
      
      TimSortAlgorithm(const Config& config = Config{}, const FileTape::Config& tape_config = FileTape::Config{});
      
      void sort(const ITape<uint32_t>& input, ITape<uint32_t>& output) const override;
    
    private:
      Config m_config;
      FileTape::Config m_tape_config;
  };
}
