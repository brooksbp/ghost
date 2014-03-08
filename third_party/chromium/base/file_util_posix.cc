// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_util.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#if defined(OS_MACOSX)
#include <AvailabilityMacros.h>
#include "base/mac/foundation_util.h"
#elif !defined(OS_CHROMEOS) && defined(USE_GLIB)
#include <glib.h>  // for g_get_home_dir()
#endif

#include <fstream>

#include "base/basictypes.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
// #include "base/logging.h"
// #include "base/memory/scoped_ptr.h"
// #include "base/memory/singleton.h"
// #include "base/path_service.h"
#include "base/posix/eintr_wrapper.h"
#include "base/stl_util.h"
#include "base/strings/string_util.h"
// #include "base/strings/stringprintf.h"
// #include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
// #include "base/sys_info.h"
// #include "base/threading/thread_restrictions.h"
// #include "base/time/time.h"

#if defined(OS_ANDROID)
#include "base/android/content_uri_utils.h"
#include "base/os_compat_android.h"
#endif

#if !defined(OS_IOS)
#include <grp.h>
#endif

namespace base {

namespace {

#if defined(OS_BSD) || defined(OS_MACOSX)
typedef struct stat stat_wrapper_t;
static int CallStat(const char* path, stat_wrapper_t* sb) {
  // ThreadRestrictions::AssertIOAllowed();
  return stat(path, sb);
}
static int CallLstat(const char* path, stat_wrapper_t* sb) {
  // ThreadRestrictions::AssertIOAllowed();
  return lstat(path, sb);
}
#else
typedef struct stat64 stat_wrapper_t;
static int CallStat(const char* path, stat_wrapper_t* sb) {
  // ThreadRestrictions::AssertIOAllowed();
  return stat64(path, sb);
}
static int CallLstat(const char* path, stat_wrapper_t* sb) {
  // ThreadRestrictions::AssertIOAllowed();
  return lstat64(path, sb);
}
#if defined(OS_ANDROID)
static int CallFstat(int fd, stat_wrapper_t* sb) {
  // ThreadRestrictions::AssertIOAllowed();
  return fstat64(fd, sb);
}
#endif
#endif

// Helper for NormalizeFilePath(), defined below.
bool RealPath(const FilePath& path, FilePath* real_path) {
  // ThreadRestrictions::AssertIOAllowed();  // For realpath().
  FilePath::CharType buf[PATH_MAX];
  if (!realpath(path.value().c_str(), buf))
    return false;

  *real_path = FilePath(buf);
  return true;
}

// Helper for VerifyPathControlledByUser.
bool VerifySpecificPathControlledByUser(const FilePath& path,
                                        uid_t owner_uid,
                                        const std::set<gid_t>& group_gids) {
  stat_wrapper_t stat_info;
  if (CallLstat(path.value().c_str(), &stat_info) != 0) {
    // DPLOG(ERROR) << "Failed to get information on path "
    //              << path.value();
    return false;
  }

  if (S_ISLNK(stat_info.st_mode)) {
    // DPLOG(ERROR) << "Path " << path.value()
    //              << "is a symbolic link.";
    return false;
  }

  if (stat_info.st_uid != owner_uid) {
    // DPLOG(ERROR) << "Path " << path.value()
    //              << "is owned by the wrong user.";
    return false;
  }

  if ((stat_info.st_mode & S_IWGRP) &&
      !ContainsKey(group_gids, stat_info.st_gid)) {
    // DPLOG(ERROR) << "Path " << path.value()
    //              << "is writable by an unprivileged group.";
    return false;
  }

  if (stat_info.st_mode & S_IWOTH) {
    // DPLOG(ERROR) << "Path " << path.value()
    //              << "is writable by any user.";
    return false;
  }

  return true;
}

}  // namespace

FilePath MakeAbsoluteFilePath(const FilePath& input) {
  // ThreadRestrictions::AssertIOAllowed();
  char full_path[PATH_MAX];
  if (realpath(input.value().c_str(), full_path) == NULL)
    return FilePath();
  return FilePath(full_path);
}


bool PathExists(const FilePath& path) {
  // ThreadRestrictions::AssertIOAllowed();
#if defined (OS_ANDROID)
  if (path.IsContentUri()) {
    return ContentUriExists(path);
  }
#endif
  return access(path.value().c_str(), F_OK) == 0;
}


bool DirectoryExists(const FilePath& path) {
  // ThreadRestrictions::AssertIOAllowed();
  stat_wrapper_t file_info;
  if (CallStat(path.value().c_str(), &file_info) == 0)
    return S_ISDIR(file_info.st_mode);
  return false;
}


#if !defined(OS_MACOSX)  // Mac implementation is in file_util_mac.mm.
FilePath GetHomeDir() {
#if defined(OS_CHROMEOS)
  if (SysInfo::IsRunningOnChromeOS())
    return FilePath("/home/chronos/user");
#endif

  const char* home_dir = getenv("HOME");
  if (home_dir && home_dir[0])
    return FilePath(home_dir);

  // FIXME(brbrooks) the rest of this function...

  // Last resort.
  return FilePath("/tmp");
}
#endif  // !defined(OS_MACOSX)


FILE* OpenFile(const FilePath& filename, const char* mode) {
  // ThreadRestrictions::AssertIOAllowed();
  FILE* result = NULL;
  do {
    result = fopen(filename.value().c_str(), mode);
  } while (!result && errno == EINTR);
  return result;
}

int ReadFile(const FilePath& filename, char* data, int size) {
  // ThreadRestrictions::AssertIOAllowed();
  int fd = HANDLE_EINTR(open(filename.value().c_str(), O_RDONLY));
  if (fd < 0)
    return -1;

  ssize_t bytes_read = HANDLE_EINTR(read(fd, data, size));
  if (int ret = IGNORE_EINTR(close(fd)) < 0)
    return ret;
  return bytes_read;
}

}  // namespace base
