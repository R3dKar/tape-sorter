cmake_minimum_required(VERSION 3.30)
project(tape_sorter LANGUAGES CXX)

include(cmake/AddGitSubmodule.cmake)
add_git_submodule(libs/tomlplusplus)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE tomlplusplus::tomlplusplus)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if (WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
