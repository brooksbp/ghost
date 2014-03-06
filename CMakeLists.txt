# Copyright 2014 Ghost Authors. All rights reserved.
# Use of this source code is governed by a ALv2 license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 2.8.8)
project(ghost)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (UNIX)
  add_definitions("-std=c++11")
  set(CMAKE_CXX_FLAGS "-O0 -ggdb -Wall")
endif()
if (MSVC)
  add_definitions(-DUNICODE -D_UNICODE)
  set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE        "/MT /O2 /Ob2 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")
endif()

set(base_SRC_FILES
  src/base/command_line.cc
  src/base/file_util.cc
  src/base/files/file_enumerator.cc
  src/base/files/file_path.cc
  src/base/files/file_path_constants.cc
  src/base/logging.cc
  src/base/strings/nullable_string16.cc
  src/base/strings/string_util.cc
  src/base/strings/string_util_constants.cc
  src/base/strings/string_piece.cc
  src/base/strings/string_number_conversions.cc
  src/base/strings/string_split.cc
  src/base/strings/utf_string_conversions.cc
  src/base/strings/utf_string_conversion_utils.cc
  src/base/third_party/icu/icu_utf.cc
  src/base/vlog.cc
)
if (WIN32)
  list(APPEND base_SRC_FILES src/base/file_util_win.cc)
  list(APPEND base_SRC_FILES src/base/files/file_enumerator_win.cc)
  list(APPEND base_SRC_FILES src/base/synchronization/lock_impl_win.cc)
  list(APPEND base_SRC_FILES src/base/win/windows_version.cc)
endif()
if (UNIX)
  list(APPEND base_SRC_FILES src/base/file_util_posix.cc)
  list(APPEND base_SRC_FILES src/base/files/file_enumerator_posix.cc)
  list(APPEND base_SRC_FILES src/base/strings/string16.cc)
  list(APPEND base_SRC_FILES src/base/synchronization/lock_impl_posix.cc)
endif()

## Google Test

set(gtest_SOURCE_DIR third_party/googletest)

add_subdirectory(third_party/googletest)
enable_testing(true)

include_directories(${gtest_SOURCE_DIR}/include)

set(base_UT_FILES
  src/base/vlog_unittest.cc
  src/base/command_line_unittest.cc
  src/base/strings/string16_unittest.cc
  src/base/strings/string_piece_unittest.cc
  src/base/strings/string_number_conversions_unittest.cc
  src/base/strings/nullable_string16_unittest.cc
  src/base/strings/utf_string_conversions_unittest.cc
)
if (UNIX)
  list(APPEND base_UT_FILES src/base/scoped_clear_errno_unittest.cc)
endif()

add_executable(unit-tests
  src/playlist_pls.cc
  src/playlist_pls_unittest.cc

  ${base_SRC_FILES}
  ${base_UT_FILES}
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
  src/playlist_pls.cc
  src/g_own_ptr.cc
  src/audio_manager.cc
  src/main_window.cc
  src/player.cc
)

if (UNIX)
  list(APPEND SRC_FILES src/timer_posix.cc)
elseif (WIN32)
  list(APPEND SRC_FILES src/timer_win.cc)
endif()

add_executable(ghost
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
