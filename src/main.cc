#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"
#include "library.h"
#include "audio_manager.h"

#include <QtWidgets>

#include <iostream>


int main(int argc, const char* argv[]) {
  CommandLine cl(argc, argv);

  base::FilePath dir("../test-data/");
  if (cl.HasSwitch("dir")) {
    dir = cl.GetSwitchValuePath("dir");
  }

  QApplication app(argc, NULL);
  QWidget window;
  window.setWindowTitle("Ghost");
  window.show();
  
  Library library(dir);
  library.PrintTracks();
  
  AudioManager audio_manager;

  std::cout << "location = " << library.GetATrack()->file_path_.value().c_str() << std::endl;
  audio_manager.PlayMp3File(library.GetATrack()->file_path_.value().c_str());
  
  return app.exec();  
}
