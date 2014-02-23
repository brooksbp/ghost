cmake_minimum_required(VERSION 2.8.8)
project(ghost)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (UNIX)
  add_definitions("-std=c++11")
  set(CMAKE_CXX_FLAGS "-O0 -ggdb -Wall")
endif()

set(base_SRC_FILES
  src/base/command_line.cc
  src/base/file_path.cc
  src/base/file_path_constants.cc
  src/base/strings/string_util.cc
  src/base/strings/string_util_constants.cc
  src/base/strings/string16.cc
  src/base/strings/utf_string_conversions.cc
  src/base/strings/utf_string_conversion_utils.cc
)

if (WIN32)
  list(REMOVE_ITEM base_SRC_FILES src/base/strings/string16.cc)
endif()

## GTest

set(gtest_SOURCE_DIR third_party/gtest-1.7.0)

add_subdirectory(third_party/gtest-1.7.0)
enable_testing(true)

include_directories(${gtest_SOURCE_DIR}/include)

add_executable(unit-tests
  src/base/command_line_unittest.cc
  src/base/strings/string16_unittest.cc
  ${base_SRC_FILES}
)

target_link_libraries(unit-tests gtest gtest_main)

add_test(unit-tests unit-tests)

## ghost

include_directories(src/)
include_directories(/usr/include/gstreamer-1.0)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/glib-2.0/include)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
if (WIN32)
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\include\\gdk-pixbuf-2.0\\")
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\include\\glib-2.0\\")
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\include\\gstreamer-0.10\\")
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\include\\gtk-2.0\\")
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\include\\libxml2\\")
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\lib\\glib-2.0\\include\\")
  include_directories("C:\\gstreamer-sdk\\0.10\\x86_64\\lib\\gtk-2.0\\include\\")
endif()

# Add header files so that CMake will list them in MSVC.
if (WIN32)
  file(GLOB_RECURSE INCS "src/*.h")
endif()

set(SRC_FILES
  src/main.cc
  src/track.cc
  src/library.cc
  src/audio_manager.cc
  src/main_window.cc
)

if (UNIX)
  list(APPEND SRC_FILES src/timer_posix.cc)
elseif (WIN32)
  list(APPEND SRC_FILES src/timer_win.cc)
endif()

add_executable(ghost WIN32
  ${base_SRC_FILES}
  ${SRC_FILES}
  ${INCS}
)

if (UNIX)
  target_link_libraries(ghost rt)
  target_link_libraries(ghost X11)
endif()

target_link_libraries(ghost ${CMAKE_THREAD_LIBS_INIT})


if (WIN32)
  find_library(GSTREAMER_LIB gstreamer-1.0 C:/gstreamer/1.0/x86_64/lib/)
  target_link_libraries(ghost ${GSTREAMER_LIB})
  find_library(GLIB_LIB glib-2.0 C:/gstreamer/1.0/x86_64/lib/)
  target_link_libraries(ghost ${GLIB_LIB})
  find_library(GOBJECT_LIB gobject-2.0 C:/gstreamer/1.0/x86_64/lib/)
  target_link_libraries(ghost ${GOBJECT_LIB})
endif()

if (UNIX)
  target_link_libraries(ghost gstreamer-1.0)
  target_link_libraries(ghost gobject-2.0 gmodule-2.0 gthread-2.0 glib-2.0)
endif()

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
