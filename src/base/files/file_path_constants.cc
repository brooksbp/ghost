// base/files/file_path_constants.cc

#include "file_path.h"

namespace base {

#if defined(FILE_PATH_USES_WIN_SEPARATORS)
const FilePath::CharType FilePath::kSeparators[] = FILE_PATH_LITERAL("\\/");
#else
const FilePath::CharType FilePath::kSeparators[] = FILE_PATH_LITERAL("/");
#endif

const size_t FilePath::kSeparatorsLength = arraysize(kSeparators);

const FilePath::CharType FilePath::kCurrentDirectory[] = FILE_PATH_LITERAL(".");
const FilePath::CharType FilePath::kParentDirectory[] = FILE_PATH_LITERAL("..");

const FilePath::CharType FilePath::kExtensionSeparator = FILE_PATH_LITERAL('.');

}  // namespace base
