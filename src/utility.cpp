#include "utility.hpp"
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

    throw std::runtime_error("Invalid duration format: " + str);
  }
} // namespace tape_sorter::utility
