# Copyright 2014 Ghost Authors. All rights reserved.
# Use of this source code is governed by a ALv2 license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 2.8.8)
project(ghost)

# message("PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})

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

################################################################################
#
# Chromium

set(chromium_SOURCE_DIR third_party/chromium)

set(chromium_SOURCE_FILES
  ${chromium_SOURCE_DIR}/base/base_switches.cc
  ${chromium_SOURCE_DIR}/base/command_line.cc
  ${chromium_SOURCE_DIR}/base/file_util.cc
  ${chromium_SOURCE_DIR}/base/files/file_enumerator.cc
  ${chromium_SOURCE_DIR}/base/files/file_path.cc
  ${chromium_SOURCE_DIR}/base/files/file_path_constants.cc
  ${chromium_SOURCE_DIR}/base/logging.cc
  ${chromium_SOURCE_DIR}/base/strings/nullable_string16.cc
  ${chromium_SOURCE_DIR}/base/strings/string_util.cc
  ${chromium_SOURCE_DIR}/base/strings/string_util_constants.cc
  ${chromium_SOURCE_DIR}/base/strings/string_piece.cc
  ${chromium_SOURCE_DIR}/base/strings/string_number_conversions.cc
  ${chromium_SOURCE_DIR}/base/strings/string_split.cc
  ${chromium_SOURCE_DIR}/base/strings/stringprintf.cc
  ${chromium_SOURCE_DIR}/base/strings/utf_string_conversions.cc
  ${chromium_SOURCE_DIR}/base/strings/utf_string_conversion_utils.cc
  ${chromium_SOURCE_DIR}/base/third_party/icu/icu_utf.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_id_manager.cc
  ${chromium_SOURCE_DIR}/base/vlog.cc
)
if (WIN32)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/file_util_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/files/file_enumerator_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/lock_impl_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/win/windows_version.cc)
endif()
if (UNIX)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/file_util_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/files/file_enumerator_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/safe_strerror_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/strings/string16.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/lock_impl_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/platform_thread_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/platform_thread_posix.cc)
endif()

add_library(chromium OBJECT ${chromium_SOURCE_FILES})
add_library(chromium-static STATIC $<TARGET_OBJECTS:chromium>)
#add_library(chromium-shared SHARED $<TARGET_OBJECTS:chromium>)

set(chromium_UNITTEST
  ${chromium_SOURCE_DIR}/base/scoped_clear_errno_unittest.cc
  ${chromium_SOURCE_DIR}/base/command_line_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/string_piece_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/string_number_conversions_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/string16_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/nullable_string16_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/utf_string_conversions_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/platform_thread_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_id_name_maanger_unittest.cc
  ${chromium_SOURCE_DIR}/base/vlog_unittest.cc
)
if (UNIX)
  list(APPEND chromium_UNITTEST ${chromium_SOURCE_DIR}/base/scoped_clear_errno_unittest.cc)
endif()

################################################################################
#
# Google Test

set(gtest_SOURCE_DIR third_party/googletest)

add_subdirectory(third_party/googletest)

enable_testing(true)

add_executable(unit-tests
  src/playlist_pls.cc
  src/playlist_pls_unittest.cc

  ${chromium_UNITTEST}
)

target_link_libraries(unit-tests chromium-static)
target_link_libraries(unit-tests gtest gtest_main)

add_test(unit-tests unit-tests)

################################################################################
#
# Ghost

include_directories(${chromium_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include)

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

# Add header files so they are visible in MSVC.
if (WIN32)
  file(GLOB_RECURSE INCS "src/*.h")
endif()

set(SRC_FILES
  src/main.cc
  src/track.cc
  src/library.cc
  src/playlist_pls.cc
  src/g_own_ptr.cc
  src/gst_player.cc
  src/main_window.cc
  src/player.cc
)

if (UNIX)
  list(APPEND SRC_FILES src/timer_posix.cc)
elseif (WIN32)
  list(APPEND SRC_FILES src/timer_win.cc)
endif()

add_executable(ghost
  ${SRC_FILES}
  ${INCS}
)

target_link_libraries(ghost chromium-static)

if (UNIX)
  target_link_libraries(ghost rt)
  target_link_libraries(ghost X11)
endif()

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

target_link_libraries(ghost pthread)

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
