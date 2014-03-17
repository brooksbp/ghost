# Copyright 2014 Ghost Authors. All rights reserved.
# Use of this source code is governed by a ALv2 license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 2.8.8)
project(ghost)

# message("PROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
  ${chromium_SOURCE_DIR}/base/allocator/type_profiler.cc
  ${chromium_SOURCE_DIR}/base/allocator/type_profiler_control.cc
  ${chromium_SOURCE_DIR}/base/at_exit.cc
  #${chromium_SOURCE_DIR}/base/base64.cc
  ${chromium_SOURCE_DIR}/base/base_paths.cc
  ${chromium_SOURCE_DIR}/base/base_switches.cc
  ${chromium_SOURCE_DIR}/base/big_endian.cc
  ${chromium_SOURCE_DIR}/base/bind_helpers.cc
  ${chromium_SOURCE_DIR}/base/build_time.cc
  ${chromium_SOURCE_DIR}/base/command_line.cc
  ${chromium_SOURCE_DIR}/base/callback_helpers.cc
  ${chromium_SOURCE_DIR}/base/callback_internal.cc
  ${chromium_SOURCE_DIR}/base/debug/alias.cc
  ${chromium_SOURCE_DIR}/base/debug/crash_logging.cc
  ${chromium_SOURCE_DIR}/base/debug/debugger.cc
  ${chromium_SOURCE_DIR}/base/debug/stack_trace.cc
  ${chromium_SOURCE_DIR}/base/debug/trace_event_impl.cc
  ${chromium_SOURCE_DIR}/base/debug/trace_event_impl_constants.cc
  ${chromium_SOURCE_DIR}/base/debug/trace_event_memory.cc
  ${chromium_SOURCE_DIR}/base/debug/trace_event_synthetic_delay.cc
  ${chromium_SOURCE_DIR}/base/debug/trace_event_system_stats_monitor.cc
  ${chromium_SOURCE_DIR}/base/environment.cc
  ${chromium_SOURCE_DIR}/base/file_util.cc
  ${chromium_SOURCE_DIR}/base/files/file.cc
  ${chromium_SOURCE_DIR}/base/files/file_enumerator.cc
  ${chromium_SOURCE_DIR}/base/files/file_path.cc
  ${chromium_SOURCE_DIR}/base/files/file_path_constants.cc
  ${chromium_SOURCE_DIR}/base/files/file_util_proxy.cc
  ${chromium_SOURCE_DIR}/base/files/scoped_temp_dir.cc
  ${chromium_SOURCE_DIR}/base/json/json_file_value_serializer.cc
  ${chromium_SOURCE_DIR}/base/json/json_parser.cc
  ${chromium_SOURCE_DIR}/base/json/json_reader.cc
  ${chromium_SOURCE_DIR}/base/json/json_string_value_serializer.cc
  ${chromium_SOURCE_DIR}/base/json/json_writer.cc
  ${chromium_SOURCE_DIR}/base/json/string_escape.cc
  ${chromium_SOURCE_DIR}/base/lazy_instance.cc
  ${chromium_SOURCE_DIR}/base/location.cc
  ${chromium_SOURCE_DIR}/base/logging.cc
  ${chromium_SOURCE_DIR}/base/memory/aligned_memory.cc
  ${chromium_SOURCE_DIR}/base/memory/ref_counted.cc
  ${chromium_SOURCE_DIR}/base/memory/ref_counted_memory.cc
  ${chromium_SOURCE_DIR}/base/memory/singleton.cc
  ${chromium_SOURCE_DIR}/base/memory/weak_ptr.cc
  ${chromium_SOURCE_DIR}/base/message_loop/incoming_task_queue.cc
  ${chromium_SOURCE_DIR}/base/message_loop/message_loop.cc
  ${chromium_SOURCE_DIR}/base/message_loop/message_loop_proxy.cc
  ${chromium_SOURCE_DIR}/base/message_loop/message_loop_proxy_impl.cc
  ${chromium_SOURCE_DIR}/base/message_loop/message_pump.cc
  ${chromium_SOURCE_DIR}/base/message_loop/message_pump_default.cc
  ${chromium_SOURCE_DIR}/base/metrics/bucket_ranges.cc
  ${chromium_SOURCE_DIR}/base/metrics/field_trial.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_base.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_delta_serialization.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_samples.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_snapshot_manager.cc
  ${chromium_SOURCE_DIR}/base/metrics/sample_map.cc
  ${chromium_SOURCE_DIR}/base/metrics/sample_vector.cc
  ${chromium_SOURCE_DIR}/base/metrics/sparse_histogram.cc
  ${chromium_SOURCE_DIR}/base/metrics/statistics_recorder.cc
  ${chromium_SOURCE_DIR}/base/metrics/stats_counters.cc
  ${chromium_SOURCE_DIR}/base/metrics/stats_table.cc
  ${chromium_SOURCE_DIR}/base/metrics/user_metrics.cc
  ${chromium_SOURCE_DIR}/base/path_service.cc
  ${chromium_SOURCE_DIR}/base/pending_task.cc
  ${chromium_SOURCE_DIR}/base/pickle.cc
  ${chromium_SOURCE_DIR}/base/platform_file.cc
  ${chromium_SOURCE_DIR}/base/process/kill.cc
  ${chromium_SOURCE_DIR}/base/process/launch.cc
  ${chromium_SOURCE_DIR}/base/process/process_iterator.cc
  ${chromium_SOURCE_DIR}/base/process/process_metrics.cc
  ${chromium_SOURCE_DIR}/base/profiler/alternate_timer.cc
  ${chromium_SOURCE_DIR}/base/profiler/tracked_time.cc
  ${chromium_SOURCE_DIR}/base/rand_util.cc
  ${chromium_SOURCE_DIR}/base/run_loop.cc
  ${chromium_SOURCE_DIR}/base/sequence_checker_impl.cc
  ${chromium_SOURCE_DIR}/base/sequenced_task_runner.cc
  ${chromium_SOURCE_DIR}/base/strings/nullable_string16.cc
  ${chromium_SOURCE_DIR}/base/strings/string_util.cc
  ${chromium_SOURCE_DIR}/base/strings/string_util_constants.cc
  ${chromium_SOURCE_DIR}/base/strings/string_piece.cc
  ${chromium_SOURCE_DIR}/base/strings/string_number_conversions.cc
  ${chromium_SOURCE_DIR}/base/strings/string_split.cc
  ${chromium_SOURCE_DIR}/base/strings/stringprintf.cc
  ${chromium_SOURCE_DIR}/base/strings/utf_string_conversions.cc
  ${chromium_SOURCE_DIR}/base/strings/utf_string_conversion_utils.cc
  ${chromium_SOURCE_DIR}/base/synchronization/cancellation_flag.cc
  ${chromium_SOURCE_DIR}/base/synchronization/lock.cc
  ${chromium_SOURCE_DIR}/base/sys_info.cc
  ${chromium_SOURCE_DIR}/base/task_runner.cc
  ${chromium_SOURCE_DIR}/base/task/cancelable_task_tracker.cc
  ${chromium_SOURCE_DIR}/base/thread_task_runner_handle.cc
  ${chromium_SOURCE_DIR}/base/third_party/icu/icu_utf.cc
  ${chromium_SOURCE_DIR}/base/third_party/dmg_fp/dtoa.cc
  #${chromium_SOURCE_DIR}/base/third_party/dmg_fp/dtoa_wrapper.cc
  ${chromium_SOURCE_DIR}/base/third_party/dmg_fp/g_fmt.cc
  ${chromium_SOURCE_DIR}/base/third_party/dynamic_annotations/dynamic_annotations.c
  ${chromium_SOURCE_DIR}/base/third_party/nspr/prtime.cc
  ${chromium_SOURCE_DIR}/base/time/clock.cc
  ${chromium_SOURCE_DIR}/base/time/default_clock.cc
  ${chromium_SOURCE_DIR}/base/time/default_tick_clock.cc
  ${chromium_SOURCE_DIR}/base/time/tick_clock.cc
  ${chromium_SOURCE_DIR}/base/time/time.cc
  ${chromium_SOURCE_DIR}/base/timer/elapsed_timer.cc
  ${chromium_SOURCE_DIR}/base/timer/timer.cc
  ${chromium_SOURCE_DIR}/base/threading/non_thread_safe_impl.cc
  ${chromium_SOURCE_DIR}/base/threading/post_task_and_reply_impl.cc
  ${chromium_SOURCE_DIR}/base/threading/sequenced_worker_pool.cc
  ${chromium_SOURCE_DIR}/base/threading/simple_thread.cc
  ${chromium_SOURCE_DIR}/base/threading/thread.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_checker_impl.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_collision_warner.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_id_name_manager.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_local_storage.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_restrictions.cc
  ${chromium_SOURCE_DIR}/base/threading/watchdog.cc
  ${chromium_SOURCE_DIR}/base/threading/worker_pool.cc
  ${chromium_SOURCE_DIR}/base/tracked_objects.cc
  ${chromium_SOURCE_DIR}/base/tracking_info.cc
  ${chromium_SOURCE_DIR}/base/vlog.cc
  ${chromium_SOURCE_DIR}/base/values.cc
)
if (WIN32)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/base_paths_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/debug/debugger_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/debug/stack_trace_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/file_util_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/files/file_enumerator_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/files/file_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/message_loop/message_pump_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/platform_file_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/kill_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/launch_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/memory_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/shared_memory_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_handle_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_info_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_iterator_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_metrics_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/rand_util_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/strings/sys_string_conversions_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/condition_variable_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/lock_impl_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/waitable_event_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/sys_info_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/thread_local_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/thread_local_storage_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/worker_pool_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/time/time_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/timer/hi_res_timer_manager_win.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/win/windows_version.cc)
endif()
if (UNIX)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/atomicops_internals_x86_gcc.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/base_paths_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/debug/debugger_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/debug/stack_trace_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/file_util_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/file_util_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/files/file_enumerator_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/files/file_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/memory/shared_memory_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/message_loop/message_pump_glib.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/message_loop/message_pump_gtk.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/message_loop/message_pump_libevent.cc)
  #list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/message_loop/message_pump_x11.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/nix/mime_util_xdg.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/nix/xdg_util.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/platform_file_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/posix/file_descriptor_shuffle.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/posix/global_descriptors.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/posix/unix_domain_socket_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/internal_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/kill_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/launch_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/memory_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_handle_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_handle_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_info_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_iterator_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_metrics_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_metrics_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/process/process_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/rand_util_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/safe_strerror_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/strings/string16.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/strings/sys_string_conversions_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/condition_variable_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/lock_impl_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/synchronization/waitable_event_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/sys_info_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/sys_info_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimealias.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmime.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimecache.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimeglob.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimeicon.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimeint.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimemagic.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_mime/xdgmimeparent.c)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/third_party/xdg_user_dirs/xdg_user_dir_lookup.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/platform_thread_linux.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/platform_thread_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/thread_local_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/thread_local_storage_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/threading/worker_pool_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/time/time_posix.cc)
  list(APPEND chromium_SOURCE_FILES ${chromium_SOURCE_DIR}/base/timer/hi_res_timer_manager_posix.cc)
