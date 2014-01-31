#include <iostream>

#include "command_line.h"
#include "file_path.h"

int main(int argc, const char* argv[]) {
  CommandLine cl(argc, argv);

  if (!cl.HasSwitch("dir")) {
    std::cout << "usage: " << argv[0] << " --dir=<music-directory>" << std::endl;
    return 0;
  }
  base::FilePath dir = cl.GetSwitchValuePath("dir");
  std::cout << "dir=" << dir.value() << std::endl;
}
