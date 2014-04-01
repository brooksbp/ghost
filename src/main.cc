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

#include "blocking_pool.h"
#include "command_line_switches.h"
#include "gst_player.h"
#include "library.h"
#include "playlist_pls.h"
#include "prefs.h"
#include "track.h"
#include "ui/ui.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(OS_POSIX)
#include <X11/Xlib.h>
#endif
#if defined(OS_WIN)
#include <Windows.h>
#include <tchar.h>
#endif

base::AtExitManager exit_manager;

// All shutdown work should start from here. Once this function returns, the
// main RunLoop::QuitClosure will be posted and the main function will return.
//
// This function should *NOT* be called directly, use InitiateShutdown()
// instead.
void DoShutdown(void) {
  LOG(INFO) << "running DoShutdown";
  // TODO(brbrooks) Add OnWillShutdown() to observers?
  Ui::GetInstance()->DeleteInstance();
  GstPlayer::GetInstance()->DeleteInstance();
  Library::GetInstance()->DeleteInstance();
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
void setup_sig_handler() {
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
}
#endif

void initialize_logging() {
  logging::SetLogItems(true, true, true, true);

  logging::LoggingSettings logging_settings;
  logging_settings.logging_dest = logging::LOG_TO_ALL;
  logging_settings.log_file = FILE_PATH_LITERAL("ghost.log");
  logging_settings.lock_log = logging::LOCK_LOG_FILE;
  logging_settings.delete_old = logging::DELETE_OLD_LOG_FILE;

  logging::InitLogging(logging_settings);
}

#if defined(OS_POSIX)
int main(int argc, const char* argv[]) {
#elif defined(OS_WIN)
int _tmain(int argc, _TCHAR* argv[]) {
#endif
#if defined(OS_POSIX)
  XInitThreads();
#endif

  // Initialize the commandline singleton from the environment.
#if defined(OS_WIN)
  CommandLine::Init(argc, (const char *const *) argv);
#elif defined(OS_POSIX)
  CommandLine::Init(argc, argv);
#endif

#if defined(OS_POSIX)
  setup_sig_handler();
#endif

  base::MessageLoop main_loop(base::MessageLoop::TYPE_UI);
  BlockingPool::CreateInstance();

  initialize_logging();

  // Process *early* command line switches.
  CommandLine* cl = CommandLine::ForCurrentProcess();
  if (cl->HasSwitch(switches::kHelp) || cl->HasSwitch(switches::kHelpShort)) {
    switches::Usage();
    return 0;
  }


  Prefs::CreateInstance();

  LOG(INFO) << "dywm="
            << Prefs::GetInstance()->prefs()->GetBoolean("do.you.want.more");
  //LOG(INFO) << "dywm=" << prefs->GetBoolean("do.you.want.more");
  // //prefs->SetBoolean("do.you.want.more", false);

  // base::FilePath path(prefs->GetFilePath(prefs::kLibraryDir));
  // LOG(INFO) << prefs::kLibraryDir << "=" << path.value();

  // prefs->CommitPendingWrite();




  Library::CreateInstance();
  GstPlayer::CreateInstance();
  Ui::CreateInstance();

  // Import from |dir|.
#if defined(OS_WIN)
  base::FilePath dir(FILE_PATH_LITERAL("../../test-data/"));
#else
  base::FilePath dir(FILE_PATH_LITERAL("../test-data/"));
#endif
  if (cl->HasSwitch("dir"))
    dir = cl->GetSwitchValuePath("dir");
  Library::GetInstance()->Init(dir);

  // Play a playlist if passed in from command line..?
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

  base::RunLoop run_loop;

  g_post_shutdown_tasks_cb = base::Bind(
      &PostShutdownTasks, main_loop.message_loop_proxy(), run_loop.QuitClosure());

  run_loop.Run();

  LOG(INFO) << "Returning from main loop";

  Prefs::GetInstance()->DeleteInstance();  
  BlockingPool::GetInstance()->DeleteInstance();

  return 0;
}
