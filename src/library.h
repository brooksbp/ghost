#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "base/file_path.h"
#include "track.h"

class Library {
 public:
  Library(const base::FilePath& root_path);
  ~Library();

  Track* GetATrack(void);

  void PrintTracks(void);
  
 private:
  base::FilePath root_path_;
};

#endif  // LIBRARY_H_
