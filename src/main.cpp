#include "tape_sorter/bubble_sort.hpp"
#include "tape_sorter/file_tape.hpp"
#include "tape_sorter/sorting_algorithm.hpp"
#include "tape_sorter/utility.hpp"
#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <toml++/toml.hpp>

int main(int argc, char** argv) {
  namespace fs = std::filesystem;
  namespace chrono = std::chrono;
  using namespace tape_sorter;

  argparse::ArgumentParser program(argv[0], TAPE_SORTER_VERSION);

  fs::path config_path = "config.toml";
  program.add_argument("-c", "--config").help("path to configuration file (default: \"config.toml\")").store_into(config_path);

  fs::path input_path;
  program.add_argument("input").help("path to input tape file").store_into(input_path);

  fs::path output_path;
  program.add_argument("output").help("path to output tape file").store_into(output_path);

  const char* custom_argv[] = {"./tape_sorter", "./tape_n.txt", "./output.txt"};
  const int custom_argc = sizeof(custom_argv) / sizeof(custom_argv[0]);

  try {
    program.parse_args(argc, argv);
    // program.parse_args(custom_argc, custom_argv);
  } catch (const std::exception& err) {
    std::cerr << err.what() << '\n';
    std::cerr << program;
    return 1;
  }

  // Check options
  // Config path
  if (!fs::exists(config_path)) {
    std::cerr << "Config file does not exist: " << config_path << '\n';
    return 1;
  } else if (!fs::is_regular_file(config_path)) {
    std::cerr << "Config file is not a file: " << config_path << '\n';
    return 1;
  }

  // Input path
  if (!fs::exists(input_path)) {
    std::cerr << "Input file does not exist: " << input_path << '\n';
    return 1;
  } else if (!fs::is_regular_file(input_path)) {
    std::cerr << "Input file is not a file: " << input_path << '\n';
    return 1;
  }

  // Output path
  const auto output_parent_path = output_path.parent_path();
  if (!output_parent_path.empty() && (!fs::exists(output_parent_path) || !fs::is_directory(output_parent_path))) {
    std::cerr << "Output file directory does not exists: " << output_parent_path << '\n';
    return 1;
  } else if (fs::exists(output_path) && !fs::is_regular_file(output_path)) {
    std::cerr << "Output file is not a file: " << output_path << '\n';
    return 1;
  }

  const auto toml_config = toml::parse_file(config_path.string());
  FileTape::Config tape_config{};

  try {
    const auto latencies_config = toml_config["latencies"];
    tape_config.shift_latency = utility::parse_duration(latencies_config["shift"].value_or(""));
    tape_config.rewind_latency = utility::parse_duration(latencies_config["rewind"].value_or(""));
    tape_config.read_latency = utility::parse_duration(latencies_config["read"].value_or(""));
    tape_config.write_latency = utility::parse_duration(latencies_config["write"].value_or(""));
  } catch (const std::runtime_error& err) {
    std::cerr << err.what() << '\n';
    return 1;
  }

  FileTape input_tape(input_path, tape_config);
  FileTape output_tape(input_tape.size(), output_path, tape_config);

  std::unique_ptr<ISortingAlgorithm<uint32_t>> sorter = std::make_unique<BubbleSortAlgorithm<uint32_t>>();

  sorter->sort(input_tape, output_tape);

  return 0;
}
