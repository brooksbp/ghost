// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef UI_UI_H_
#define UI_UI_H_

#include "ui/main_window.h"

#include "base/logging.h"
#include "base/memory/ref_counted.h"

#include <QtWidgets/QApplication>

class Ui : public base::RefCountedThreadSafe<Ui> {
 public:
  Ui();
  ~Ui();

 private:
  QApplication* app;
  MainWindow* main_window;
};

#endif  // UI_UI_H_
