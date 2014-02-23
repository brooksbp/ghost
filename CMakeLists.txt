cmake_minimum_required(VERSION 2.8.8)
project(ghost)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions("-std=c++0x")

#set(CMAKE_CXX_FLAGS "-O0 -ggdb -Wall")

set(base_SRC_FILES
  src/base/command_line.cc
  src/base/file_path.cc
  src/base/file_path_constants.cc
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

include_directories(src/)
include_directories(/usr/include/gstreamer-1.0)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)

# Add header files so that CMake will list them in MSVC.
if (WIN32)
  file(GLOB_RECURSE INCS "src/*.h")
endif()

add_executable(ghost WIN32
  src/main.cc
  src/track.cc
  src/library.cc
  src/audio_manager.cc
  src/main_window.cc
  src/timer.cc
  ${base_SRC_FILES}
  ${INCS}
)

target_link_libraries(ghost rt)
target_link_libraries(ghost X11)

target_link_libraries(ghost ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(ghost gstreamer-1.0)
target_link_libraries(ghost gobject-2.0 gmodule-2.0 gthread-2.0 glib-2.0)

#target_link_libraries(ghost id3)

if (WIN32)
  set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.2.1\\5.2.1\\msvc2012_64\\")
endif()

if (POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

qt5_use_modules(ghost Widgets Core)
