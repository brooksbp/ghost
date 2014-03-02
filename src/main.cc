#include "base/command_line.h"
#include "base/files/file_path.h"

#include "track.h"
#include "library.h"
#include "audio_manager.h"
#include "main_window.h"
#include "timer.h"
#include "playlist_pls.h"

#include <QtWidgets/QApplication>

#include <functional>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#if defined(OS_POSIX)
#include <X11/Xlib.h>
#endif
#if defined(OS_WIN)
#include <Windows.h>
#include <tchar.h>
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
int _tmain(int argc, _TCHAR* argv[]) {
#endif
#if defined(OS_POSIX)
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
#endif

#if defined(OS_POSIX)
  XInitThreads();
#endif

  // Initialize the commandline singleton from the environment.
  CommandLine::Init(argc, argv);
  CommandLine* cl = CommandLine::ForCurrentProcess();

  if (cl->HasSwitch("help")) {
    std::cout << "\t-help" << std::endl;
    std::cout << "\t--dir=/path/to/library/" << "\t\tmusic library path."
              << std::endl;
    std::cout << "\t--pls=/path/to/playlist.pls" << "\tplay/stream playlist."
              << std::endl;
    return 0;
  }

#if defined(OS_WIN)
  base::FilePath dir(FILE_PATH_LITERAL("../../test-data/"));
#else
  base::FilePath dir(FILE_PATH_LITERAL("../test-data/"));
#endif
  if (cl->HasSwitch("dir"))
    dir = cl->GetSwitchValuePath("dir");


  app = new QApplication(argc, NULL);  
  MainWindow main_window;

  AudioManager audio_manager;
  Library library;

  audio_manager.eosCallback = std::bind(&Library::EndOfStream, library);
  audio_manager.PlaybackProgressCallback =
      [&] (int64_t one, int64_t two) { main_window.PlaybackProgress(one, two); };

  library.Init(dir, &audio_manager);
  std::cout << "n tracks = " << library.GetNumTracks() << std::endl;

  main_window.Init(&library);
  main_window.show();

  base::FilePath playlist;
  if (cl->HasSwitch("pls")) {
    playlist = cl->GetSwitchValuePath("pls");
    // TODO(brbrooks) exists check

    PlaylistPLS pls(playlist);
    if (pls.tracks_.size() > 0)
      audio_manager.PlayURI(pls.tracks_[0].file_);
  }

  return app->exec();
}
