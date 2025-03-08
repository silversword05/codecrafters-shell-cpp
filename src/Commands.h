#pragma once

#include "Util.h"

enum class Command : char {
    EXIT,       // exit
    ECHO,       // echo
    TYPE,       // type
    EXECUTABLE, // custom command
    PWD,        // pwd
    CD,         // cd
    INVALID = std::numeric_limits<char>::max(),
};

namespace detail {
template <Command cmd>
void execute(const std::string &cmd_str, const std::string &arg_str);

template <>
void execute<Command::INVALID>(const std::string &cmd_str,
                               const std::string &arg_str);
template <>
void execute<Command::EXIT>(const std::string &cmd_str,
                            const std::string &arg_str);
template <>
void execute<Command::ECHO>(const std::string &cmd_str,
                            const std::string &arg_str);
template <>
void execute<Command::TYPE>(const std::string &cmd_str,
                            const std::string &arg_str);
template <>
void execute<Command::EXECUTABLE>(const std::string &cmd_str,
                                  const std::string &arg_str);
template <>
void execute<Command::PWD>(const std::string &cmd_str,
                           const std::string &arg_str);
template <>
void execute<Command::CD>(const std::string &cmd_str,
                          const std::string &arg_str);

Command getCommand(std::string &cmd_str, bool replace_path);
std::string exec(std::string_view cmd);
} // namespace detail

class CommandProcessor {
  public:
    void dispatch(const std::string &input);

  private:
    template <Command cmd>
    void execute(const std::string &cmd_str, const std::string &arg_str);

    void redirectCout(const std::string &output_file);
    void restoreCout();

    std::streambuf *cout_buf;
    std::ofstream out;
};
