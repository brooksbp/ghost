// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "library.h"

#include <vector>

#include "base/bind.h"
#include "base/files/file_enumerator.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread.h"

namespace {

static base::Thread import_thread("Import thread");

// TODO: should be a base::ScopedVector
static std::vector<Track*> tracks_;

}  // namespace

Library::Library() {
}

Library::~Library() {
}

void Library::Init(const base::FilePath& path) {
  root_path_ = path;

  CHECK(import_thread.Start());

  FOR_EACH_OBSERVER(LibraryObserver, observer_list_, OnBeginImport(this));
  import_thread.message_loop_proxy()->PostTaskAndReply(
      FROM_HERE,
      base::Bind(&Library::Import, this),
      base::Bind(&Library::NotifyImportDone, this));
}

void Library::Import(void) {
  // Add music files found in root_path.
  base::FileEnumerator iter(root_path_, true, base::FileEnumerator::FILES);
  for (base::FilePath name = iter.Next(); !name.empty(); name = iter.Next()) {
    if (name.MatchesExtension(FILE_PATH_LITERAL(".mp3")) ||
        name.MatchesExtension(FILE_PATH_LITERAL(".flac"))) {
      Track* track = new Track(name);
      tracks_.push_back(track);
    }
  }
}

void Library::NotifyImportDone(void) {
  FOR_EACH_OBSERVER(LibraryObserver, observer_list_, OnFinishImport(this));
}

Track* Library::GetTrack(int index) {
  if (index >= 0 && index < (int) tracks_.size()) {
    return tracks_[index];
  }
  return NULL;
}

Track* Library::GetTrackForPlaying(int index) {
  if (index >= 0 && index < (int) tracks_.size()) {
    current_track_ = index;
    return GetCurrentTrack();
  }
  return NULL;
}

Track* Library::GetCurrentTrack() {
  return tracks_[current_track_];
}

int Library::GetNumTracks(void) {
  return tracks_.size();
}

void Library::PrintTracks(void) {
  for (std::vector<Track*>::iterator it = tracks_.begin(); it != tracks_.end();
       ++it) {
    Track* track = *it;
#if defined(OS_WIN)
    LOG(INFO) << base::WideToUTF8(track->file_path_.value());
#elif defined(OS_POSIX)
    LOG(INFO) << track->file_path_.value();
#endif
  }
}
