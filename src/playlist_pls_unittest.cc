#include "playlist_pls.h"

#include <gtest/gtest.h>

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

  for (size_t i = 0; i < playlist.tracks_.size(); ++i) {
    std::string file   = playlist.tracks_[i].file_;
    std::string title  = playlist.tracks_[i].title_;
    int length         = playlist.tracks_[i].length_;
    switch (i) {
      case 0:
        EXPECT_EQ(file,   "http://108.61.73.118:14008");
        EXPECT_EQ(title,  "(#1 - 92/1000) 181.fm - Rock 181 (Active Rock)");
        EXPECT_EQ(length, -1);
        break;
      case 1:
        EXPECT_EQ(file,   "http://108.61.73.117:14008");
        EXPECT_EQ(title,  "(#2 - 93/1000) 181.fm - Rock 181 (Active Rock)");
        EXPECT_EQ(length, -1);
        break;
      default:
        break;
    }
  }
}
