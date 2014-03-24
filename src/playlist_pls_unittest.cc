// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "playlist_pls.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(PlaylistPLSTest, MainTest) {
  const char* s =
      "[playlist]\n"
      "numberofentries=2\n"
      "File1=http://108.61.73.118:14008\n"
      "Title1=(#1 - 92/1000) 181.fm - Rock 181 (Active Rock)\n"
      "Length1=-1\n"
      "File2=http://108.61.73.117:14008\n"
      "Title2=(#2 - 93/1000) 181.fm - Rock 181 (Active Rock)\n"
      "Length2=-1\n"
      "Version=2\n";
  std::string pls(s);

  PlaylistPLS playlist(pls);

  EXPECT_EQ(playlist.tracks_.size(), 2);

  typedef std::map<int, PlaylistPLS::Track>::iterator track_iter;

  for (track_iter it = playlist.tracks_.begin(); it != playlist.tracks_.end();
       ++it) {
    PlaylistPLS::Track* track = &it->second;
    switch (it->first) {
      case 1:
        EXPECT_EQ(track->file_,   "http://108.61.73.118:14008");
        EXPECT_EQ(track->title_,  "(#1 - 92/1000) 181.fm - Rock 181 (Active Rock)");
        EXPECT_EQ(track->length_, -1);
        break;
      case 2:
        EXPECT_EQ(track->file_,   "http://108.61.73.117:14008");
        EXPECT_EQ(track->title_,  "(#2 - 93/1000) 181.fm - Rock 181 (Active Rock)");
        EXPECT_EQ(track->length_, -1);
        break;
      default:
        // TODO(brbrooks) assert failure here
        break;
    }
  }
}
