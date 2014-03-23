// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef PLAYLIST_PLS_H_
#define PLAYLIST_PLS_H_

#include <string>
#include <vector>
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

  std::vector<Track> tracks_;

  std::map<int, Track> tracks2_;

  PlaylistPLS(base::FilePath& file);
  PlaylistPLS(std::string& input);
  ~PlaylistPLS();

 private:

  void Parse();

  void Parse2();

  // Quick check that the stream has capacity to consume |length| more bytes.
  bool CanConsume(int length);
  // The basic way to consume a single character in the stream. Consumes one
  // byte of the input stream and returns a pointer to the rest of it.
  const char* NextChar();
  // Performs the equivalent of NextChar N times.
  void NextNChars(int n);
  // Consumes whitespce characters.
  void EatWhitespace();

  bool ExpectAndConsumeChar(char c);
  bool ExpectAndConsumeInt(int *i);
  bool ExpectAndConsumeString(std::string* str);

  bool ConsumeLiteral(const char* literal);

  bool StringsAreEqual(const char* a, const char* b, size_t len);


  bool parsed_;

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
