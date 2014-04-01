// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "prefs.h"

#include "base/prefs/json_pref_store.h"
#include "base/prefs/pref_registry_simple.h"
#include "base/prefs/pref_service_factory.h"

#include "blocking_pool.h"
#include "command_line_pref_store.h"
#include "pref_names.h"

// XXX: include files that implement methods for registering prefs

// static
Prefs* Prefs::instance_ = NULL;

Prefs::Prefs() {
  // 1. Register all the prefs
  scoped_refptr<PrefRegistrySimple> registry = new PrefRegistrySimple;

  // XXX: call the register methods here
  registry->RegisterBooleanPref("do.you.want.more", true);
  registry->RegisterFilePathPref(prefs::kLibraryDir, base::FilePath());

  // 2. Setup USER_DATA prefs file
  base::FilePath prefs_file(FILE_PATH_LITERAL("Preferences"));
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner =
      JsonPrefStore::GetTaskRunnerForFile(prefs_file,
                                          BlockingPool::GetBlockingPool());

  // 3. Create a pref service
  base::PrefServiceFactory pref_factory;
  pref_factory.SetUserPrefsFile(prefs_file, sequenced_task_runner.get());

  prefs_ = pref_factory.Create(registry.get());

  // 4. Merge in command line prefs
  prefs_->UpdateCommandLinePrefStore(
      new CommandLinePrefStore(CommandLine::ForCurrentProcess()));
}
Prefs::~Prefs() {
}

// static
void Prefs::CreateInstance() {
  if (!instance_)
    instance_ = new Prefs;
}
// static
Prefs* Prefs::GetInstance() {
  DCHECK(instance_) << "Prefs::CreateInstance must be called before getting "
      "the instance of Prefs.";
  return instance_;
}
// static
void Prefs::DeleteInstance() {
  delete instance_;
  instance_ = NULL;
}

PrefService* Prefs::prefs() {
  DCHECK(instance_);
  return prefs_.get();
}
