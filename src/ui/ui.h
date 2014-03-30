// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef UI_UI_H_
#define UI_UI_H_

#include "ui/main_window.h"

#include "base/logging.h"

#include <QtWidgets/QApplication>

class Ui {
 public:
  Ui();
  ~Ui();

  static void CreateInstance();
  static Ui* GetInstance();
  static void DeleteInstance();

 private:
  QApplication* app;
  MainWindow* main_window;

  static Ui* instance_;
  
  DISALLOW_COPY_AND_ASSIGN(Ui);
};

#endif  // UI_UI_H_
