// base/strings/string_util.h

#ifndef BASE_STRINGS_STRING_UTIL_H_
#define BASE_STRINGS_STRING_UTIL_H_

#include <ctype.h>
#include <stdarg.h>

#include <string>
#include <vector>

#include "base/base_export.h"
#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/strings/string16.h"
// #include "base/strings/string_piece.h"

// HACK
typedef wchar_t char16;


namespace base {


// ASCII-specific tolower.  The standard library's tolower is locale sensitive,
// so we don't want to use it here.
template <class Char> inline Char ToLowerASCII(Char c) {
  return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

// ASCII-specific toupper.  The standard library's toupper is locale sensitive,
// so we don't want to use it here.
template <class Char> inline Char ToUpperASCII(Char c) {
  return (c >= 'a' && c <= 'z') ? (c + ('A' - 'a')) : c;
}

// Function objects to aid in comparing/searching strings.

template<typename Char> struct CaseInsensitiveCompare {
 public:
  bool operator()(Char x, Char y) const {
    // TODO(darin): Do we really want to do locale sensitive comparisons here?
    // See http://crbug.com/24917
    return tolower(x) == tolower(y);
  }
};

template<typename Char> struct CaseInsensitiveCompareASCII {
 public:
  bool operator()(Char x, Char y) const {
    return ToLowerASCII(x) == ToLowerASCII(y);
  }
};

}  // namespace base


// Contains the set of characters representing whitespace in the corresponding
// encoding. Null-terminated.
extern const wchar_t kWhitespaceWide[];
extern const char16 kWhitespaceUTF16[];
extern const char kWhitespaceASCII[];

// Null-terminated string representing the UTF-8 byte order mark.
extern const char kUtf8ByteOrderMark[];


#if defined(OS_WIN)
#include "base/strings/string_util_win.h"
#elif defined(OS_POSIX)
#include "base/strings/string_util_posix.h"
#else
#error Define string operations appropriately for your platform
#endif

// Trims any whitespace from either end of the input string.  Returns where
// whitespace was found.
// The non-wide version has two functions:
// * TrimWhitespaceASCII()
//   This function is for ASCII strings and only looks for ASCII whitespace;
// Please choose the best one according to your usage.
// NOTE: Safe to use the same variable for both input and output.
enum TrimPositions {
  TRIM_NONE     = 0,
  TRIM_LEADING  = 1 << 0,
  TRIM_TRAILING = 1 << 1,
  TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
};
BASE_EXPORT TrimPositions TrimWhitespace(const base::string16& input,
                                         TrimPositions positions,
                                         base::string16* output);
BASE_EXPORT TrimPositions TrimWhitespaceASCII(const std::string& input,
                                              TrimPositions postiions,
                                              std::string* output);

// Deprecated.  This function is only for backward compatibility and calls
// TrimWhitespaceASCII().
BASE_EXPORT TrimPositions TrimWhitespace(const std::string& input,
                                         TrimPositions postiions,
                                         std::string* output);

// Converts to 7-bit ASCII by truncating. The result must be known to be ASCII
// beforehand.
BASE_EXPORT std::string WideToASCII(const std::wstring& wide);
BASE_EXPORT std::string UTF16ToASCII(const base::string16& utf16);

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
BASE_EXPORT bool IsStringUTF8(const std::string& str);
BASE_EXPORT bool IsStringUTF8(const base::StringPiece& str);
BASE_EXPORT bool IsStringUTF8(const std::string& str);
#endif

// Converts the elements of the given string.  This version uses a pointer to
// clearly differentiate it from the non-pointer variant.
template <class str> inline void StringToLowerASCII(str* s) {
  for (typename str::iterator i = s->begin(); i != s->end(); ++i)
    *i = base::ToLowerASCII(*i);
}

template <class str> inline str StringToLowerASCII(const str& s) {
  // for std::string and std::wstring
  str output(s);
  StringToLowerASCII(&output);
  return output;
}

// Converts the elements of the given string.  This version uses a pointer to
// clearly differentiate it form the non-pointer variant.
template <class str> inline void StringToUpperASCII(str* s) {
  for (typename str::iterator i = s->begin(); i != s->end(); ++i)
    *i = base::ToUpperASCII(*i);
}

template <class str> inline str StringToUpperASCII(const str& s) {
  // for std::string and std::wstring
  str output(s);
  StringToUpperASCII(&output);
  return output;
}

// Compare the lower-case form of the given string against the given ASCII
// string.  This is useful for doing checking if an input string matches some
// token, and it is optimized to avoid intermediate string copies.  This API is
// borrowed from the equivalent APIs in Mozilla.
BASE_EXPORT bool LowerCaseEqualsASCII(const std::string& a, const char* b);
BASE_EXPORT bool LowerCaseEqualsASCII(const base::string16& a, const char* b);

// Same thing, but with string iterators instead.
BASE_EXPORT bool LowerCaseEqualsASCII(std::string::const_iterator a_begin,
                                      std::string::const_iterator a_end,
                                      const char* b);
BASE_EXPORT bool LowerCaseEqualsASCII(base::string16::const_iterator a_begin,
                                      base::string16::const_iterator a_end,
                                      const char* b);
BASE_EXPORT bool LowerCaseEqualsASCII(const char* a_begin,
                                      const char* a_end,
                                      const char* b);
BASE_EXPORT bool LowerCaseEqualsASCII(const base::char16* a_begin,
                                      const base::char16* a_end,
                                      const char* b);


#endif  // BASE_STRINGS_STRING_UTIL_H_
