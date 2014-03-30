// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef PLAYER_UI_H_
#define PLAYER_UI_H_

#include "main_window.h"

#include "base/logging.h"
#include "base/memory/ref_counted.h"

#include <QtWidgets/QApplication>

class PlayerUi : public base::RefCountedThreadSafe<PlayerUi> {
 public:
  PlayerUi();
  ~PlayerUi();

 private:
  QApplication* app;
  scoped_refptr<MainWindow> main_window;
};

#endif  // PLAYER_UI_H_
