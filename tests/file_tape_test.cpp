#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <memory>
#include <tape_sorter/file_tape.hpp>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;
using namespace tape_sorter;

class FileTapeTest : public testing::TestWithParam<std::tuple<fs::path, bool>> {
protected:
  std::unique_ptr<FileTape> file_tape;
  fs::path temp_copy_path;

  std::vector<uint32_t> tape_data;

  void LoadData() {
    tape_data.clear();
    std::ifstream data(temp_copy_path);

    uint32_t item;
    while (data >> item) {
      tape_data.push_back(item);
    }
  }

  void SetUp() override {
    fs::path test_file = std::get<0>(GetParam());
    bool do_normalize = std::get<1>(GetParam());

    if (!fs::exists(test_file)) {
      FAIL() << "Test file does not exists: " << test_file;
    }

    temp_copy_path = std::tmpnam(nullptr);
    fs::copy_file(test_file, temp_copy_path);

    LoadData();

    file_tape = std::make_unique<FileTape>(temp_copy_path);

    if (do_normalize) {
      file_tape->write(tape_data[file_tape->pos()]);
    }
  }

  void TearDown() override {
    file_tape.reset();

    if (fs::exists(temp_copy_path)) fs::remove(temp_copy_path);
  }
};

TEST_P(FileTapeTest, CorrectSize) {
  EXPECT_EQ(file_tape->size(), tape_data.size());
}

TEST_P(FileTapeTest, RewindBackwardAtStart) {
  file_tape->rewind_backward();

  file_tape->rewind_backward();

  EXPECT_EQ(file_tape->pos(), 0);
}

TEST_P(FileTapeTest, RewindBackwardAtEnd) {
  file_tape->rewind_forward();

  file_tape->rewind_backward();

  EXPECT_EQ(file_tape->pos(), 0);
}

TEST_P(FileTapeTest, RewindForwardAtStart) {
  file_tape->rewind_backward();

  file_tape->rewind_forward();

  EXPECT_EQ(file_tape->pos(), file_tape->size() - 1);
}

TEST_P(FileTapeTest, RewindForwardAtEnd) {
  file_tape->rewind_forward();

  file_tape->rewind_forward();

  EXPECT_EQ(file_tape->pos(), file_tape->size() - 1);
}

TEST_P(FileTapeTest, ShiftForwardAtStart) {
  file_tape->rewind_backward();

  file_tape->shift_forward();

  EXPECT_EQ(file_tape->pos(), 1);
}

TEST_P(FileTapeTest, ShiftForwardAtEnd) {
  file_tape->rewind_forward();

  file_tape->shift_forward();

  EXPECT_EQ(file_tape->pos(), file_tape->size() - 1);
}

TEST_P(FileTapeTest, ShiftBackwardAtStart) {
  file_tape->rewind_backward();

  file_tape->shift_backward();

  EXPECT_EQ(file_tape->pos(), 0);
}

TEST_P(FileTapeTest, ShiftBackwardAtEnd) {
  file_tape->rewind_forward();

  file_tape->shift_backward();

  EXPECT_EQ(file_tape->pos(), file_tape->size() - 2);
}

TEST_P(FileTapeTest, ConsecutiveRead) {
  file_tape->rewind_backward();

  for (size_t i = 0; i < file_tape->size(); i++) {
    ASSERT_EQ(file_tape->pos(), i);
    ASSERT_EQ(file_tape->read(), tape_data[i]) << "Item mismatch at position " << i << '/' << file_tape->size();

    file_tape->shift_forward();
  }
}

TEST_P(FileTapeTest, ReverseConsecutiveRead) {
  file_tape->rewind_forward();

  for (size_t i = file_tape->size(); i > 0; i--) {
    ASSERT_EQ(file_tape->pos(), i - 1);
    ASSERT_EQ(file_tape->read(), tape_data[i - 1]) << "Item mismatch at position " << i - 1 << '/' << file_tape->size();

    file_tape->shift_backward();
  }
}

TEST_P(FileTapeTest, ConsecutiveWrite) {
  file_tape->rewind_backward();

  for (size_t i = 0; i < file_tape->size(); i++) {
    ASSERT_EQ(file_tape->pos(), i);

    file_tape->write(123);

    ASSERT_EQ(file_tape->read(), 123) << "Wrong item written at position " << i << '/' << file_tape->size();

    file_tape->shift_forward();
  }
}

TEST_P(FileTapeTest, ReverseConsecutiveWrite) {
  file_tape->rewind_forward();

  for (size_t i = file_tape->size(); i > 0; i--) {
    ASSERT_EQ(file_tape->pos(), i - 1);

    file_tape->write(123);

    ASSERT_EQ(file_tape->read(), 123) << "Wrong item written at position " << i - 1 << '/' << file_tape->size();

    file_tape->shift_backward();
  }
}

INSTANTIATE_TEST_SUITE_P(
  TapeTestFiles, 
  FileTapeTest,
  testing::Combine(
    testing::Values("./data/10000_random.txt", "./data/10000_random_normalized.txt"),
    testing::Bool()
  )
);
