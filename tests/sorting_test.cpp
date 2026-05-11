#include <algorithm>
#include <gtest/gtest.h>
#include <tape_sorter/bubble_sort.hpp>
#include <tape_sorter/file_tape.hpp>
#include <vector>

namespace fs = std::filesystem;
using namespace tape_sorter;

class SortingTest : public testing::TestWithParam<fs::path> {
protected:
  std::unique_ptr<FileTape> input_tape;
  fs::path temp_input_path;

  std::unique_ptr<FileTape> output_tape;
  fs::path temp_output_path;

  std::vector<uint32_t> sorted_data;

  void LoadData() {
    sorted_data.clear();
    std::ifstream data(temp_input_path);

    uint32_t item;
    while (data >> item) {
      sorted_data.push_back(item);
    }

    std::sort(sorted_data.begin(), sorted_data.end());
  }

  void SetUp() override {
    fs::path test_file = GetParam();

    if (!fs::exists(test_file)) {
      FAIL() << "Test file does not exists: " << test_file;
    }

    temp_input_path = std::tmpnam(nullptr);
    fs::copy_file(test_file, temp_input_path);

    LoadData();

    input_tape = std::make_unique<FileTape>(temp_input_path);

    temp_output_path = std::tmpnam(nullptr);
    output_tape = std::make_unique<FileTape>(input_tape->size(), temp_output_path);
  }

  void TearDown() override {
    input_tape.reset();
    if (fs::exists(temp_input_path)) {
      fs::remove(temp_input_path);
    }

    output_tape.reset();
    if (fs::exists(temp_output_path)) {
      fs::remove(temp_output_path);
    }
  }
};

TEST_P(SortingTest, Sort) {
  BubbleSortAlgorithm<uint32_t> bubble_sort{};

  bubble_sort.sort(*input_tape, *output_tape);

  output_tape->rewind_backward();
  for (size_t i = 0; i < output_tape->size(); i++) {
    ASSERT_EQ(output_tape->read(), sorted_data[i]) << "Wrong sorted item at position " << i << '/' << output_tape->size();

    output_tape->shift_forward();
  }
}

INSTANTIATE_TEST_SUITE_P(
  SortingTestFiles, 
  SortingTest,
  testing::Values(
    "./data/10000_random.txt",
    "./data/10000_random_normalized.txt", 
    "./data/10000_almost_sorted.txt",
    "./data/10000_reversed.txt",
    "./data/10000_sorted.txt"
  )
);
