// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef PLAYLIST_PLS_H_
#define PLAYLIST_PLS_H_

#include <string>
#include <map>

#include "base/files/file_path.h"
#include "base/logging.h"

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
    Track() : length_(0) {
      file_.clear();
      title_.clear();
    }
  };
  void UpdateTrackFile(int id, std::string& value);
  void UpdateTrackTitle(int id, std::string& value);
  void UpdateTrackLength(int id, int value);

  std::map<int, Track> tracks_;

  PlaylistPLS(base::FilePath& file);
  PlaylistPLS(std::string& input);
  ~PlaylistPLS();

 private:

  void Parse();

  // Quick check that the stream has capacity to consume |length| more bytes.
  bool CanConsume(int length);
  // The basic way to consume a single character in the stream. Consumes one
  // byte of the input stream and returns a pointer to the rest of it.
  const char* NextChar();
  // Performs the equivalent of NextChar N times.
  void NextNChars(int n);
  // Consumes whitespce characters.
  void EatWhitespace();

  // Returns true if successfully consumes char |c|, false otherwise.
  bool ExpectAndConsumeChar(char c);
  // Returns true if successfully consumes |literal|, false otherwise. This
  // version does not log a message if unsuccessful.
  bool TryExpectAndConsumeLiteral(const char* literal);

  // Returns true if successfully consumes an integer and copies it into *|i|,
  // false otherwise.
  bool ConsumeInt(int *out);
  // Returns true if successfully consumes a string and copies it into *|str|,
  // false otherwise.
  bool ConsumeString(std::string* out);

  // Case-sensitive compare.
  bool StringsAreEqual(const char* a, const char* b, size_t len);

  // Pointer to the start of the input data.
  const char* start_pos_;
  // Pointer to the current position in the input data. Equivalent to
  // |start_pos_ + index_|.
  const char* pos_;
  // Pointer to the last character of the input data.
  const char* end_pos_;

  // The index in the input stream to which the parser is wound.
  int index_;
  
  // The line number that the parser is currently at.
  int line_number_;
  
  // The input data to be parsed.
  std::string input_;

  base::FilePath file_;
};

#endif  // PLAYLIST_PLS_H_
