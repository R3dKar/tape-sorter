#pragma once

#include "tape_sorter/file_tape.hpp"
#include "tape_sorter/tape.hpp"
#include <filesystem>
#include <cstdint>
#include <memory>

namespace tape_sorter {
  class TempFileTape : public ITape<uint32_t> {
  public:
    TempFileTape(size_t n, const FileTape::Config& config = FileTape::Config{});

    size_t size() const override;
    size_t pos() const override;

    void shift_forward() const override;
    void shift_backward() const override;

    void rewind_forward() const override;
    void rewind_backward() const override;

    void write(const uint32_t& value) override;
    uint32_t read() const override;

    ~TempFileTape() override;

  private:
    const std::filesystem::path m_temp_path;
    std::unique_ptr<FileTape> m_file_tape;
  };
} // namespace tape_sorter
