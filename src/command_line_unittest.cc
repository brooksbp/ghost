#include "command_line.h"
#include "macros.h"
#include <gtest/gtest.h>

TEST(CommandLineTest, CommandLineConstructor) {
  const CommandLine::CharType* argv[] = {
    FILE_PATH_LITERAL("program"),
    FILE_PATH_LITERAL("--foo="),
    FILE_PATH_LITERAL("-bAr"),
    FILE_PATH_LITERAL("-spaetzel=pierogi"),
    FILE_PATH_LITERAL("-baz"),
    FILE_PATH_LITERAL("flim"),
    FILE_PATH_LITERAL("--other-switches=--dog=canine --cat=feline"),
    FILE_PATH_LITERAL("-spaetzle=Crepe"),
    FILE_PATH_LITERAL("-=loosevalue"),
    FILE_PATH_LITERAL("-"),
    FILE_PATH_LITERAL("FLAN"),
    FILE_PATH_LITERAL("a"),
    FILE_PATH_LITERAL("--input-translation=45--output-rotation"),
    FILE_PATH_LITERAL("--"),
    FILE_PATH_LITERAL("--"),
    FILE_PATH_LITERAL("--not-a-switch"),
    FILE_PATH_LITERAL("\"in the time of submarines...\""),
    FILE_PATH_LITERAL("unquoted arg-with-space")};
  CommandLine cl(arraysize(argv), argv);

  EXPECT_FALSE(cl.GetCommandLineString().empty());
}

