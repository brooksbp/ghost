// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/command_line.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"

#include "track.h"
#include "library.h"
#include "player.h"
#include "player_ui.h"
#include "gst_player.h"
#include "playlist_pls.h"

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

struct MainThreadGlobals {
  MainThreadGlobals()
      : player(new Player()),
        gst_player(new GstPlayer()),
        player_ui(new PlayerUi()) {
  }

  scoped_refptr<Player> player;
  scoped_refptr<GstPlayer> gst_player;
  scoped_refptr<PlayerUi> player_ui;
};

base::LazyInstance<MainThreadGlobals>::Leaky g_globals =
    LAZY_INSTANCE_INITIALIZER;

base::AtExitManager exit_manager;

// All shutdown work should start from here. Once this function returns, the
// main RunLoop::QuitClosure will be posted and the main function will return.
//
// This function should *NOT* be called directly, use InitiateShutdown()
// instead.
void DoShutdown(void) {
  LOG(INFO) << "running DoShutdown";
  g_globals.Get().player_ui = NULL;
  g_globals.Get().gst_player = NULL;

  // Notify observers first?
  Library::GetInstance()->DeleteInstance();

  g_globals.Get().player = NULL;
}

base::Callback<void(void)> g_post_shutdown_tasks_cb;

void PostShutdownTasks(const scoped_refptr<base::TaskRunner>& task_runner,
                       base::Closure quit_closure) {
  task_runner->PostTask(FROM_HERE, base::Bind(&DoShutdown));
  task_runner->PostTask(FROM_HERE, quit_closure);
  LOG(INFO) << "posted shutdown tasks";
}

static bool shutdown_once = false;

void InitiateShutdown(void) {
  if (!shutdown_once) {
    shutdown_once = true;
    g_post_shutdown_tasks_cb.Run();
  }
}

#if defined(OS_POSIX)
void sig_handler(int s) {
  InitiateShutdown();
}
#endif

void MainInit(void) {
  Library::CreateInstance(); // #1

  scoped_refptr<Player> player = g_globals.Get().player;

  scoped_refptr<GstPlayer> gst_player = g_globals.Get().gst_player;
  scoped_refptr<PlayerUi> player_ui = g_globals.Get().player_ui;

  //MainWindow* main_window = player_ui->GetMainWindow();
  scoped_refptr<MainWindow> main_window = player_ui->GetMainWindow();

  player->Init(gst_player, main_window);

  main_window->Init(player);

  base::FilePath dir(FILE_PATH_LITERAL("../test-data/"));
  Library::GetInstance()->Init(dir);


  gst_player->OnPositionUpdated = base::Bind(&MainWindow::OnPositionUpdated, main_window);
  gst_player->OnDurationUpdated = base::Bind(&MainWindow::OnDurationUpdated, main_window);

  main_window->show();


  // base::FilePath playlist;
  // if (cl->HasSwitch("pls")) {
  //   playlist = cl->GetSwitchValuePath("pls");
  //   // TODO(brbrooks) file exists check

  //   PlaylistPLS pls(playlist);
  //   if (pls.tracks_.size() > 0) {
  //     gst_player->Load(pls.tracks_[0].file_);
  //     gst_player->Play();
  //   }
  // }
}

#if defined(OS_POSIX)
int main(int argc, const char* argv[]) {
#elif defined(OS_WIN)
int _tmain(int argc, _TCHAR* argv[]) {
#endif

#if defined(OS_POSIX)
  XInitThreads();

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

  // Initialize logging.
  logging::SetLogItems(true, true, true, true);
  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_ALL;
  logging_settings.log_file = FILE_PATH_LITERAL("ghost-debug.log");
  logging_settings.lock_log = logging::LOCK_LOG_FILE;
  logging_settings.delete_old = logging::DELETE_OLD_LOG_FILE;
  logging::InitLogging(logging_settings);

  // Process command line switches.
  if (cl->HasSwitch("help")) {
    std::cout << "\t-help" << std::endl;
    std::cout << "\t--dir=/path/to/library/" << "\t\tmusic library path."
              << std::endl;
    std::cout << "\t--pls=/path/to/playlist.pls" << "\tplay/stream playlist."
              << std::endl;
    return 0;
  }

  // User Preferences.
#if defined(OS_WIN)
  base::FilePath dir(FILE_PATH_LITERAL("../../test-data/"));
#else
  base::FilePath dir(FILE_PATH_LITERAL("../test-data/"));
#endif
  if (cl->HasSwitch("dir"))
    dir = cl->GetSwitchValuePath("dir");

  MainInit();

  base::RunLoop run_loop;

  g_post_shutdown_tasks_cb = base::Bind(
      &PostShutdownTasks, main_loop.message_loop_proxy(), run_loop.QuitClosure());

  run_loop.Run();

  LOG(INFO) << "EndOfMain";
  return 0;
}
