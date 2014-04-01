// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef PREFS_H_
#define PREFS_H_

#include "base/prefs/pref_service.h"

class Prefs {
 public:
  Prefs();
  ~Prefs();

  static void CreateInstance();
  static Prefs* GetInstance();
  static void DeleteInstance();

  PrefService* prefs();

 private:
  scoped_ptr<PrefService> prefs_;

  static Prefs* instance_;

  DISALLOW_COPY_AND_ASSIGN(Prefs);
};

#endif  // PREFS_H_
