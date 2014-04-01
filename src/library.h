// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef LIBRARY_H_
#define LIBRARY_H_

#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list.h"

#include "track.h"

class PrefRegistrySimple;

class Library : public base::RefCountedThreadSafe<Library> {
 public:
  Library();
  ~Library();

  static void CreateInstance();
  static Library* GetInstance();
  static void DeleteInstance();

  class LibraryObserver {
   public:
    virtual void OnBeginImport(Library* library) = 0;
    virtual void OnFinishImport(Library* library) = 0;
  };

  void AddObserver(LibraryObserver* observer);
  void RemoveObserver(LibraryObserver* observer);

  void ImportFromLibraryDir();

  Track* GetTrack(int index);
  Track* GetTrackForPlaying(int index);
  Track* GetCurrentTrack();
  int GetNumTracks(void);

  void PrintTracks(void);

  static void RegisterPrefs(PrefRegistrySimple* registry);

 private:
  void Import(void);
  void NotifyImportDone(void);

  ObserverList<LibraryObserver> observers_;

  static scoped_refptr<Library> instance_;

  int current_track_;

  base::FilePath root_path_;

  DISALLOW_COPY_AND_ASSIGN(Library);
};

#endif  // LIBRARY_H_
