#pragma once

#include "tape.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>

namespace tape_sorter {
  class FileTape : public ITape<uint32_t> {
  public:
    struct Config {
      std::chrono::nanoseconds shift_latency{};
      std::chrono::nanoseconds rewind_latency{};
      std::chrono::nanoseconds read_latency{};
      std::chrono::nanoseconds write_latency{};
    };

    FileTape(const char* filename, const Config& config = Config{});
    FileTape(size_t n, const char* filename, const Config& config = Config{});

    size_t size() const override;
    size_t pos() const override;

    void shift_forward() override;
    void shift_backward() override;

    void rewind_forward() override;
    void rewind_backward() override;

    void write(const uint32_t& value) override;
    uint32_t read() override;

  private:
    static constexpr size_t NUMBER_WIDTH = std::numeric_limits<uint32_t>::digits10 + 1;
    static constexpr size_t LINE_WIDTH = NUMBER_WIDTH + 1;

    void normalize_file();

    const size_t m_size;
    Config m_config;
    size_t m_pos;
    bool m_file_normalized;
    std::fstream m_file;
    std::filesystem::path m_file_path;
  };
} // namespace tape_sorter
