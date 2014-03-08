// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_util.h"

#if defined(OS_WIN)
#include <io.h>
#endif
#include <stdio.h>

#include <fstream>
#include <limits>

#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_util.h"
// #include "base/strings/stringprintf.h"
#include "base/strings/utf_string_conversions.h"

namespace base {

namespace {

// The maximum number of 'unqualified' files we will try to create.
// This is used when the filename we're trying to download is already in use,
// so we create a new unique filename by appending " (nnn)" before the
// extension, where 1 <= nnn <= kMaxUniqueFiles.
// Also used by code that cleans up said files.
static const int kMaxUniqueFiles = 100;

}  // namespace

int64 ComputeDirectorySize(const FilePath& root_path) {
  int64 running_size = 0;
  FileEnumerator file_iter(root_path, true, FileEnumerator::FILES);
  while (!file_iter.Next().empty())
    running_size += file_iter.GetInfo().GetSize();
  return running_size;
}


bool ReadFileToString(const FilePath& path,
                      std::string* contents,
                      size_t max_size) {
  if (contents)
    contents->clear();
  if (path.ReferencesParent())
    return false;
  FILE* file = OpenFile(path, "rb");
  if (!file) {
    return false;
  }

  char buf[1 << 16];
  size_t len;
  size_t size = 0;
  bool read_status = true;

  // Many files supplied in |path| have incorrect size (proc files etc).
  // Hence, the file is read sequentially as opposed to a one-shot read.
  while ((len = fread(buf, 1, sizeof(buf), file)) > 0) {
    if (contents)
      contents->append(buf, std::min(len, max_size - size));

    if ((max_size - size) < len) {
      read_status = false;
      break;
    }
    
    size += len;
  }
  CloseFile(file);

  return read_status;
}

bool ReadFileToString(const FilePath& path, std::string* contents) {
  return ReadFileToString(path, contents, std::numeric_limits<size_t>::max());
}


bool CloseFile(FILE* file) {
  if (file == NULL)
    return true;
  return fclose(file) == 0;
}

bool TruncateFile(FILE* file) {
  if (file == NULL)
    return false;
  long current_offset = ftell(file);
  if (current_offset == -1)
    return false;
#if defined(OS_WIN)
  int fd = _fileno(file);
  if (_chsize(fd, current_offset) != 0)
    return false;
#else
  int fd = fileno(file);
  if (ftruncate(fd, current_offset) != 0)
    return false;
#endif
  return true;
}

}  // namespace base
