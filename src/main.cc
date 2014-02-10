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

  app = new QApplication(argc, NULL);  

  AudioManager audio_manager;
  Library library(dir, &audio_manager);

  MainWindow main_window;

  audio_manager.eosCallback = std::bind(&Library::EndOfStream, library);
  audio_manager.PlaybackProgressCallback =
      [&] (int64_t one, int64_t two) { main_window.PlaybackProgress(one, two); };
  
  main_window.Initialize(&library);
  main_window.show();

  return app->exec();
}
