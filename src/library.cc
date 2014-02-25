#include "library.h"

#include "base/files/file_enumerator.h"

#include <vector>

#include <QtCore/QDebug>


static std::vector<Track*> tracks_;

Library::Library(const base::FilePath& root_path, AudioManager* audio_manager)
    : root_path_(root_path),
      audio_manager_(audio_manager) {
  // Add all mp3 files found in root_path.
  base::FileEnumerator iter(root_path, true, base::FileEnumerator::FILES);
  for (base::FilePath name = iter.Next(); !name.empty(); name = iter.Next()) {
    if (name.MatchesExtension(".mp3")) {
      Track* track = new Track(name);
      tracks_.push_back(track);
    }
  }
}

Library::~Library() {
  // TODO: should tracks_ be vector of scoped_ptr's?
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

void Library::Play(int index) {
  Track* track = GetTrack(index);
  if (track) {
    current_index_ = index;
    audio_manager_->PlayMp3File(track->file_path_);
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
#if 0
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
#endif
}
