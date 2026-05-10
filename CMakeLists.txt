cmake_minimum_required(VERSION 3.30)
project(tape_sorter LANGUAGES CXX VERSION 1.0.0)

option(BUILD_TESTS "Build tests executable" ${PROJECT_IS_TOP_LEVEL})

# Dependencies
include(cmake/AddGitSubmodule.cmake)
add_git_submodule(libs/tomlplusplus)
add_git_submodule(libs/argparse)

if (BUILD_TESTS)
  add_git_submodule(libs/googletest)
endif()

# Core
file(GLOB_RECURSE PROJECT_CORE_SOURCES CONFIGURE_DEPENDS src/tape_sorter/*.cpp)
add_library(${PROJECT_NAME}_core ${PROJECT_CORE_SOURCES})
target_include_directories(${PROJECT_NAME}_core PUBLIC src)
target_compile_features(${PROJECT_NAME}_core PUBLIC cxx_std_20)

if (WIN32)
  target_compile_definitions(${PROJECT_NAME}_core PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Executable
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_core tomlplusplus::tomlplusplus argparse::argparse)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/config/config.toml $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Tests
if (BUILD_TESTS)
  enable_testing()

  file(GLOB_RECURSE PROJECT_TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
  add_executable(${PROJECT_NAME}_test ${PROJECT_TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME}_core GTest::gtest GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(${PROJECT_NAME}_test)
endif()
