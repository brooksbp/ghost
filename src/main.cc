#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"

//#define _XOPEN_SOURCE 600
#include <ftw.h>

#include <iostream>
#include <vector>

static std::vector<Track*> tracks;

static int func(const char* pathname, const struct stat* sbuf, int type,
                struct FTW* ftwb) {
  if (type == FTW_F) {
    Track* track = new Track(base::FilePath(pathname));
    tracks.push_back(track);
  }
  return 0;
}


int main(int argc, const char* argv[]) {
  CommandLine cl(argc, argv);

  if (!cl.HasSwitch("dir")) {
    std::cout << "usage: " << argv[0] << " --dir=<music-directory>" << std::endl;
    return 0;
  }
  base::FilePath dir = cl.GetSwitchValuePath("dir");
  std::cout << "dir=" << dir.value() << std::endl;

  // Find files in |dir|.
  nftw(dir.value().c_str(), func, 100, FTW_DEPTH | FTW_PHYS);

  // Print the ID3 track title.
  for (std::vector<Track*>::iterator it = tracks.begin(); it != tracks.end();
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
