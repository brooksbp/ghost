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

#if 0
// Returns true if the specified string matches the criteria. How can a wide
// string be 8-bit or UTF8? It contains only characters that are < 256 (in the
// first case) or characters that use only 8-bits and whos 8-bit
// representation looks like a UTF-8 string (the second case).
//
// Note that IsStringUTF8 checks not only if the input is structurally
// valid but also if it doesn't contain any non-character codepoint
// (e.g. U+FFFE). It's done on purpose because all the existing callers want
// to have the maximum 'discriminating' power from other encodings. If
// there's a use case for just checking the structural validity, we have to
// add a new function for that.
bool IsStringUTF8(const std::string& str);
bool IsStringUTF8(const base::StringPiece& str);
bool IsStringUTF8(const std::string& str);
#endif

#endif  // BASE_STRINGS_STRING_UTIL_H_