endif()

add_library(chromium OBJECT ${chromium_SOURCE_FILES})
add_library(chromium-static STATIC $<TARGET_OBJECTS:chromium>)
#add_library(chromium-shared SHARED $<TARGET_OBJECTS:chromium>)

set(chromium_UNITTEST
  ${chromium_SOURCE_DIR}/base/atomicops_unittest.cc
  ${chromium_SOURCE_DIR}/base/at_exit_unittest.cc
  ${chromium_SOURCE_DIR}/base/callback_helpers_unittest.cc
  ${chromium_SOURCE_DIR}/base/callback_list_unittest.cc
  ${chromium_SOURCE_DIR}/base/callback_unittest.cc
#  ${chromium_SOURCE_DIR}/base/command_line_unittest.cc
  ${chromium_SOURCE_DIR}/base/debug/leak_tracker_unittest.cc
  #${chromium_SOURCE_DIR}/base/debug/stack_trace_unittest.cc
  ${chromium_SOURCE_DIR}/base/debug/trace_event_unittest.cc
  ${chromium_SOURCE_DIR}/base/environment_unittest.cc
#FATAL  ${chromium_SOURCE_DIR}/base/file_util_unittest.cc
  ${chromium_SOURCE_DIR}/base/files/file_path_unittest.cc
#LINK  ${chromium_SOURCE_DIR}/base/files/file_util_proxy_unittest.cc
  ${chromium_SOURCE_DIR}/base/files/scoped_temp_dir_unittest.cc
  ${chromium_SOURCE_DIR}/base/json/json_parser_unittest.cc
#FATAL  ${chromium_SOURCE_DIR}/base/json/json_reader_unittest.cc
  ${chromium_SOURCE_DIR}/base/json/json_value_converter_unittest.cc
#FATAL  ${chromium_SOURCE_DIR}/base/json/json_value_serializer_unittest.cc
  ${chromium_SOURCE_DIR}/base/json/json_writer_unittest.cc
  ${chromium_SOURCE_DIR}/base/json/string_escape_unittest.cc
#COMP  ${chromium_SOURCE_DIR}/base/lazy_instance_unittest.cc
#GMOCK  ${chromium_SOURCE_DIR}/base/logging_unittest.cc
  ${chromium_SOURCE_DIR}/base/memory/aligned_memory_unittest.cc
  ${chromium_SOURCE_DIR}/base/memory/linked_ptr_unittest.cc
  ${chromium_SOURCE_DIR}/base/memory/ref_counted_memory_unittest.cc
  ${chromium_SOURCE_DIR}/base/memory/ref_counted_unittest.cc
  ${chromium_SOURCE_DIR}/base/memory/scoped_ptr_unittest.cc
#COMP  ${chromium_SOURCE_DIR}/base/memory/singleton_unittest.cc
#LINK  ${chromium_SOURCE_DIR}/base/memory/weak_ptr_unittest.cc
#  ${chromium_SOURCE_DIR}/base/message_loop/message_loop_proxy_impl_unittest.cc
  #${chromium_SOURCE_DIR}/base/message_loop/message_loop_proxy_unittest.cc
  #${chromium_SOURCE_DIR}/base/message_loop/message_loop_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_base_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_delta_serialization_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/histogram_snapshot_manager_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/sample_map_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/sample_vector_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/sparse_histogram_unittest.cc
  ${chromium_SOURCE_DIR}/base/metrics/statistics_recorder_unittest.cc
#LINK  ${chromium_SOURCE_DIR}/base/observer_list_unittest.cc
#FATAL  ${chromium_SOURCE_DIR}/base/path_service_unittest.cc
  ${chromium_SOURCE_DIR}/base/pickle_unittest.cc
#LINK  ${chromium_SOURCE_DIR}/base/platform_file_unittest.cc
#  ${chromium_SOURCE_DIR}/base/process/memory_unittest.cc
#  ${chromium_SOURCE_DIR}/base/process/process_metrics_unittest.cc
#  ${chromium_SOURCE_DIR}/base/process/process_util_unittest.cc
  ${chromium_SOURCE_DIR}/base/profiler/tracked_time_unittest.cc
  ${chromium_SOURCE_DIR}/base/scoped_clear_errno_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/nullable_string16_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/string16_unittest.cc
#GMOCK  ${chromium_SOURCE_DIR}/base/strings/string_util_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/string_piece_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/string_number_conversions_unittest.cc
#GMOCK  ${chromium_SOURCE_DIR}/base/strings/string_split_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/stringprintf_unittest.cc
  ${chromium_SOURCE_DIR}/base/strings/utf_string_conversions_unittest.cc
#LINK  ${chromium_SOURCE_DIR}/base/synchronization/condition_variable_unittest.cc
  ${chromium_SOURCE_DIR}/base/synchronization/lock_unittest.cc
  ${chromium_SOURCE_DIR}/base/synchronization/waitable_event_unittest.cc
  ${chromium_SOURCE_DIR}/base/sys_info_unittest.cc
  ${chromium_SOURCE_DIR}/base/template_util_unittest.cc
  ${chromium_SOURCE_DIR}/base/time/pr_time_unittest.cc
  ${chromium_SOURCE_DIR}/base/time/time_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/platform_thread_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/simple_thread_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_collision_warner_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_checker_unittest.cc
#LINK ${chromium_SOURCE_DIR}/base/threading/thread_id_name_manager_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_local_storage_unittest.cc
  ${chromium_SOURCE_DIR}/base/threading/thread_local_unittest.cc  
#LINK  ${chromium_SOURCE_DIR}/base/threading/thread_unittest.cc
  ${chromium_SOURCE_DIR}/base/tracked_objects_unittest.cc
  ${chromium_SOURCE_DIR}/base/vlog_unittest.cc
  ${chromium_SOURCE_DIR}/base/values_unittest.cc
)
if (UNIX)
  list(APPEND chromium_UNITTEST ${chromium_SOURCE_DIR}/base/scoped_clear_errno_unittest.cc)
