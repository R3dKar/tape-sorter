#include "tape_sorter/temp_file_tape.hpp"
#include <cstdio>

namespace tape_sorter {
  TempFileTape::TempFileTape(size_t n, const FileTape::Config& config)
      : m_temp_path(std::tmpnam(nullptr)), m_file_tape(std::make_unique<FileTape>(n, m_temp_path, config)) {
  }

  size_t TempFileTape::size() const {
    return m_file_tape->size();
  }

  size_t TempFileTape::pos() const {
    return m_file_tape->pos();
  }

  void TempFileTape::shift_forward() const {
    m_file_tape->shift_forward();
  }

  void TempFileTape::shift_backward() const {
    m_file_tape->shift_backward();
  }

  void TempFileTape::rewind_forward() const {
    m_file_tape->rewind_forward();
  }

  void TempFileTape::rewind_backward() const {
    m_file_tape->rewind_backward();
  }

  void TempFileTape::write(const uint32_t& value) {
    m_file_tape->write(value);
  }

  uint32_t TempFileTape::read() const {
    return m_file_tape->read();
  }

  TempFileTape::~TempFileTape() {
    namespace fs = std::filesystem;

    m_file_tape.reset();

    if (fs::exists(m_temp_path)) {
      fs::remove(m_temp_path);
    }
  }
} // namespace tape_sorter
