cmake_minimum_required(VERSION 2.8.8)
project(ghost)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions("-std=c++0x")

set(base_SRC_FILES
  src/base/command_line.cc
  src/base/file_path.cc
  src/base/string_util.cc
  src/base/string_util_constants.cc
)

## GTest

set(gtest_SOURCE_DIR third_party/gtest-1.7.0)

add_subdirectory(third_party/gtest-1.7.0)
enable_testing(true)

include_directories(${gtest_SOURCE_DIR}/include)

add_executable(unit-tests
  src/base/command_line_unittest.cc
  ${base_SRC_FILES}
)

target_link_libraries(unit-tests gtest gtest_main)

add_test(unit-tests unit-tests)

## ghost

include_directories(src/base/)

add_executable(ghost
  src/main.cc
  ${base_SRC_FILES}
)

find_package(Qt5Widgets REQUIRED)
qt5_use_modules(ghost Widgets)
