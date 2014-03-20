// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "track.h"

class Library : public base::RefCountedThreadSafe<Library> {
 public:

  class LibraryObserver {
   public:
    virtual void OnBeginImport(Library* library) = 0;
    virtual void OnFinishImport(Library* library) = 0;
  };

  void AddObserver(LibraryObserver* obs) {
    observer_list_.AddObserver(obs);
  }

  Library();
  ~Library();

  void Init(const base::FilePath& path);

  // Returns the Track located at |index|.
  Track* GetTrack(int index);

  // Returns the Track located at |index| and sets current_track_.
  Track* GetTrackForPlaying(int index);

  // Returns the Track located at current_track_;
  Track* GetCurrentTrack();

  int GetNumTracks(void);

  void PrintTracks(void);

 private:
  base::FilePath root_path_;

  // Current index into |tracks|.
  int current_track_;

  void Import(void);
  void NotifyImportDone(void);

  ObserverList<LibraryObserver> observer_list_;
};

#endif  // LIBRARY_H_
