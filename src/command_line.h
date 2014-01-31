// base/command_line.h

#ifndef BASE_COMMAND_LINE_H_
#define BASE_COMMAND_LINE_H_

#include <map>
#include <string>
#include <vector>

#include "build_config.h"
#include "file_path.h"

class CommandLine {
 public:
#if defined(OS_WIN)
  // The native command line string type.
  typedef std::wstring StringType;
#elif defined(OS_POSIX)
  typedef std::string StringType;
#endif

  typedef StringType::value_type CharType;
  typedef std::vector<StringType> StringVector;
  typedef std::map<std::string, StringType> SwitchMap;

  // A constructor for CommandLines that only carry switches and arguments.
  enum NoProgram { NO_PROGRAM };

  CommandLine(int argc, const CharType* const* argv);

  ~CommandLine();

  // Initialize from an argv vector.
  void InitFromArgv(int argc, const CharType* const* argv);
  void InitFromArgv(const StringVector& argv);

  // Constructs and returns the represented command line string.
  // CAUTION! This should be avoided on POSIX because quoting behavior is
  // unclear.
  StringType GetCommandLineString() const;

  // Constructs and returns the represented arguments string.
  // CAUTION! This should be avoided on POSIX because quoting behavior is
  // unclear.
  StringType GetArgumentsString() const;
  
  // Get and Set the program part of the command line string (the first item).
  FilePath GetProgram() const;
  void SetProgram(const FilePath& program);

  // Append a switch [with optional value] to the command line.
  // Note: Switches will precede arguments regardless of appending order.
#if 0
  void AppendSwitch(const std::string& switch_string);
  void AppendSwitchPath(const std::string& switch_string,
                        const FilePath& path);
#endif
  void AppendSwitchNative(const std::string& switch_string,
                          const StringType& value);
#if 0
  void AppendSwitchASCII(const std::string& switch_string,
                         const std::string& value);
#endif


  // Append an argument to the command line. Note that the argument is quoted
  // properly such that it is interpreted as one argument to the target command.
  // AppendArg is primarily for ASCII; non-ASCII input is interpreted as UTF-8.
  // Note: Switches will precede arguments regardless of appending order.
  void AppendArg(const std::string& value);
  void AppendArgPath(const FilePath& value);
  void AppendArgNative(const StringType& value);
  
 private:
  // Disallow default constructor; a program name must be explicitly specified.
  CommandLine();
  // Allow the copy constructor. A common pattern is to copy of the current
  // process's command line and then add some flags to it. For example:
  //   CommandLine cl(*CommandLine::ForCurrentProcess());
  //   cl.AppendSwitch(...);

  // The singleton CommandLine representing the current process's command line.
  static CommandLine * current_process_commandline_;

  // The argv array: { program, [(--|-|/)switch[=value]]*, [--], [argument]* }
  StringVector argv_;

  // Parsed-out switch keys and values.
  SwitchMap switches_;

  // The index after the program and switches, and arguments start here.
  size_t begin_args_;
};

#endif  // BASE_COMMAND_LINE_H_
