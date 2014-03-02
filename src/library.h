// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "base/files/file_path.h"
#include "track.h"

class Library {
 public:
  Library();
  ~Library();

  void Init(const base::FilePath& path);

  Track* GetTrack(int index);
  int GetNumTracks(void);

  void PrintTracks(void);

  void Play(int index);
  
  void EndOfStream(void);
  
 private:
  base::FilePath root_path_;

  // Current index into |tracks|.
  int current_index_;
};

#endif  // LIBRARY_H_
