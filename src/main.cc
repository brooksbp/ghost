// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "base/logging.h"
#include "base/files/file_path.h"

#include "track.h"
#include "library.h"
#include "gst_player.h"
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
  LOG(INFO) << "Goodbye.";
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

  // Initialize the commandline singleton from the environment.
  CommandLine::Init(argc, argv);
  CommandLine* cl = CommandLine::ForCurrentProcess();

  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_ALL;
  logging_settings.log_file = "ghost-debug.log";
  logging_settings.lock_log = logging::LOCK_LOG_FILE;
  logging_settings.delete_old = logging::DELETE_OLD_LOG_FILE;
  logging::InitLogging(logging_settings);

#if defined(OS_POSIX)
  XInitThreads();
#endif

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

  Player player;
  GstPlayer gst_player;
  Library library;
  app = new QApplication(argc, NULL);  
  MainWindow main_window;

  player.Init(&library, &gst_player, &main_window);
  library.Init(dir);
  main_window.Init(&player);

  gst_player.eosCallback = std::bind(&Library::EndOfStream, library);
  gst_player.PlaybackProgressCallback =
      [&] (int64_t one, int64_t two) { main_window.PlaybackProgress(one, two); };

  main_window.show();

  base::FilePath playlist;
  if (cl->HasSwitch("pls")) {
    playlist = cl->GetSwitchValuePath("pls");
    // TODO(brbrooks) exists check

    PlaylistPLS pls(playlist);
#if 0 // FIXME(brbrooks) use Load() and Play() and convert to correct URI
    if (pls.tracks_.size() > 0)
      gst_player.PlayURI(pls.tracks_[0].file_);
#endif
  }

  return app->exec();
}
