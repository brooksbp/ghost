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
  tracks_[id].file_ = value;
}
void PlaylistPLS::UpdateTrackTitle(int id, std::string& value) {
  tracks_[id].title_ = value;
}
void PlaylistPLS::UpdateTrackLength(int id, int value) {
  tracks_[id].length_ = value;
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

bool PlaylistPLS::TryExpectAndConsumeLiteral(const char* literal) {
  EatWhitespace();
  int len = static_cast<int>(strlen(literal));
  if (CanConsume(len - 1) && StringsAreEqual(pos_, literal, len)) {
    NextNChars(len);
    return true;
  }
  return false;
}

bool PlaylistPLS::ConsumeInt(int *out) {
  EatWhitespace();
  bool negate_value = false;

  if (CanConsume(1) && *pos_ == '-') {
    NextChar();
    negate_value = true;
  }

  const char* num_start = pos_;
  const int start_index = index_;
  int end_index = start_index;

  while (CanConsume(1) && IsAsciiDigit(*pos_)) {
    NextChar();
    ++end_index;
  }

  if (end_index == start_index)
    return false;

  base::StringPiece num_string(num_start, end_index - start_index);
  
  int num_int;
  if (base::StringToInt(num_string, &num_int)) {
    *out = (negate_value) ? -num_int : num_int;
    return true;
  }
  return false;
}

bool PlaylistPLS::ConsumeString(std::string* out) {
  EatWhitespace();
  const char* str_start = pos_;
  const int start_index = index_;
  int end_index = start_index;

  while (CanConsume(1) && *pos_ != '\r' && *pos_ != '\n') {
    NextChar();
    ++end_index;
  }
  
  if (end_index == start_index)
    return false;

  // TODO(brbrooks) Strip trailing whitespace?

  std::string str(str_start, end_index - start_index);

  *out = str;
  return true;
}

bool PlaylistPLS::StringsAreEqual(const char* a, const char* b, size_t len) {
  // TODO(brbrooks) Case-insensitive compare?
  return strncmp(a, b, len) == 0;
}

static const char* kFileLiteral = "File";
static const char* kTitleLiteral = "Title";
static const char* kLengthLiteral = "Length";
static const char* kPlaylistLiteral = "[playlist]";
static const char* kNumEntriesLiteral = "numberofentries";
static const char* kVersionLiteral = "Version";

void PlaylistPLS::Parse() {
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
    if (TryExpectAndConsumeLiteral(kFileLiteral)) {
      if (!ConsumeInt(&id))
        break;
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ConsumeString(&value_str))
        break;
      UpdateTrackFile(id, value_str);
    } else if (TryExpectAndConsumeLiteral(kTitleLiteral)) {
      if (!ConsumeInt(&id))
        break;
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ConsumeString(&value_str))
        break;
      UpdateTrackTitle(id, value_str);
    } else if (TryExpectAndConsumeLiteral(kLengthLiteral)) {
      if (!ConsumeInt(&id))
        break;
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ConsumeInt(&value_int))
        break;
      UpdateTrackLength(id, value_int);
    } else if (TryExpectAndConsumeLiteral(kPlaylistLiteral)) {
      // Do nothing..
    } else if (TryExpectAndConsumeLiteral(kNumEntriesLiteral)) {
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ConsumeInt(&num_entries))
        break;
    } else if (TryExpectAndConsumeLiteral(kVersionLiteral)) {
      if (!ExpectAndConsumeChar('='))
        break;
      if (!ConsumeInt(&version))
        break;
    } else {
      EatWhitespace();
      if (CanConsume(1))
        LOG(ERROR) << "Cannot parse.. bailing";
      break;
    }
  }
}

PlaylistPLS::PlaylistPLS(base::FilePath& file)
    : start_pos_(NULL),
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
    : start_pos_(NULL),
      pos_(NULL),
      end_pos_(NULL),
      index_(0),
      line_number_(0),
      input_(input) {
  Parse();
}

PlaylistPLS::~PlaylistPLS() {
}
