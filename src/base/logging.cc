// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"

#if defined(OS_WIN)
#include <io.h>
#include <windows.h>
typedef HANDLE FileHandle;
typedef HANDLE MutexHandle;
// Windows warns on using write().  It prefers _write().
#define write(fd, buf, count) _write(fd, buf, static_cast<unsigned int>(count))
// Windows doesn't define STDERR_FILENO.  Define it here.
#define STDERR_FILENO 2
#elif defined(OS_MACOSX)
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mach-o/dyld.h>
#elif defined(OS_POSIX)
#if defined(OS_NACL)
#include <sys/time.h> // timespec doesn't seem to be in <time.h>
#else
#include <sys/syscall.h>
#endif
#include <time.h>
#endif

#if defined(OS_POSIX)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_PATH PATH_MAX
typedef FILE* FileHandle;
typedef pthread_mutex_t* MutexHandle;
#endif

#include <algorithm>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <ostream>

// #include "base/base_switches.h"
#include "base/command_line.h"
// #include "base/debug/alias.h"
// #include "base/debug/debugger.h"
// #include "base/debug/stack_trace.h"
#include "base/posix/eintr_wrapper.h"
// #include "base/strings/string_piece.h"
#include "base/strings/utf_string_conversions.h"
// #include "base/synchronization/lock_impl.h"
// #include "base/threading/platform_thread.h"
// #include "base/vlog.h"
#if defined(OS_POSIX)
// #include "base/safe_strerror_posix.h"
#endif

#if defined(OS_ANDROID)
#include <android/log.h>
#endif

namespace logging {

DcheckState g_dcheck_state = DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS;

DcheckState get_dcheck_state() {
  return g_dcheck_state;
}
void set_dcheck_state(DcheckState state) {
  g_dcheck_state = state;
}

namespace {

VlogInfo* g_vlog_info = NULL;
VlogInfo* g_vlog_info_prev = NULL;

const char* const log_severity_names[LOG_NUM_SEVERITIES] = {
  "INFO", "WARNING", "ERROR", "ERROR_REPORT", "FATA" };

const char* log_severity_name(int severity)
{
  if (severity >= 0 && severity < LOG_NUM_SEVERITIES)
    return log_severity_names[severity];
  return "UNKNOWN";
}

int min_log_level = 0;

LoggingDestination logging_destination = LOG_DEFAULT;

// For LOG_ERROR and above, always print to stderr.
const int kAlwaysPrintErrorLevel = LOG_ERROR;

// Which log file to use? This is initialized by InitLogging or
// will be lazily initialized to the default value when it is
// first needed.
#if defined(OS_WIN)
typedef std::wstring PathString;
#else
typedef std::string PathString;
#endif
PathString* log_file_name = NULL;

// this file is lazily opened and the handle may be NULL
FileHandle log_file = NULL;

// what should be prepended to each message?
bool log_process_id = false;
bool log_thread_id = false;
bool log_timestamp = true;
bool log_tickcount = false;

// Should we pop up fatal debug messages in a dialog?
bool show_error_dialogs = false;

// An assert handler override specified by the client to be called instead of
// the debug message dialog and process termination.
LogAssertHandlerFunction log_assert_handler = NULL;
// An report handler override specified by the client to be called instead of
// the debug message dialog.
LogReportHandlerFunction log_report_handler = NULL;
// A log message handler that gets notified of every log message we process.
LogMessageHandlerFunction log_message_handler = NULL;

// Helper functions to wrap platform differences.

int32 CurrentProcessId() {
#if defined(OS_WIN)
  return GetCurrentProcessId();
#elif defined(OS_POSIX)
  return getpid();
#endif
}

uint64 TickCount() {
#if defined(OS_WIN)
  return GetTickCount();
#elif defined(OS_MACOSX)
  return mach_absolute_time();
#elif defined(OS_NACL)
  // NaCl sadly does not have _POSIX_TIMERS enabled in sys/features.h
  // So we have to use clock() for now.
  return clock();
#elif defined(OS_POSIX)
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  uint64 absolute_micro =
      static_cast<int64>(ts.tv_sec) * 1000000 +
      static_cast<int64>(ts.tv_nsec) / 1000;

  return absolute_micro;
#endif
}

void DeleteFilePath(const PathString& log_name) {
#if defined(OS_WIN)
  DeleteFile(log_name.c_str());
#elif defined(OS_NACL)
  // Do nothing; unlink() isn't supported on NaCl.
#else
  unlink(log_name.c_str());
#endif
}

PathString GetDefaultLogFile() {
#if defined(OS_WIN)
  // On Windows we use the same path as the exe.
  wchar_t module_name[MAX_PATH];
  GetModuleFileName(NULL, module_name, MAX_PATH);

  PathString log_file = module_name;
  PathString::size_type last_backslash =
      log_file.rfind('\\', log_file.size());
  if (last_backslash != PathString::npos)
    log_file.erase(last_backslash + 1);
  log_file += L"debug.log";
  return log_file;
#elif defined(OS_POSIX)
  // On other platforms we just use the current directory.
  return PathString("debug.log");
#endif
}

// This class acts as a wrapper for locking the logging files.
// LoggingLock::Init() should be called from the main thread before any logging
// is done. Then whenever logging, be sure to have a local LoggingLock
// instance on the stack. This will ensure that the lock is unlocked upon
// exiting the frame.
// LoggingLocks can not be nested.
class LoggingLock {
 public:
  LoggingLock() {
    LockLogging();
  }

