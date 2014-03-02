#ifndef PLAYLIST_PLS_H_
#define PLAYLIST_PLS_H_

#include <string>
#include <vector>

#include "base/files/file_path.h"

// PLS file format.
class PlaylistPLS {
 public:
  struct Track {
    std::string file_;    // Location of stream.
    std::string title_;   // Track title.
    int length_;          // Length in seconds of track. -1 indicates indefinite.

    Track(std::string file, std::string title, int length)
        : file_(file),
          title_(title),
          length_(length) {
    }
  };

  std::vector<Track> tracks_;

  PlaylistPLS(base::FilePath& file);
  PlaylistPLS(std::string& contents);
  ~PlaylistPLS();

 private:

  void Parse();
  bool parsed_;

  std::string contents_;
  base::FilePath file_;
};

#endif  // PLAYLIST_PLS_H_
