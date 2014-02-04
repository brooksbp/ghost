#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"
#include "library.h"
#include "audio_manager.h"
#include "main_window.h"

#include <QtWidgets/QApplication>

#include <iostream>


int main(int argc, const char* argv[]) {
  CommandLine cl(argc, argv);

  base::FilePath dir("../test-data/");
  if (cl.HasSwitch("dir")) {
    dir = cl.GetSwitchValuePath("dir");
  }
  
  Library library(dir);
  library.PrintTracks();
  
  AudioManager audio_manager;
  audio_manager.PlayMp3File(library.GetATrack()->file_path_.value().c_str());

  QApplication app(argc, NULL);

  MainWindow main_window;
  main_window.SetAudioManager(&audio_manager);
  main_window.show();
  
  return app.exec();  
}