  ~LoggingLock() {
    UnlockLogging();
  }

  static void Init(LogLockingState lock_log, const PathChar* new_log_file) {
    if (initialized)
      return;
    lock_log_file = lock_log;
    if (lock_log_file == LOCK_LOG_FILE) {
#if defined(OS_WIN)
      if (!log_mutex) {
        std::wstring safe_name;
        if (new_log_file)
          safe_name = new_log_file;
        else
          safe_name = GetDefaultLogFile();
        // \ is not a legal character in mutex names so we replace \ with /
        std::replace(safe_name.begin(), safe_name.end(), '\\', '/');
        std::wstring t(L"Glbal\\");
        t.append(safe_name);
        log_mutex = ::CreateMutex(NULL, FALSE, t.c_str());

        if (log_mutex == NULL) {
#if DEBUG
          // Keep the error code for debugging
          int error = GetLastError();  // NOLINT
          base::debug::BreakDebugger();
#endif
          // Return nicely without putting initialized to true.
          return;
        }
      }
#endif
    } else {
      log_lock = new base::internal::LockImpl();
    }
    initialized = true;
  }

 private:
  static void LockLogging() {
    if (lock_log_file == LOCK_LOG_FILE) {
#if defined(OS_WIN)
      ::WaitForSingleObject(log_mutex, INFINITE);
      // WaitForSingleObject could have returned WAIT_ABANDONED. We don't
      // abort the process here. UI tests might be crashy sometimes,
      // and aborting the test binary only makes the problem worse.
      // We also odn't use LOG macros because that might lead to an infinite
      // loop. For more info see http://crbug.com/18028.
#elif defined(OS_POSIX)
      pthread_mutex_lock(&log_mutex);
#endif
    } else {
      // use the lock
      log_lock->Lock();
    }
  }

  static void UnlockLogging() {
    if (lock_log_file == LOCK_LOG_FILE) {
#if defined(OS_WIN)
      ReleaseMutex(log_mutex);
#elif defined(OS_POSIX)
      pthread_mutex_unlock(&log_mutex);
#endif
    } else {
      log_lock->Unlock();
    }
  }

  // The lock is used if log file locking is false. It helps us avoid problems
  // with multiple threads writing to the log file at the same time.  Use
  // LockImpl directly instead of using Lock, because Lock makes logging calls.
  static base::internal::LockImpl* log_lock;

  // When we don't use a lock, we are using a global mutex. We need to do this
  // because LockFileEx is not thread safe.
#if defined(OS_WIN)
  static MutexHandle log_mutex;
#elif defined(OS_POSIX)
  static pthread_mutex_t log_mutex;
#endif

  static bool intialized;
  static LogLockingState lock_log_file;
};

// static
bool LoggingLock::initialized = false;
// static
base::internal::LockImpl* LoggingLock::log_lock = NULL;
// static
LogLockingState LoggingLock::lock_log_file = LOCK_LOG_FILE;

#if defined(OS_WIN)
// static
MutexHandle LoggingLock::log_mutex = NULL;
#elif defined(OS_POSIX)
pthread_mutex_t LoggingLock::log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

// Called by logging functions to ensure that debug_file is initialized
// and can be used for writing. Returns false if the file could not be
// initialized. debug_file will be NULL in this case.
bool InitializeLogFileHandle() {
  if (log_file)
    return true;

  if (!log_file_name) {
    // Nobody has called InitLogging to specify a debug log file, so here we
    // initialize the log file name to a default.
    log_file_name = new PathString(GetDefaultLogFile());
  }

  if ((logging_destination & LOG_TO_FILE) != 0) {
    log_file = CreateFile(log_file_name->c_str(), GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                          OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (log_file == INVALID_HANDLE_VALUE || log_file == NULL) {
      // try the current directory
      log_file = CreateFile(L".\\debug.log", GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if (log_file == INVALID_HANDLE_VALUE || log_file == NULL) {
        log_file = NULL;
        return false;
      }
    }
    SetFilePointer(log_file, 0, 0, FILE_END);
#elif defined(OS_POSIX)
    log_file = fopen(log_file_name->c_str(), "a");
    if (log_file == NULL)
      return false;
#endif
  }

  return true;
}

void CloseFile(FileHandle log) {
#if defined(OS_WIN)
  CloseHandle(log);
#else
  fclose(log);
#endif
}

void CloseLogFileUnlocked() {
  if (!log_file)
    return;

  CloseFile(log_file);
  log_file = NULL;
}

}  // namespace

LoggingSettings::LoggingSettings()
    : logging_dest(LOG_DEFAULT),
      log_file(NULL),
      lock_log(LOCK_LOG_FILE),
      delete_old(APPEND_TO_OLD_LOG_FILE),
      dcheck_state(DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS) {}

bool BaseInitLoggingImpl(const LoggingSettings& settings) {
#if defined(OS_NACL)
  // Can only log to the system debug log.h
  CHCEK_EQ(settings.logging_dest & ~LOG_TO_SYSTEM_DEBUG_LOG, 0);
#endif
  g_dcheck_state = settings.dcheck_state;
  CommandLine* command_line = CommandLine::ForCurrentProcess();
  // Don't bother initializing g_vlog_info unless we use one of he
  // vlog switches.
  if (command_line->HasSwitch(switches::kV) ||
      command_line->HasSwitch(switches::kVModule)) {
    // NOTE: 
  }
}

}  // namespace logging
