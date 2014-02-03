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

Library::Library(const base::FilePath& root_path) : root_path_(root_path) {
  // Visit all files in root_path_.
  nftw(root_path_.value().c_str(), LibraryNftwCallback, 100, FTW_DEPTH | FTW_PHYS);
}

Library::~Library() {
  // TODO: should tracks_ be scoped_ptrs?
}

Track* Library::GetATrack(void) {
  return tracks_[1];
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
