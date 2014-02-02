#include "track.h"

Track::Track(const base::FilePath& that) :
    file_path_(that),
    tag_(that.value().c_str()) {
}

Track::~Track() {
}
