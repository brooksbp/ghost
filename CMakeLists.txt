cmake_minimum_required(VERSION 2.8.8)
project(ghost)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions("-std=c++0x")

find_package(Qt5Widgets REQUIRED)

add_executable(ghost
  src/main.cc
  src/command_line.cc
  src/file_path.cc
  src/string_util.cc
  src/string_util_constants.cc
)

qt5_use_modules(ghost Widgets)
