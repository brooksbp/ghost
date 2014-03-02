#include "playlist_pls.h"

#include <sstream>

#include "base/basictypes.h"
#include "base/file_util.h"
#include "base/strings/string_util.h"

void PlaylistPLS::Parse() {
  std::istringstream iss(contents_);
  std::string line;

  uint8 nentries = 0;
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
      nentries = std::stoi(v);

      // FIXME(brbrooks) once we know how many entries there are, pre-populate
      // the vector with empty entries so that we can index into them later.
      // It might be better to use a hash table for unencumbered indexing and
      // then convert it to a vector.
      PlaylistPLS::Track empty_track("", "", 0);
      for (int i = 0; i < nentries; ++i) {
        tracks_.push_back(empty_track);
      }
    } else if (LowerCaseEqualsASCII(k, "version")) {
      version = std::stoi(v);
    } else {
      // Handle either File, Title, or Length.
      size_t f = k.find(f_str);
      size_t t = k.find(t_str);
      size_t l = k.find(l_str);

      if (f != std::string::npos) {
        int n = std::stoi(k.substr(f_str.length(), k.length()));
        if (n >= 1 && n <= nentries)
          tracks_[n - 1].file_ = v;
      } else if (t != std::string::npos) {
        int n = std::stoi(k.substr(t_str.length(), k.length()));
        if (n >= 1 && n <= nentries)
          tracks_[n - 1].title_ = v;
      } else if (l != std::string::npos) {
        int n = std::stoi(k.substr(l_str.length(), k.length()));
        if (n >= 1 && n <= nentries)
          tracks_[n - 1].length_ = std::stoi(v);
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
      file_(file) {
  contents_.clear();
  if (!base::ReadFileToString(base::MakeAbsoluteFilePath(file_), &contents_))
    return;
  Parse();
}

PlaylistPLS::PlaylistPLS(std::string& contents)
    : parsed_(false),
      contents_(contents) {
  Parse();
}

PlaylistPLS::~PlaylistPLS() {
}
