// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains utility functions for dealing with the local
// filesystem.

#ifndef BASE_FILE_UTIL_H_
#define BASE_FILE_UTIL_H_

#include "base/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <stdio.h>

#include <set>
#include <string>
#include <vector>

#include "base/base_export.h"
#include "base/basictypes.h"
// #include "base/files/file.h"
#include "base/files/file_path.h"
// #include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"

#if defined(OS_POSIX)
#include "base/file_descriptor_posix.h"
// #include "base/logging.h"
#include "base/posix/eintr_wrapper.h"
#endif

namespace base {

// class Time;

//------------------------------------------------------------------------------
// Functions that involve filesystem access or modification:

// Returns an absolute version of a relative path. Returns an empty path on
// error. On POSIX, this function fails if the path does not exist. This
// function can result in I/O so it can be slow.
BASE_EXPORT FilePath MakeAbsoluteFilePath(const FilePath& input);

// Returns the total number of bytes used by all the files under |root_path|.
// If the path does not exist the function returns 0.
//
// This function is implemented using the FileEnumerator class so it is not
// particularly speedy in any platform.
BASE_EXPORT int64 ComputeDirectorySize(const FilePath& root_path);


}  // namespace base

#endif  // BASE_FILE_UTIL_H_
