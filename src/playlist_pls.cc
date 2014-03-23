// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "playlist_pls.h"

#include <sstream>

#include "base/basictypes.h"
#include "base/file_util.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"

void PlaylistPLS::UpdateTrackFile(int id, std::string& value) {
  tracks2_[id].file_ = value;
}
void PlaylistPLS::UpdateTrackTitle(int id, std::string& value) {
  tracks2_[id].title_ = value;
}
void PlaylistPLS::UpdateTrackLength(int id, int value) {
  tracks2_[id].length_ = value;
}

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

bool PlaylistPLS::ExpectAndConsumeChar(char c) {
  EatWhitespace();
  if (CanConsume(1) && *pos_ == c) {
    NextChar();
    return true;
  }
  LOG(ERROR) << "Expected '" << c << "'.";
  return false;
}

bool PlaylistPLS::ExpectAndConsumeInt(int *i) {
  EatWhitespace();
  const char* num_start = pos_;
  const int start_index = index_;
  int end_index = start_index;

  if (CanConsume(1) && *pos_ == '-')
    NextChar();

  while (CanConsume(1) && IsAsciiDigit(*pos_)) {
    NextChar();
    ++end_index;
  }

  if (end_index == start_index) {
    LOG(ERROR) << "Expected integer.";
    return false;
  }

  base::StringPiece num_string(num_start, end_index - start_index);
  
  int num_int;
  if (base::StringToInt(num_string, &num_int)) {
    *i = num_int;
    return true;
  }
  return false;
}

bool PlaylistPLS::ExpectAndConsumeString(std::string* str) {
  return true;
}

bool PlaylistPLS::ConsumeLiteral(const char* literal) {
  EatWhitespace();
  int len = static_cast<int>(strlen(literal));
  if (CanConsume(len - 1) && StringsAreEqual(pos_, literal, len)) {
    NextNChars(len);
    return true;
  }
  return false;
}

bool PlaylistPLS::StringsAreEqual(const char* a, const char* b, size_t len) {
  return strncmp(a, b, len) == 0;
}

static const char* kFileLiteral = "File";
static const char* kTitleLiteral = "Title";
static const char* kLengthLiteral = "Length";
static const char* kPlaylistLiteral = "[playlist]";
static const char* kNumEntriesLiteral = "numberofentries";
static const char* kVersionLiteral = "Version";

void PlaylistPLS::Parse2() {
  start_pos_ = &input_[0];
  pos_ = start_pos_;
  end_pos_ = start_pos_ + input_.length();
  index_ = 0;
  line_number_ = 0;

  int num_entries = 0;
  int version = 0;
  int id;
  std::string value_str;
  int value_int;

  while (CanConsume(1)) {
    if (ConsumeLiteral(kFileLiteral)) {
      if (!ExpectAndConsumeInt(&id))
        break;
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ExpectAndConsumeString(&value_str))
        break;
      UpdateTrackFile(id, value_str);
    } else if (ConsumeLiteral(kTitleLiteral)) {
      if (!ExpectAndConsumeInt(&id))
        break;
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ExpectAndConsumeString(&value_str))
        break;
      UpdateTrackTitle(id, value_str);
    } else if (ConsumeLiteral(kLengthLiteral)) {
      if (!ExpectAndConsumeInt(&id))
        break;
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ExpectAndConsumeInt(&value_int))
        break;
      UpdateTrackLength(id, value_int);
    } else if (ConsumeLiteral(kPlaylistLiteral)) {
      // Do nothing..
    } else if (ConsumeLiteral(kNumEntriesLiteral)) {
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ExpectAndConsumeInt(&num_entries))
        break;
    } else if (ConsumeLiteral(kVersionLiteral)) {
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ExpectAndConsumeInt(&version))
        break;
    } else {
      LOG(ERROR) << "Cannot parse.. bailing";
      break;
    }
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
