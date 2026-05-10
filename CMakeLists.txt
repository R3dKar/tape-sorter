cmake_minimum_required(VERSION 3.30)
project(tape_sorter LANGUAGES CXX VERSION 1.0.0)

include(cmake/AddGitSubmodule.cmake)
add_git_submodule(libs/tomlplusplus)
add_git_submodule(libs/argparse)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE tomlplusplus::tomlplusplus argparse::argparse)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/config/config.toml $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

if (WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
