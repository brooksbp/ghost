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
// #include "base/stl_util.h"
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

}  // namespace

FilePath MakeAbsoluteFilePath(const FilePath& input) {
  // ThreadRestrictions::AssertIOAllowed();
  char full_path[PATH_MAX];
  if (realpath(input.value().c_str(), full_path) == NULL)
    return FilePath();
  return FilePath(full_path);
}

}  // namespace base
