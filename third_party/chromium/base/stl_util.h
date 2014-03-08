// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Derived from google3/util/gtl/stl_util.h

#ifndef BASE_STL_UTIL_H_
#define BASE_STL_UTIL_H_

#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

// #include "base/logging.h"


// Test to see if a set, map, hash_set or hash_map contains a particular key.
// Returns true if the key is in the collection.
template <typename Collection, typename Key>
bool ContainsKey(const Collection& collection, const Key& key) {
  return collection.find(key) != collection.end();
}


#endif  // BASE_STIL_UTIL_H_
