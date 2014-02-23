// base/strings/utf_string_conversions.h

#ifndef BASE_STRINGS_UTF_STRING_CONVERSIONS_H_
#define BASE_STRINGS_UTF_STRING_CONVERSIONS_H_

#include <string>

#include "base/strings/string16.h"
#include "base/strings/string_piece.h"

namespace base {

// These convert between UTF-8, -16, and -32 strings.  They are potentially slow,
// so avoid unnecessary conversions. The low-level versions return a boolean
// indicating whether the conversion was 100% valid. In this case, it will still
// do the best it can and put the result in the output buffer. The versions that
// return strings ignore this error and just return the best conversion
// possible.
bool WideToUTF8(const wchar_t* src, size_t src_len,
                std::string* output);
std::string WideToUTF8(const std::wstring& wide);
bool UTF8ToWide(const char* src, size_t src_len,
                std::wstring* output);


  bool UTF8ToUTF16(const char* src, size_t src_len, string16* output)

}  // namespace base

#endif  // BASE_STRINGS_UTF_STRING_CONVERSIONS_H_
