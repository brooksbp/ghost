// base/strings/string_util.h

#ifndef BASE_STRINGS_STRING_UTIL_H_
#define BASE_STRINGS_STRING_UTIL_H_

#include <string>

// HACK
typedef wchar_t char16;

// Contains the set of characters representing whitespace in the corresponding
// encoding. Null-terminated.
extern const wchar_t kWhitespaceWide[];
extern const char16 kWhitespaceUTF16[];
extern const char kWhitespaceASCII[];

// Null-terminated string representing the UTF-8 byte order mark.
extern const char kUtf8ByteOrderMark[];


enum TrimPositions {
  TRIM_NONE     = 0,
  TRIM_LEADING  = 1 << 0,
  TRIM_TRAILING = 1 << 1,
  TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
};

TrimPositions TrimWhitespaceASCII(const std::string& input,
                                  TrimPositions postiions,
                                  std::string* output);

// Deprecated.  This function is only for backward compatibility and calls
// TrimWhitespaceASCII().
TrimPositions TrimWhitespace(const std::string& input,
                             TrimPositions postiions,
                             std::string* output);

#endif  // BASE_STRINGS_STRING_UTIL_H_
