#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"
#include "library.h"
#include "audio_manager.h"
#include "main_window.h"
#include "timer.h"

#include <QtWidgets/QApplication>

#include <functional>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

QApplication* app;

void sig_handler(int s) {
  qDebug() << "exiting..";
  app->quit();
}

int main(int argc, const char* argv[]) {
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  
  CommandLine cl(argc, argv);

  base::FilePath dir("../test-data/");
  if (cl.HasSwitch("dir")) {
    dir = cl.GetSwitchValuePath("dir");
  }

  AudioManager audio_manager;
  Library library(dir, &audio_manager);

  audio_manager.eosCallback = std::bind(&Library::EndOfStream, library);

  app = new QApplication(argc, NULL);

  MainWindow main_window;
  main_window.Initialize(&library);
  main_window.show();

  return app->exec();
}