#GMOCK  list(APPEND chromium_UNITTEST ${chromium_SOURCE_DIR}/base/nix/xdg_util_unittest.cc)
endif()

################################################################################
#
# Google Mock

set(gmock_SOURCE_DIR third_party/testing/gmock)

add_subdirectory(third_party/testing/gmock)

################################################################################
#
# Google Test

set(gtest_SOURCE_DIR third_party/testing/gtest)

# Don't build gtest, gmock handles it..?
#add_subdirectory(third_party/testing/gtest)

################################################################################
#
# Unit Tests

enable_testing(true)

add_executable(unit-tests-chromium
  # We can't link chromium code as a library because it must be compiled with
  # -DUNIT_TEST
  ${chromium_SOURCE_FILES}

  ${chromium_UNITTEST}

  # ${chromium_SOURCE_DIR}/base/test/launcher/test_launcher.cc
  # ${chromium_SOURCE_DIR}/base/test/launcher/test_result.cc
  # ${chromium_SOURCE_DIR}/base/test/launcher/test_results_tracker.cc
  # ${chromium_SOURCE_DIR}/base/test/launcher/unit_test_launcher.cc
  # #${chromium_SOURCE_DIR}/base/test/gtest_xml_util.cc
  # ${chromium_SOURCE_DIR}/base/test/mock_devices_changed_observer.cc
  # ${chromium_SOURCE_DIR}/base/test/mock_time_provider.cc
  # ${chromium_SOURCE_DIR}/base/test/multiprocess_test.cc
  # ${chromium_SOURCE_DIR}/base/test/null_task_runner.cc
  # ${chromium_SOURCE_DIR}/base/test/perf_log.cc
  # ${chromium_SOURCE_DIR}/base/test/perf_test_suite.cc
  # ${chromium_SOURCE_DIR}/base/test/perf_time_logger.cc
  # #${chromium_SOURCE_DIR}/base/test/power_monitor_test_base.cc
  # #${chromium_SOURCE_DIR}/base/test/run_all_perftests.cc
  # ${chromium_SOURCE_DIR}/base/test/run_all_unittests.cc
  # ${chromium_SOURCE_DIR}/base/test/scoped_locale.cc
  # ${chromium_SOURCE_DIR}/base/test/scoped_path_override.cc
  # ${chromium_SOURCE_DIR}/base/test/sequenced_task_runner_test_template.cc
  # ${chromium_SOURCE_DIR}/base/test/sequenced_worker_pool_owner.cc
  # ${chromium_SOURCE_DIR}/base/test/simple_test_clock.cc
  # ${chromium_SOURCE_DIR}/base/test/simple_test_tick_clock.cc
  # ${chromium_SOURCE_DIR}/base/test/statistics_delta_reader.cc
  # ${chromium_SOURCE_DIR}/base/test/statistics_delta_reader_unittest.cc
  # ${chromium_SOURCE_DIR}/base/test/task_runner_test_template.cc
  # ${chromium_SOURCE_DIR}/base/test/test_file_util.cc
  # ${chromium_SOURCE_DIR}/base/test/test_file_util_linux.cc
  # ${chromium_SOURCE_DIR}/base/test/test_file_util_posix.cc
  # ${chromium_SOURCE_DIR}/base/test/test_pending_task.cc
  # ${chromium_SOURCE_DIR}/base/test/test_simple_task_runner.cc
  # ${chromium_SOURCE_DIR}/base/test/test_suite.cc
  # ${chromium_SOURCE_DIR}/base/test/test_switches.cc
  # ${chromium_SOURCE_DIR}/base/test/test_timeouts.cc
  # ${chromium_SOURCE_DIR}/base/test/thread_test_helper.cc
  # ${chromium_SOURCE_DIR}/base/test/trace_event_analyzer.cc
  # ${chromium_SOURCE_DIR}/base/test/trace_event_analyzer_unittest.cc
  # ${chromium_SOURCE_DIR}/base/test/values_test_util.cc

  third_party/testing/multiprocess_func_list.cc
)
# These flags are appended after CMAKE_CXX_FLAGS
set_target_properties(unit-tests-chromium PROPERTIES COMPILE_FLAGS -DUNIT_TEST)

