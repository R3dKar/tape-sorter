#include "tape_sorter/file_tape.hpp"
#include "tape_sorter/sorting_algorithm.hpp"
#include "tape_sorter/tim_sort.hpp"
#include "tape_sorter/utility.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string_view>
#include <toml++/toml.hpp>

namespace fs = std::filesystem;
namespace chrono = std::chrono;
using namespace tape_sorter;

namespace {
  template <typename T>
  void check_field(const T& node, std::string_view field) {
    if (!node[field]) {
      std::cerr << std::format("Field \"{}\" is missing in configuration file\n", field);
      std::exit(1);
    }
  }
} // namespace

int main(int argc, char** argv) {
  argparse::ArgumentParser program(argv[0], TAPE_SORTER_VERSION);

  fs::path config_path = "config.toml";
  program.add_argument("-c", "--config").help("path to configuration file (default: \"config.toml\")").store_into(config_path);

  fs::path input_path;
  program.add_argument("input").help("path to input tape file").store_into(input_path);

  fs::path output_path;
  program.add_argument("output").help("path to output tape file").store_into(output_path);

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << '\n';
    std::cerr << program;
    return 1;
  }

  // Check options
  // Config path
  if (!fs::exists(config_path)) {
    std::cerr << std::format("Config file does not exist: {}\n", config_path.string());
    return 1;
  } else if (!fs::is_regular_file(config_path)) {
    std::cerr << std::format("Config file is not a file: {}\n", config_path.string());
    return 1;
  }

  // Input path
  if (!fs::exists(input_path)) {
    std::cerr << std::format("Input file does not exist: {}\n", input_path.string());
    return 1;
  } else if (!fs::is_regular_file(input_path)) {
    std::cerr << std::format("Input file is not a file: {}\n", input_path.string());
    return 1;
  }

  // Output path
  const auto output_parent_path = output_path.parent_path();
  if (!output_parent_path.empty() && (!fs::exists(output_parent_path) || !fs::is_directory(output_parent_path))) {
    std::cerr << std::format("Output file directory does not exist: {}\n", output_parent_path.string());
    return 1;
  } else if (fs::exists(output_path) && !fs::is_regular_file(output_path)) {
    std::cerr << std::format("Output file is not a file: {}\n", output_path.string());
    return 1;
  }

  // Config
  toml::table toml_config;
  FileTape::Config tape_config{};
  TimSortAlgorithm::Config sort_config{};

  try {
    toml_config = toml::parse_file(config_path.string());

  } catch (const toml::parse_error& err) {
    std::cerr << std::format("Config parsing failed: {}\n", err.what());
    return 1;
  }

  // Latencies validation
  try {
    ::check_field(toml_config, "latencies");
    const auto latencies_config = toml_config["latencies"];

    ::check_field(latencies_config, "shift");
    ::check_field(latencies_config, "rewind");
    ::check_field(latencies_config, "read");
    ::check_field(latencies_config, "write");

    tape_config.shift_latency = utility::parse_duration(latencies_config["shift"].value_or(""));
    tape_config.rewind_latency = utility::parse_duration(latencies_config["rewind"].value_or(""));
    tape_config.read_latency = utility::parse_duration(latencies_config["read"].value_or(""));
    tape_config.write_latency = utility::parse_duration(latencies_config["write"].value_or(""));
  } catch (const std::invalid_argument& err) {
    std::cerr << err.what() << '\n';
    return 1;
  }

  // Max ram elements validation
  ::check_field(toml_config, "max_ram_elements");
  sort_config.max_ram_elements = toml_config["max_ram_elements"].value_or(0);

  if (sort_config.max_ram_elements == 0) {
    std::cerr << "Max ram elements must be at least 1\n";
    return 1;
  }

  // Core logic
  FileTape input_tape(input_path, tape_config);
  FileTape output_tape(input_tape.size(), output_path, tape_config);

  std::unique_ptr<ISortingAlgorithm<uint32_t>> sorter = std::make_unique<TimSortAlgorithm>(sort_config);

  sorter->sort(input_tape, output_tape);

  return 0;
}
