#include "base/command_line.h"
#include "base/file_path.h"

#include "track.h"
#include "library.h"
#include "audio_manager.h"
#include "main_window.h"

#include <QtWidgets/QApplication>

#include <functional>


int main(int argc, const char* argv[]) {
  CommandLine cl(argc, argv);

  base::FilePath dir("../test-data/");
  if (cl.HasSwitch("dir")) {
    dir = cl.GetSwitchValuePath("dir");
  }

  AudioManager audio_manager;
  Library library(dir, &audio_manager);

  audio_manager.eosCallback = std::bind(&Library::EndOfStream, library);
  
  QApplication app(argc, NULL);

  MainWindow main_window;
  main_window.Initialize(&library);
  main_window.show();
  
  return app.exec();  
}
