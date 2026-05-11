#include <algorithm>
#include <gtest/gtest.h>
#include <stdexcept>
#include <tape_sorter/file_tape.hpp>
#include <tape_sorter/temp_file_tape.hpp>
#include <tape_sorter/tim_sort.hpp>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;
using namespace tape_sorter;

class SortingTest : public testing::TestWithParam<std::tuple<fs::path, size_t>> {
protected:
  std::unique_ptr<FileTape> input_tape;
  fs::path temp_input_path;

  std::unique_ptr<TempFileTape> output_tape;

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
    fs::path test_file = std::get<0>(GetParam());

    if (!fs::exists(test_file)) {
      FAIL() << "Test file does not exists: " << test_file;
    }

    temp_input_path = std::tmpnam(nullptr);
    fs::copy_file(test_file, temp_input_path);

    LoadData();

    input_tape = std::make_unique<FileTape>(temp_input_path);
    output_tape = std::make_unique<TempFileTape>(input_tape->size());
  }

  void TearDown() override {
    input_tape.reset();
    if (fs::exists(temp_input_path)) {
      fs::remove(temp_input_path);
    }
  }
};

TEST_P(SortingTest, Sort) {
  const size_t max_ram_elements = std::get<1>(GetParam());
  TimSortAlgorithm tim_sort({max_ram_elements});

  tim_sort.sort(*input_tape, *output_tape);

  output_tape->rewind_backward();
  for (size_t i = 0; i < output_tape->size(); i++) {
    ASSERT_EQ(output_tape->read(), sorted_data[i]) << "Wrong sorted item at position " << i << '/' << output_tape->size();

    output_tape->shift_forward();
  }
}

TEST(ConfigTest, InvalidValueThrows) {
  EXPECT_THROW({ 
    TimSortAlgorithm tim_sort({0}); 
  }, std::invalid_argument);
}

INSTANTIATE_TEST_SUITE_P(
  LargeSortingTestFiles, 
  SortingTest,
  testing::Combine(
    testing::Values(
      "./data/10000_random.txt",
      "./data/10000_random_normalized.txt",
      "./data/10000_almost_sorted.txt",
      "./data/10000_reversed.txt",
      "./data/10000_sorted.txt",
      "./data/empty.txt"
    ),
    testing::Values(123, 10000, 13000)
  )
);

INSTANTIATE_TEST_SUITE_P(
  SmallSortingTestFiles, 
  SortingTest,
  testing::Combine(
    testing::Values(
      "./data/1000_random.txt",
      "./data/1000_random_normalized.txt",
      "./data/1000_almost_sorted.txt", 
      "./data/1000_reversed.txt", 
      "./data/1000_sorted.txt",
      "./data/empty.txt"
    ),
    testing::Values(1, 10, 13)
  )
);
