#include "tape_sorter/utility.hpp"
#include <random>
#include <regex>
#include <stdexcept>

namespace tape_sorter::utility {
  std::chrono::nanoseconds parse_duration(const std::string& str) {
    namespace chrono = std::chrono;

    std::regex r(R"((\d+(?:\.\d+)?)\s*(ns|us|ms|s|m|h))");
    std::smatch match;

    if (std::regex_search(str, match, r)) {
      double value = std::stod(match[1]);
      const std::string& unit = match[2];

      if (unit == "ns") return chrono::duration_cast<chrono::nanoseconds>(chrono::duration<double, std::nano>(value));
      if (unit == "us") return chrono::duration_cast<chrono::nanoseconds>(chrono::duration<double, std::micro>(value));
      if (unit == "ms") return chrono::duration_cast<chrono::nanoseconds>(chrono::duration<double, std::milli>(value));
      if (unit == "s") return chrono::duration_cast<chrono::nanoseconds>(chrono::duration<double, std::ratio<1>>(value));
      if (unit == "m") return chrono::duration_cast<chrono::nanoseconds>(chrono::duration<double, std::ratio<60>>(value));
      if (unit == "h") return chrono::duration_cast<chrono::nanoseconds>(chrono::duration<double, std::ratio<3600>>(value));
    }

    throw std::invalid_argument("Invalid duration format: " + str);
  }

  std::string random_string(size_t length) {
    static constexpr char charset[] = "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<size_t> index(0, sizeof(charset) - 2);

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; i++) {
      result += charset[index(rng)];
    }

    return result;
  }

  std::filesystem::path tempfile() {
    namespace fs = std::filesystem;

    fs::path result = fs::temp_directory_path() / (random_string(20) + ".tmp");
    while (fs::exists(result)) {
      result = fs::temp_directory_path() / (random_string(20) + ".tmp");
    }

    return result;
  }
} // namespace tape_sorter::utility
