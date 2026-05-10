#include "file_tape.hpp"
#include "utility.hpp"
#include <cctype>
#include <fstream>
#include <thread>

namespace tape_sorter {
  FileTape::FileTape(const std::filesystem::path& filename, const Config& config)
      : m_size(utility::count_file_items<uint32_t>(filename)), m_config(config), m_pos(0), m_file_normalized(false),
        m_file(filename, std::ios::in | std::ios::out | std::ios::binary), m_filename(filename) {
    char symbol;
    m_file >> symbol;
    m_file.seekg(-1, std::ios::cur);
  }

  FileTape::FileTape(size_t n, const std::filesystem::path& filename, const Config& config)
      : m_size(n), m_config(config), m_pos(0), m_file_normalized(true),
        m_file(filename, std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary), m_filename(filename) {
    for (size_t i = 0; i < m_size; i++) {
      m_file << std::format("{:<{}}\n", 0, NUMBER_WIDTH);
    }

    m_file.seekg(0, std::ios::beg);
  }

  size_t FileTape::size() const {
    return m_size;
  }

  size_t FileTape::pos() const {
    return m_pos;
  }

  void FileTape::shift_forward() const {
    if (pos() + 1 >= size()) return;

    if (m_file_normalized) {
      m_file.seekg(LINE_WIDTH, std::ios::cur);
    } else {
      uint32_t current_item;
      m_file >> current_item;

      char symbol;
      m_file >> symbol;

      m_file.seekg(-1, std::ios::cur);
    }

    std::this_thread::sleep_for(m_config.shift_latency);
    m_pos++;
  }

  void FileTape::shift_backward() const {
    if (pos() == 0) return;

    if (m_file_normalized) {
      m_file.seekg(-static_cast<std::streamoff>(LINE_WIDTH), std::ios::cur);
    } else {
      m_file.seekg(-1, std::ios::cur);
      char symbol = m_file.peek();

      while (std::isspace(symbol)) {
        m_file.seekg(-1, std::ios::cur);
        symbol = m_file.peek();
      }

      while (!std::isspace(symbol)) {
        m_file.seekg(-1, std::ios::cur);
        symbol = m_file.peek();
      }

      m_file.seekg(1, std::ios::cur);
    }

    std::this_thread::sleep_for(m_config.shift_latency);
    m_pos--;
  }

  void FileTape::rewind_forward() const {
    if (pos() + 1 >= size()) return;

    if (m_file_normalized) {
      m_file.seekg((size() - 1) * LINE_WIDTH, std::ios::beg);
    } else {
      m_file.seekg(0, std::ios::end);

      char symbol = m_file.peek();

      while (std::isspace(symbol)) {
        m_file.seekg(-1, std::ios::cur);
        symbol = m_file.peek();
      }

      while (!std::isspace(symbol)) {
        m_file.seekg(-1, std::ios::cur);
        symbol = m_file.peek();
      }

      m_file.seekg(1, std::ios::cur);
    }

    std::this_thread::sleep_for(m_config.rewind_latency);
    m_pos = m_size - 1;
  }

  void FileTape::rewind_backward() const {
    if (pos() == 0) return;

    if (m_file_normalized) {
      m_file.seekg(0, std::ios::beg);
    } else {
      m_file.seekg(0, std::ios::beg);

      char symbol;
      m_file >> symbol;

      m_file.seekg(-1, std::ios::cur);
    }

    std::this_thread::sleep_for(m_config.rewind_latency);
    m_pos = 0;
  }

  void FileTape::write(const uint32_t& value) {
    if (!m_file_normalized) normalize_file();

    const auto pos = m_file.tellg();

    m_file.seekp(pos);
    m_file << std::format("{:<{}}", value, NUMBER_WIDTH);
    m_file.flush();

    // yeah, this have to be synced too despite no read was made
    m_file.seekg(pos);

    std::this_thread::sleep_for(m_config.write_latency);
  }

  uint32_t FileTape::read() const {
    const auto pos = m_file.tellg();

    uint32_t result;
    m_file >> result;

    m_file.seekg(pos);

    std::this_thread::sleep_for(m_config.read_latency);
    return result;
  }

  void FileTape::normalize_file() {
    namespace fs = std::filesystem;

    if (m_file_normalized) return;

    fs::path temp_file_path = std::tmpnam(nullptr);
    std::fstream temp_file(temp_file_path, std::ios::in | std::ios::out | std::ios::trunc);

    m_file.seekg(0, std::ios::beg);

    for (size_t i = 0; i < m_size; i++) {
      uint32_t item;
      m_file >> item;
      temp_file << std::format("{:<{}}\n", item, NUMBER_WIDTH);
    }

    fs::resize_file(m_filename, 0);
    m_file.clear();
    m_file.seekp(0, std::ios::beg);

    temp_file.flush();
    temp_file.seekg(0, std::ios::beg);

    m_file << temp_file.rdbuf();
    m_file.flush();

    temp_file.close();

    try {
      fs::remove(temp_file_path);
    } catch (fs::filesystem_error) {
    }

    m_file.seekg(m_pos * LINE_WIDTH, std::ios::beg);
    m_file_normalized = true;
  }

} // namespace tape_sorter
