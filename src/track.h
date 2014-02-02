#ifndef TRACK_H_
#define TRACK_H_

#include "base/file_path.h"
#include <id3/tag.h>

class Track {
 public:
  Track(const base::FilePath& that);
  ~Track();

  base::FilePath file_path_;
  ID3_Tag tag_;
};

#endif  // TRACK_H_
