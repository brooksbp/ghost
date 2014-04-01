// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#ifndef COMMAND_LINE_PREF_STORE_H_
#define COMMAND_LINE_PREF_STORE_H_

#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/prefs/value_map_pref_store.h"
#include "base/values.h"

// This PrefStore keeps track of preferences set by command-line switches.
class CommandLinePrefStore : public ValueMapPrefStore {
 public:
  explicit CommandLinePrefStore(const CommandLine* command_line);

 protected:
  virtual ~CommandLinePrefStore();

 private:
  struct StringSwitchToPreferenceMapEntry {
    const char* switch_name;
    const char* preference_path;
  };

  // Weak reference.
  const CommandLine* command_line_;

  // Mappings of command line switches to prefs.
  static const StringSwitchToPreferenceMapEntry string_switch_map_[];

  DISALLOW_COPY_AND_ASSIGN(CommandLinePrefStore);
};

#endif  // COMMAND_LINE_PREF_STORE_H_
