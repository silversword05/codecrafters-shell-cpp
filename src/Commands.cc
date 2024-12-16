#include "Commands.h"

#include <sys/wait.h>
#include <unistd.h>

Command get_command(std::string &cmd_str) {
    if (cmd_str == "exit") {
        return Command::EXIT;
    } else if (cmd_str == "echo") {
        return Command::ECHO;
    } else if (cmd_str == "type") {
        return Command::TYPE;
    } else if (cmd_str == "pwd") {
        return Command::PWD;
    } else if (cmd_str == "cd") {
        return Command::CD;
    }
    std::optional<std::string> cmd_path = cmd_exists_in_path(cmd_str);
    if (cmd_path.has_value()) {
        cmd_str = cmd_path.value();
        return Command::EXECUTABLE;
    }
    return Command::INVALID;
}

template <>
void execute<Command::INVALID>(const std::string &cmd_str,
                               const std::string &arg_str) {
    std::cout << cmd_str << ": command not found" << std::endl;
}

template <>
void execute<Command::EXIT>(const std::string &, const std::string &arg_str) {
    if (arg_str.size() == 0) {
        std::exit(0);
    } else {
        std::exit(std::stoi(trim(arg_str)));
    }
}

template <>
void execute<Command::ECHO>(const std::string &, const std::string &arg_str) {
    std::cout << arg_str << std::endl;
}

template <>
void execute<Command::TYPE>(const std::string &, const std::string &arg_str) {
    std::string arg_str_cpy = arg_str;
    Command cmd = get_command(arg_str_cpy);
    if (cmd == Command::INVALID) {
        std::cout << arg_str << ": not found" << std::endl;
    } else if (cmd == Command::EXECUTABLE) {
        std::cout << arg_str << " is " << arg_str_cpy << std::endl;
    } else {
        std::cout << arg_str << " is a shell builtin" << std::endl;
    }
}

template <>
void execute<Command::EXECUTABLE>(const std::string &cmd_str,
                                  const std::string &arg_str) {
    std::string full_cmd = cmd_str + " " + arg_str;
    std::system(full_cmd.c_str());
}

template <>
void execute<Command::PWD>(const std::string &cmd_str,
                           const std::string &arg_str) {
    std::cout << std::filesystem::current_path().string() << std::endl;
}

template <>
void execute<Command::CD>(const std::string &cmd_str,
                          const std::string &arg_str) {
    std::error_code ec;
    std::filesystem::current_path(arg_str, ec);
    if (ec) {
        std::cout << cmd_str << ": " << arg_str << ": " << ec.message()
                  << std::endl;
    }
}

void dispatch(const std::string &input) {
    std::vector<std::string> parts = split(input, ' ', 1);
    Command cmd = get_command(parts[0]);

#define EXECUTE(CMD)                                                           \
    case Command::CMD:                                                         \
        execute<Command::CMD>(parts[0], parts[1]);                             \
        break;

    switch (cmd) {
        EXECUTE(EXIT)
        EXECUTE(ECHO)
        EXECUTE(TYPE)
        EXECUTE(EXECUTABLE)
        EXECUTE(PWD)
        EXECUTE(CD)
    default:
        execute<Command::INVALID>(parts[0], parts[1]);
    }

#undef EXECUTE
}