#pragma once

#include "tape_sorter/tape.hpp"
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

    FileTape(const std::filesystem::path& filename, const Config& config = Config{});
    FileTape(size_t n, const std::filesystem::path& filename, const Config& config = Config{});

    size_t size() const override;
    size_t pos() const override;

    void shift_forward() const override;
    void shift_backward() const override;

    void rewind_forward() const override;
    void rewind_backward() const override;

    void write(const uint32_t& value) override;
    uint32_t read() const override;

  private:
    static constexpr size_t NUMBER_WIDTH = std::numeric_limits<uint32_t>::digits10 + 1;
    static constexpr size_t LINE_WIDTH = NUMBER_WIDTH + 1;

    void normalize_file();

    Config m_config;

    const size_t m_size;
    mutable size_t m_pos;

    bool m_file_normalized;
    const std::filesystem::path m_filename;
    mutable std::fstream m_file;
  };
} // namespace tape_sorter
