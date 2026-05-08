cmake_minimum_required(VERSION 3.30)
project(tape_sorter LANGUAGES CXX)

include(cmake/AddGitSubmodule.cmake)

add_git_submodule(libs/tomlplusplus)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
