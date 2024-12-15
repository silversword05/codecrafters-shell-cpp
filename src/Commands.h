#pragma once

#include "Util.h"

enum class Command : char {
    EXIT,       // exit
    ECHO,       // echo
    TYPE,       // type
    EXECUTABLE, // custom command
    INVALID = std::numeric_limits<char>::max(),
};

Command get_command(std::string &cmd_str);

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

void dispatch(const std::string &input);
