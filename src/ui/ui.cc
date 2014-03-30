// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "ui/ui.h"

// static
Ui* Ui::instance_ = NULL;

static int dummy_argc = 1;
static char* dummy_argv = const_cast<char*>("ghost");

Ui::Ui() {
  app = new QApplication(dummy_argc, &dummy_argv);

  main_window = new MainWindow();

  main_window->Init();
  main_window->show();

  LOG(INFO) << "Ui()";
}

Ui::~Ui() {
  LOG(INFO) << "~PlayerUi()";
  app->closeAllWindows();

  main_window->Shutdown();

  //delete main_window;
  //delete app;
}

// static
void Ui::CreateInstance() {
  if (!instance_) {
    instance_ = new Ui;
  }
}

// static
Ui* Ui::GetInstance() {
  DCHECK(instance_) << "Ui::CreateInstance must be called before getting "
      "the instance of Ui.";
  return instance_;
}

// static
void Ui::DeleteInstance() {
  delete instance_;
  instance_ = NULL;
}
