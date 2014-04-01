// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "command_line_pref_store.h"

#include "command_line_switches.h"
#include "pref_names.h"

const CommandLinePrefStore::StringSwitchToPreferenceMapEntry
    CommandLinePrefStore::string_switch_map_[] = {
  { switches::kLibraryDir, prefs::kLibraryDir },
};

CommandLinePrefStore::CommandLinePrefStore(const CommandLine* command_line)
    : command_line_(command_line) {
  for (size_t i = 0; i < arraysize(string_switch_map_); ++i) {
    if (command_line_->HasSwitch(string_switch_map_[i].switch_name)) {
      base::Value* value = base::Value::CreateStringValue(
          command_line_->GetSwitchValueASCII(
              string_switch_map_[i].switch_name));
      SetValue(string_switch_map_[i].preference_path, value);
    }
  }
}

CommandLinePrefStore::~CommandLinePrefStore() {
}
