// base/files/file_path.cc

#include "file_path.h"

typedef FilePath::StringType StringType;

const char* kCommonDoubleExtensionSuffixes[] = { "gz", "z", "bz2" };

const FilePath::CharType kStringTerminator = FILE_PATH_LITERAL('\0');

FilePath::FilePath() {
}

FilePath::FilePath(const FilePath& that) : path_(that.path_) {
}

FilePath::FilePath(const StringType& path) : path_(path) {
  StringType::size_type nul_pos = path_.find(kStringTerminator);
  if (nul_pos != StringType::npos)
    path_.erase(nul_pos, StringType::npos);
}

FilePath::~FilePath() {
}
