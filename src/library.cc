// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "library.h"

#include <set>
#include <string>
#include <vector>

#include "base/bind.h"
#include "base/files/file_enumerator.h"
#include "base/logging.h"
#include "base/prefs/pref_registry_simple.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/thread.h"

#include "prefs.h"
#include "pref_names.h"

// static
scoped_refptr<Library> Library::instance_ = NULL;

namespace {

base::Thread* import_thread = NULL;

static std::vector<Track*> tracks_;

}  // namespace

Library::Library() {
  LOG(INFO) << "Library()";

  if (!import_thread)
    import_thread = new base::Thread("import thread");
}

Library::~Library() {
  LOG(INFO) << "~Library()";

  if (import_thread->IsRunning())
    import_thread->Stop();
  delete import_thread;

  // TODO(brbrooks) delete tracks_  
}

// static
void Library::CreateInstance() {
  if (!instance_)
    instance_ = new Library;
}

// static
Library* Library::GetInstance() {
  DCHECK(instance_) << "Library::CreateInstance must be called before getting "
      "the instance of Library.";
  return instance_;
}

// static
void Library::DeleteInstance() {
  LOG(INFO) << "Library::DeleteInstance()";
  instance_ = NULL;
}

void Library::AddObserver(LibraryObserver* observer) {
    observers_.AddObserver(observer);
}
void Library::RemoveObserver(LibraryObserver* observer) {
    observers_.RemoveObserver(observer);
}

void Library::ImportFromLibraryDir() {
  base::FilePath dir(
      Prefs::GetInstance()->prefs()->GetFilePath(prefs::kLibraryDir));
  // TODO(brbrooks) validate |dir|.
  root_path_ = dir;

  CHECK(import_thread->Start());

  FOR_EACH_OBSERVER(LibraryObserver, observers_, OnBeginImport(this));
  import_thread->message_loop_proxy()->PostTaskAndReply(
      FROM_HERE,
      base::Bind(&Library::Import, this),
      base::Bind(&Library::NotifyImportDone, this));
}

void Library::Import(void) {
  std::set<std::string> extension_set;
  extension_set.insert(".aac");
  extension_set.insert(".flac");
  extension_set.insert(".m4a");
  extension_set.insert(".mp3");
  extension_set.insert(".ogg");

  LOG(INFO) << root_path_.value();
  base::FileEnumerator iter(root_path_, true, base::FileEnumerator::FILES);
  for (base::FilePath name = iter.Next(); !name.empty(); name = iter.Next()) {
    if (extension_set.count(StringToLowerASCII(name.FinalExtension())) > 0) {
      Track* track = new Track(name);
      tracks_.push_back(track);
    }
  }
  LOG(INFO) << "Import got " << tracks_.size() << " tracks..";
}

void Library::NotifyImportDone(void) {
  FOR_EACH_OBSERVER(LibraryObserver, observers_, OnFinishImport(this));
}

Track* Library::GetTrackForPlaying(int index) {
  DCHECK(index >= 0 && index < tracks_.size());
  current_track_ = index;
  return tracks_[current_track_];
}

Track* Library::GetNextTrackForPlaying() {
  current_track_ += 1;
  if (current_track_ >= tracks_.size())
    current_track_ = 0;
  return tracks_[current_track_];
}

Track* Library::GetTrack(int index) {
  DCHECK(index >= 0 && index < tracks_.size());
  return tracks_[index];
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

// static
void Library::RegisterPrefs(PrefRegistrySimple* registry) {
  registry->RegisterFilePathPref(
      prefs::kLibraryDir, base::FilePath("../test-data/"));
}
