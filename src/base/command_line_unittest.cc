// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include <gtest/gtest.h>

using base::FilePath;

TEST(CommandLineTest, CommandLineConstructor) {
  const CommandLine::CharType* argv[] = {
    FILE_PATH_LITERAL("program"),
    FILE_PATH_LITERAL("--foo="),
    FILE_PATH_LITERAL("-bAr"),
    FILE_PATH_LITERAL("-spaetzel=pierogi"),
    FILE_PATH_LITERAL("-baz"),
    FILE_PATH_LITERAL("flim"),
    FILE_PATH_LITERAL("--other-switches=--dog=canine --cat=feline"),
    FILE_PATH_LITERAL("-spaetzel=Crepe"),
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
  EXPECT_FALSE(cl.HasSwitch("cruller"));
  EXPECT_FALSE(cl.HasSwitch("flim"));
  EXPECT_FALSE(cl.HasSwitch("program"));
  EXPECT_FALSE(cl.HasSwitch("dog"));
  EXPECT_FALSE(cl.HasSwitch("cat"));
  EXPECT_FALSE(cl.HasSwitch("output-rotation"));
  EXPECT_FALSE(cl.HasSwitch("not-a-switch"));
  EXPECT_FALSE(cl.HasSwitch("--"));

  EXPECT_EQ(FilePath(FILE_PATH_LITERAL("program")).value(),
            cl.GetProgram().value());

  EXPECT_TRUE(cl.HasSwitch("foo"));
  EXPECT_TRUE(cl.HasSwitch("bAr"));
  EXPECT_TRUE(cl.HasSwitch("spaetzel"));
  EXPECT_TRUE(cl.HasSwitch("baz"));
  EXPECT_TRUE(cl.HasSwitch("other-switches"));
  EXPECT_TRUE(cl.HasSwitch("input-translation"));

  EXPECT_EQ("Crepe", cl.GetSwitchValueASCII("spaetzel"));
  EXPECT_EQ("", cl.GetSwitchValueASCII("Foo"));
  EXPECT_EQ("", cl.GetSwitchValueASCII("bar"));
  EXPECT_EQ("", cl.GetSwitchValueASCII("cruller"));
  EXPECT_EQ("--dog=canine --cat=feline", cl.GetSwitchValueASCII(
      "other-switches"));
  EXPECT_EQ("45--output-rotation", cl.GetSwitchValueASCII("input-translation"));

  const CommandLine::StringVector& args = cl.GetArgs();
  ASSERT_EQ(8U, args.size());

  std::vector<CommandLine::StringType>::const_iterator iter = args.begin();
  EXPECT_EQ(FILE_PATH_LITERAL("flim"), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("-"), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("FLAN"), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("a"), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("--"), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("--not-a-switch"), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("\"in the time of submarines...\""), *iter);
  ++iter;
  EXPECT_EQ(FILE_PATH_LITERAL("unquoted arg-with-space"), *iter);
  ++iter;
  EXPECT_TRUE(iter == args.end());
}

TEST(CommandLineTest, EmptyString) {
  CommandLine cl_from_argv(0, NULL);
  EXPECT_TRUE(cl_from_argv.GetCommandLineString().empty());
  EXPECT_TRUE(cl_from_argv.GetProgram().empty());
  EXPECT_EQ(1U, cl_from_argv.argv().size());
  EXPECT_TRUE(cl_from_argv.GetArgs().empty());
}

TEST(CommandLineTest, GetArgumentsString) {
  static const FilePath::CharType kPath1[] =
      FILE_PATH_LITERAL("C:\\Some File\\With Spaces.ggg");
  static const FilePath::CharType kPath2[] =
      FILE_PATH_LITERAL("C:\\no\\spaces.ggg");

  static const char kFirstArgName[] = "first-arg";
  static const char kSecondArgName[] = "arg2";
  static const char kThirdArgName[] = "arg with space";
  static const char kFourthArgName[] = "nospace";

  CommandLine cl(CommandLine::NO_PROGRAM);
  cl.AppendSwitchPath(kFirstArgName, FilePath(kPath1));
  cl.AppendSwitchPath(kSecondArgName, FilePath(kPath2));
  cl.AppendArg(kThirdArgName);
  cl.AppendArg(kFourthArgName);

#if defined(OS_WIN)
  CommandLine::StringType expected_first_arg(
      base::UTF8ToUTF16(kFirstArgName));
  CommandLine::StringType expected_second_arg(
      base::UTF8ToUTF16(kSecondArgName));
  CommandLine::StringType expected_third_arg(
      base::UTF8ToUTF16(kThirdArgName));
  CommandLine::StringType expected_fourth_arg(
      base::UTF8ToUTF16(kFourthArgName));
#elif defined(OS_POSIX)
  CommandLine::StringType expected_first_arg(kFirstArgName);
  CommandLine::StringType expected_second_arg(kSecondArgName);
  CommandLine::StringType expected_third_arg(kThirdArgName);
  CommandLine::StringType expected_fourth_arg(kFourthArgName);
#endif

#if defined(OS_WIN)
#define QUOTE_ON_WIN FILE_PATH_LITERAL("\"")
#else
#define QUOTE_ON_WIN FILE_PATH_LITERAL("")
#endif  // OS_WIN

  CommandLine::StringType expected_str;
  expected_str.append(FILE_PATH_LITERAL("--"))
      .append(expected_first_arg)
      .append(FILE_PATH_LITERAL("="))
      .append(QUOTE_ON_WIN)
      .append(kPath1)
      .append(QUOTE_ON_WIN)
      .append(FILE_PATH_LITERAL(" "))
      .append(FILE_PATH_LITERAL("--"))
      .append(expected_second_arg)
      .append(FILE_PATH_LITERAL("="))
      .append(QUOTE_ON_WIN)
      .append(kPath2)
      .append(QUOTE_ON_WIN)
      .append(FILE_PATH_LITERAL(" "))
      .append(QUOTE_ON_WIN)
      .append(expected_third_arg)
      .append(QUOTE_ON_WIN)
      .append(FILE_PATH_LITERAL(" "))
      .append(expected_fourth_arg);
  EXPECT_EQ(expected_str, cl.GetArgumentsString());
}
