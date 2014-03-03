/*
 *  Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *  Copyright (C) 2008 Collabora Ltd.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef G_OWN_PTR_H_
#define G_OWN_PTR_H_

#include <algorithm>

#include <gio/gio.h>
#include <glib.h>

namespace base {

template <typename T> inline void freeOwnedGPtr(T* ptr);
template <> void freeOwnedGPtr<GError>(GError*);

template <typename T> class GOwnPtr {
  // MAKE_NONCOPYABLE(GOwnPtr);
 public:
  explicit GOwnPtr(T* ptr = 0) : ptr_(ptr) { }
  ~GOwnPtr() { FreeOwnedGPtr(ptr_); }

  T* get() const { return ptr_; }
  T* release() {
    T* ptr = ptr_;
    ptr_ = 0;
    return ptr;
  }

  T*& outPtr() {
    // ASSERT(!ptr_);
    return ptr_;
  }

  void set(T* ptr) {
    // ASSERT(!ptr || ptr_ != ptr);
    freeOwnedGPtr(ptr_);
    ptr_ = ptr;
  }

  void clear() {
    T* ptr = ptr_;
    ptr_ = 0;
    freeOwnedGPtr(ptr);
  }

  T& operator*() const {
    // ASSERT(ptr_);
    return *ptr_;
  }

  T* operator->() const {
    // ASSERT(ptr_);
    return ptr_;
  }

  bool operator!() const { return !ptr_; }
  
  // This conversion opreator allows implicit conversion to bool but not to
  // other integer types.
  typedef T* GOwnPtr::*UnspecifiedBoolType;
  operator UnspecifiedBoolType() const {
    return ptr_ ? &GOwnPtr::ptr_ : 0;
  }

  void swap(GOwnPtr& o) { std::swap(ptr_, o.ptr_); }

 private:
  T* ptr_;
};

template <typename T> inline void swap(GOwnPtr<T>& a, GOwnPtr<T>& b) {
  a.swap(b);
}

template <typename T, typename U>
inline bool operator==(const GOwnPtr<T>& a, U* b) {
  return a.get() == b;
}
template <typename T, typename U>
inline bool operator==(T* a, const GOwnPtr<T>& b) {
  return a == b.get();
}

template <typename T, typename U>
inline bool operator!=(const GOwnPtr<T>& a, U* b) {
  return a.get() != b;
}
template <typename T, typename U>
inline bool operator!=(T* a, const GOwnPtr<T>& b) {
  return a != b.get();
}

template <typename T>
inline typename GOwnPtr<T>::PtrType getPtr(const GOwnPtr<T>& p) {
  return p.get();
}

template <typename T> inline void freeOwnedPtr(T* ptr) {
  g_free(ptr);
}

}  // namespace base

using base::GOwnPtr;

#endif  // G_OWN_PTR_H_
