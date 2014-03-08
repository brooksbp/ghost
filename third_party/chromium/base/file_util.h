// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains utility functions for dealing with the local
// filesystem.

#ifndef BASE_FILE_UTIL_H_
#define BASE_FILE_UTIL_H_

#include "build/build_config.h"

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
#include "base/logging.h"
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


// Returns true if the given path exists on the local filesystem,
// false otherwise.
BASE_EXPORT bool PathExists(const FilePath& path);


// Returns true if the given path exists and is a directory, false otherwise.
BASE_EXPORT bool DirectoryExists(const FilePath& path);


// Reads the file at |path| into |contents| and returns true on success.
// |contents| may be NULL, in which case this function is useful for its
// side effect of priming the disk cache (could be used for unit tests).
// The function returns false and the string pointed to by |contents| is
// cleared when |path| does not exist or if it contains path traversal
// components ('..').
BASE_EXPORT bool ReadFileToString(const FilePath& path, std::string* contents);

// Reads the file at |path| into |contents| and returns true on success.
// |contents| may be NULL, in which case this function is useful for its
// side effect of priming the disk cache (could be used for unit tests).
// The function returns false and the string pointed to by |contents| is
// cleared when |path| does not exist or if it contains path traversal
// components ('..').
// When the file size exceeds |max_size|, the function returns false
// with |contents| holding the file truncated to |max_size|.
BASE_EXPORT bool ReadFileToString(const FilePath& path,
                                  std::string* contents,
                                  size_t max_size);


// Get the home directory. This is more complicated than just getenv("HOME")
// as it knows to fall back on getpwent() etc.
//
// You should not generally call this directory. Instead use DIR_HOME with the
// path service which will use this function but cache the value.
BASE_EXPORT FilePath GetHomeDir();


// Wrapper for fopen-like calls. Returns non-NULL FILE* on success.
BASE_EXPORT FILE* OpenFile(const FilePath& filename, const char* mode);

// Closes file opened by OpenFile. Returns true on success.
BASE_EXPORT bool CloseFile(FILE* file);

// Truncates an open file to end at the location of the current file pointer.
// This is a cross-platform analog to Windows' SetEndOfFile() function.
BASE_EXPORT bool TruncateFile(FILE* file);

// Reads the given number of bytes from the file into the buffer.  Returns
// the number of read bytes, or -1 on error.
BASE_EXPORT int ReadFile(const FilePath& filename, char* data, int size);

}  // namespace base

#endif  // BASE_FILE_UTIL_H_
