// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/file_util.h"

#include <windows.h>
#include <psapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <time.h>

#include <algorithm>
#include <limits>
#include <string>

#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
// #include "base/logging.h"
// #include "base/metrics/histogram.h"
// #include "base/process/process_handle.h"
// #include "base/rand_util.h"
// #include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
// #include "base/threading/thread_restrictions.h"
// #include "base/time/time.h"
// #include "base/win/scoped_handle.h"
#include "base/win/windows_version.h"

namespace base {

namespace {

const DWORD kFileShareAll =
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

}  // namespace

FilePath MakeAbsoluteFilePath(const FilePath& input) {
  // ThreadRestrictions::AssertIOAllowed();
  wchar_t file_path[MAX_PATH];
  if (!_wfullpath(file_path, input.value().c_str(), MAX_PATH))
    return FilePath();
  return FilePath(file_path);
}

}  // namespace base
