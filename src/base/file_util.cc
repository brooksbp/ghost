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
// #include "base/logging.h"
// #include "base/strings/string_piece.h"
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


}  // namespace base
