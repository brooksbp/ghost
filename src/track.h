// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef TRACK_H_
#define TRACK_H_

#include "base/files/file_path.h"
#include <id3/tag.h>

class Track {
 public:
  Track(const base::FilePath& that);
  ~Track();

  void BuildTag(void);
  
  base::FilePath file_path_;
  ID3_Tag tag_;
};

#endif  // TRACK_H_
