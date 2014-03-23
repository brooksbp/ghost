// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"

#include "track.h"
#include "library.h"
#include "gst_player.h"
#include "main_window.h"
#include "playlist_pls.h"

#include <QtWidgets/QApplication>

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
  delete app;
  // TODO(brbrooks) run_loop->Quit()
}
#endif

base::AtExitManager exit_manager;

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

  base::MessageLoop main_loop(base::MessageLoop::TYPE_UI);

  // Initialize the commandline singleton from the environment.
#if defined(OS_WIN)
  CommandLine::Init(argc, (const char *const *) argv);
#elif defined(OS_POSIX)
  CommandLine::Init(argc, argv);
#endif
  CommandLine* cl = CommandLine::ForCurrentProcess();

  logging::SetLogItems(true, true, true, true);
  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_ALL;
  logging_settings.log_file = FILE_PATH_LITERAL("ghost-debug.log");
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
  scoped_refptr<GstPlayer> gst_player = new GstPlayer();
  scoped_refptr<Library> library = new Library();
  app = new QApplication(argc, NULL);
  scoped_refptr<MainWindow> main_window = new MainWindow();

  player.Init(library, gst_player, main_window);

  main_window->Init(&player);

  library->AddObserver(main_window.get());
  library->Init(dir);

  // Should we really handle all of this on the GUI thread?
  // Use ThreadSafeObserverList instead of this?
  //gst_player.OnEndOfStream = std::bind(&Library::EndOfStream, library);
  gst_player->OnPositionUpdated = base::Bind(&MainWindow::OnPositionUpdated, main_window);
  gst_player->OnDurationUpdated = base::Bind(&MainWindow::OnDurationUpdated, main_window);

  main_window->show();

  base::FilePath playlist;
  if (cl->HasSwitch("pls")) {
    playlist = cl->GetSwitchValuePath("pls");
    // TODO(brbrooks) file exists check

    PlaylistPLS pls(playlist);
    if (pls.tracks_.size() > 0) {
      gst_player->Load(pls.tracks_[0].file_);
      gst_player->Play();
    }
  }
  
#if defined(OS_POSIX)
  base::RunLoop run_loop;
  run_loop.Run();
#elif defined(OS_WIN)
  for (;;) {
    base::RunLoop run_loop;
    run_loop.RunUntilIdle();
    app->processEvents();
  }
#endif
}
