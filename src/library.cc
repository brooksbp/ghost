#include <QtCore/QDebug>

#include "library.h"

#include <ftw.h>

#include <vector>

static std::vector<Track*> tracks_;

static int LibraryNftwCallback(const char* pathname, const struct stat* sbuf,
                               int type, struct FTW* ftwb) {
  if (type == FTW_F) {
    base::FilePath file(pathname);
    if (file.MatchesExtension(".mp3")) {
      Track* track = new Track(file);
      tracks_.push_back(track);
    }
  }
  return 0;
}

Library::Library(const base::FilePath& root_path, AudioManager* audio_manager)
    : root_path_(root_path),
      audio_manager_(audio_manager) {
  // Visit all files in root_path_.
  nftw(root_path_.value().c_str(), LibraryNftwCallback, 100, FTW_DEPTH | FTW_PHYS);
}

Library::~Library() {
  // TODO: should tracks_ be scoped_ptrs?
}

Track* Library::GetTrack(int index) {
  if (index >= 0 && index < tracks_.size()) {
    return tracks_[index];
  }
  return NULL;
}

int Library::GetNumTracks(void) {
  return tracks_.size();
}

void Library::Play(int index) {
  Track* track = GetTrack(index);
  if (track) {
    current_index_ = index;
    audio_manager_->PlayMp3File(track->file_path_.value().c_str());
  }
}

void Library::EndOfStream(void) {
#if 0 // FIXME
  // Play the next index.
  if (current_index_ + 1 == tracks_.size())
    current_index_ = 0;
  else
    current_index_ += 1;

  Track* track = GetTrack(current_index_);
  if (track) {
    audio_manager_->PlayMp3File(track->file_path_.value().c_str());
  }
#endif
}

void Library::PrintTracks(void) {
  for (std::vector<Track*>::iterator it = tracks_.begin(); it != tracks_.end();
       ++it) {
    Track* track = *it;
    ID3_Frame* frame;
    if ((frame = track->tag_.Find(ID3FID_TITLE))) {
      char title[1024];
      frame->Field(ID3FN_TEXT).Get(title, 1024);
      std::cout << "Track: " << title << std::endl;
    }
  }
}