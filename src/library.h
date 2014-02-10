#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "base/file_path.h"
#include "track.h"
#include "audio_manager.h"

class Library {
 public:
  Library(const base::FilePath& root_path, AudioManager* audio_manager);
  ~Library();

  Track* GetTrack(int index);
  int GetNumTracks(void);

  void PrintTracks(void);

  void Play(int index);
  
  void EndOfStream(void);
  
 private:
  base::FilePath root_path_;
  AudioManager* audio_manager_;

  // Current index into |tracks|.
  int current_index_;
};

#endif  // LIBRARY_H_
