// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "library.h"

#include <vector>

#include "base/files/file_enumerator.h"
#include "base/strings/utf_string_conversions.h"

#include <QtCore/QDebug>
#include <iostream>


static std::vector<Track*> tracks_;

Library::Library() {
}

Library::~Library() {
  // TODO: should tracks_ be vector of scoped_ptr's?
}

void Library::Init(const base::FilePath& path) {
  root_path_ = path;

  // Add all mp3 files found in root_path.
  base::FileEnumerator iter(root_path_, true, base::FileEnumerator::FILES);
  for (base::FilePath name = iter.Next(); !name.empty(); name = iter.Next()) {
    if (name.MatchesExtension(FILE_PATH_LITERAL(".mp3"))) {
      Track* track = new Track(name);
      tracks_.push_back(track);
    }
  }
}

Track* Library::GetTrack(int index) {
  if (index >= 0 && index < (int) tracks_.size()) {
    return tracks_[index];
  }
  return NULL;
}

int Library::GetNumTracks(void) {
  return tracks_.size();
}

void Library::EndOfStream(void) {
}

void Library::PrintTracks(void) {
  for (std::vector<Track*>::iterator it = tracks_.begin(); it != tracks_.end();
       ++it) {
    Track* track = *it;
#if defined(OS_WIN)
    std::cout << base::WideToUTF8(track->file_path_.value()) << std::endl;
#elif defined(OS_POSIX)
    std::cout << track->file_path_.value() << std::endl;
#endif
  }
}
