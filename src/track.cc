#include "track.h"

Track::Track(const base::FilePath& that) :
    file_path_(that) {
}

Track::~Track() {
}

void Track::BuildTag(void) {
  tag_ = ID3_Tag(file_path_.value().c_str());
}
