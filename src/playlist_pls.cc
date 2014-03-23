// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "playlist_pls.h"

#include <sstream>

#include "base/basictypes.h"
#include "base/file_util.h"
#include "base/strings/string_util.h"
#include "base/strings/string_number_conversions.h"

bool PlaylistPLS::CanConsume(int length) {
  return pos_ + length <= end_pos_;
}

const char* PlaylistPLS::NextChar() {
  DCHECK(CanConsume(1));
  ++index_;
  ++pos_;
  return pos_;
}

void PlaylistPLS::NextNChars(int n) {
  DCHECK(CanConsume(n));
  index_ += n;
  pos_ += n;
}

void PlaylistPLS::EatWhitespace() {
  while (pos_ < end_pos_) {
    switch (*pos_) {
      case '\r':
      case '\n':
        // Don't increment line_number_ twice for "\r\n".
        if (!(*pos_ == '\n' && pos_ > start_pos_ && *(pos_ - 1) == '\r'))
          ++line_number_;
        // Fall through.
      case ' ':
      case '\t':
        NextChar();
        break;
      default:
        return;
    }
  }
}

bool PlaylistPLS::StringsAreEqual(const char* a, const char* b, size_t len) {
  return strncmp(a, b, len) == 0;
}

void PlaylistPLS::Parse2() {
  start_pos_ = &input_[0];
  pos_ = start_pos_;
  end_pos_ = start_pos_ + input_.length();
  index_ = 0;
  line_number_ = 0;

  // [playlist]
  // numberofentries=8
  // File1=http://108.61.73.118:14008
  // Title1=(#1 - 92/1000) 181.fm - Rock 181 (Active Rock)
  // Length1=-1
  // File2=http://108.61.73.117:14008
  // Title2=(#2 - 93/1000) 181.fm - Rock 181 (Active Rock)
  // Length2=-1
  // File3=http://108.61.73.117:8008
  // Title3=(#3 - 108/1000) 181.fm - Rock 181 (Active Rock)
  // Length3=-1
  // File4=http://108.61.73.120:8008
  // Title4=(#4 - 113/1000) 181.fm - Rock 181 (Active Rock)
  // Length4=-1
  // Version=2

  // Have an ordered map of Tracks. key=integer id. Use iterators for access.

  // If File, Length, or Title literals:
  //   Eat the literal.
  //   Eat an integer.
  //   Eat a '='
  //   Eat the value.
  //   Get the Track with id=integer.
  //   Store literal=value in Track.

  // If playlist, numberofentries, version literals:
  //   Eat the literal.
  //   Do checking if possible.

  // Else, bail..

  const char* kPlaylistLiteral = "[playlist]";
  const int kPlaylistLen = static_cast<int>(strlen(kPlaylistLiteral));
  if (CanConsume(kPlaylistLen - 1) &&
      StringsAreEqual(pos_, kPlaylistLiteral, kPlaylistLen)) {
    // Eat it.
    NextNChars(kPlaylistLen);
  }



}

void PlaylistPLS::Parse() {
  std::istringstream iss(input_);
  std::string line;

  int nentries = 0;
  int version = 0;

  std::string f_str = "File";
  std::string t_str = "Title";
  std::string l_str = "Length";

  while (std::getline(iss, line)) {
    // Parse a key=value.
    size_t delim = line.find_first_of("=");
    if (delim == std::string::npos)
      continue;

    std::string k = line.substr(0, delim);
    std::string v = line.substr(delim + 1, line.length());

    if (LowerCaseEqualsASCII(k, "numberofentries")) {
      base::StringToInt(v, &nentries);

      // FIXME(brbrooks) once we know how many entries there are, pre-populate
      // the vector with empty entries so that we can index into them later.
      // It might be better to use a hash table for unencumbered indexing and
      // then convert it to a vector.
      PlaylistPLS::Track empty_track("", "", 0);
      for (int i = 0; i < nentries; ++i) {
        tracks_.push_back(empty_track);
      }
    } else if (LowerCaseEqualsASCII(k, "version")) {
      base::StringToInt(v, &version);
    } else {
      // Handle either File, Title, or Length.
      size_t f = k.find(f_str);
      size_t t = k.find(t_str);
      size_t l = k.find(l_str);

      int n;
      if (f != std::string::npos) {
        base::StringToInt(k.substr(f_str.length(), k.length()), &n);
        if (n >= 1 && n <= nentries)
          tracks_[n - 1].file_ = v;
      } else if (t != std::string::npos) {
        base::StringToInt(k.substr(t_str.length(), k.length()), &n);
        if (n >= 1 && n <= nentries)
          tracks_[n - 1].title_ = v;
      } else if (l != std::string::npos) {
        base::StringToInt(k.substr(l_str.length(), k.length()), &n);
        if (n >= 1 && n <= nentries)
          base::StringToInt(v, &tracks_[n - 1].length_);
      }
    }
  }

  if (version != 2) {
    // unsupported version
  }
  if (nentries != tracks_.size()) {
    // mismatch
  }

  parsed_ = true;
}

PlaylistPLS::PlaylistPLS(base::FilePath& file)
    : parsed_(false),
      start_pos_(NULL),
      pos_(NULL),
      end_pos_(NULL),
      index_(0),
      line_number_(0),
      file_(file) {
  input_.clear();
  if (!base::ReadFileToString(base::MakeAbsoluteFilePath(file_), &input_)) {
    LOG(ERROR) << "Cannot read file " << file.value();
    return;
  }
  Parse();
}

PlaylistPLS::PlaylistPLS(std::string& input)
    : parsed_(false),
      start_pos_(NULL),
      pos_(NULL),
      end_pos_(NULL),
      index_(0),
      line_number_(0),
      input_(input) {
  Parse();
}

PlaylistPLS::~PlaylistPLS() {
}
