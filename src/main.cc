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

#if defined(OS_POSIX)
#include <X11/Xlib.h>
#endif
#if defined(OS_WIN)
#include <Windows.h>
#endif

QApplication* app;

#if defined(OS_POSIX)
void sig_handler(int s) {
  qDebug() << "exiting..";
  app->quit();
}
#endif

#if defined(OS_POSIX)
int main(int argc, const char* argv[]) {
#elif defined(OS_WIN)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
  int argc = 0;
#endif
#if defined(OS_POSIX)
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
#endif

  // Initialize the commandline singleton from the environment.
  CommandLine::Init(0, NULL);
  CommandLine* cl = CommandLine::ForCurrentProcess();

  base::FilePath dir(FILE_PATH_LITERAL("../test-data/"));
  if (cl->HasSwitch("dir")) {
    dir = cl->GetSwitchValuePath("dir");
  }

#if defined(OS_POSIX)
  XInitThreads();
#endif
  
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