target_link_libraries(unit-tests-chromium X11)
target_link_libraries(unit-tests-chromium event_core)
target_link_libraries(unit-tests-chromium gtk-x11-2.0 gdk-x11-2.0 atk-1.0 gio-2.0 pangoft2-1.0 pangocairo-1.0 gdk_pixbuf-2.0 cairo pango-1.0 fontconfig gobject-2.0 glib-2.0 freetype)
target_link_libraries(unit-tests-chromium gtest gmock gtest_main)
add_test(TestChromium unit-tests-chromium)


add_executable(unit-tests-ghost
  src/playlist_pls.cc
  src/playlist_pls_unittest.cc
)

target_link_libraries(unit-tests-ghost chromium-static)
target_link_libraries(unit-tests-ghost gtest gtest_main)
add_test(TestGhost unit-tests-ghost)

################################################################################
#
# Ghost

include_directories(third_party)
include_directories(${chromium_SOURCE_DIR})
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)

include_directories(src/)

# TODO(brbrooks) should really use cmake packages or pkg-config for these

include_directories(/usr/include/gstreamer-1.0)

include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/glib-2.0/include)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)

include_directories(/usr/include/gtk-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/gtk-2.0/include)
include_directories(/usr/include/cairo)
include_directories(/usr/include/pango-1.0)
include_directories(/usr/include/gdk-pixbuf-2.0)
include_directories(/usr/include/atk-1.0)


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
