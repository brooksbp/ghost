// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "track.h"

Track::Track(const base::FilePath& that) :
    file_path_(that) {
  BuildTag();
}

Track::~Track() {
}

void Track::BuildTag(void) {
  //tag_ = ID3_Tag(file_path_.value().c_str());
}
