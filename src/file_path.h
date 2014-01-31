// base/files/file_path.h

#ifndef BASE_FILES_FILE_PATH_H_
#define BASE_FILES_FILE_PATH_H_

#include <string>

#include "build_config.h"

// An abstraction to isolate users from the differences between native
// pathnames on different platforms.
class FilePath {
 public:
#if defined(OS_POSIX)
  // On most platforms, native pathnames are char arrays, and the encoding
  // may or may not be specified.  On Mac OS X, native pathnames are encoded
  // in UTF-8.
  typedef std::string StringType;
#elif defined(OS_WIN)
  // On Windows, for Unicode-aware applications, native pathnames are wchar_t
  // arrays encoded in UTF-16.
  typedef std::wstring StringType;
#endif

  typedef StringType::value_type CharType;


  FilePath();
  FilePath(const FilePath& that);
  explicit FilePath(const StringType& path);
  ~FilePath();


  const StringType& value() const { return path_; }

  bool empty() const { return path_.empty(); }

  void clear() { path_.clear(); }
  
 private:
  StringType path_;
};

// Macros for string literal initialization of FilePath::CharType[], and for
// using a FilePath::CharType[] in a printf-style format string.
#if defined(OS_POSIX)
#define FILE_PATH_LITERAL(x) x
#define PRFilePath "s"
#define PRFilePathLiteral "%s"
#elif defined(OS_WIN)
#define FILE_PATH_LITERAL(x) L ## x
#define PRFilePath "ls"
#define PRFilePathLiteral L"%ls"
#endif

#endif  // BASE_FILES_FILE_PATH_H_
