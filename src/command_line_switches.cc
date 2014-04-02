// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "command_line_switches.h"

#include <iostream>

namespace switches {

const char kHelp[] = "help";
const char kHelpShort[] = "h";

// Specifies the playlist to be played once the application loads.
const char kPlaylist[] = "playlist";

// Specifies the library directory, which contains audio files.
const char kLibraryDir[] = "library-dir";

// Specifies the user data directory, which is where preferences, etc. is
// stored.
const char kUserDataDir[] = "user-data-dir";

void Usage() {
  std::cout << "    -" << kHelp << ", -" << kHelpShort << "\n"
            << "\n"
            << "    --" << kPlaylist << "=\n"
            << "\tPLS file to play (typically for internet streaming) once application loads.\n"
            << "\n"
            << "    --" << kLibraryDir << "=\n"
            << "\tPath to directiory that contains all your audio files.\n"
            << "\n"
            << "    --" << kUserDataDir << "=\n"
            << "\tPath to directiory that is used to store user data.\n"
            << "\n";
}

}  // namespace switches
