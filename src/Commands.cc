#include "Commands.h"

Command get_command(const std::string &cmd_str) {
    if (cmd_str == "exit") {
        return Command::EXIT;
    } else if (cmd_str == "echo") {
        return Command::ECHO;
    } else if (cmd_str == "type") {
        return Command::TYPE;
    } else {
        return Command::INVALID;
    }
}

template <>
void execute<Command::INVALID>(const std::string &cmd_str,
                               const std::string &arg_str) {
    assert(get_command(cmd_str) == Command::INVALID);
    std::cout << cmd_str << ": command not found" << std::endl;
}

template <>
void execute<Command::EXIT>(const std::string &cmd_str,
                            const std::string &arg_str) {
    if (arg_str.size() == 0) {
        std::exit(0);
    } else {
        std::exit(std::stoi(trim(arg_str)));
    }
}

template <>
void execute<Command::ECHO>(const std::string &cmd_str,
                            const std::string &arg_str) {
    std::cout << arg_str << std::endl;
}

template <>
void execute<Command::TYPE>(const std::string &cmd_str,
                            const std::string &arg_str) {
    Command cmd = get_command(arg_str);
    if (cmd == Command::INVALID) {
        std::optional<std::string> cmd_path = cmd_exists_in_path(arg_str);
        if (cmd_path) {
            std::cout << arg_str << " is " << *cmd_path << std::endl;
        } else {
            std::cout << arg_str << ": not found" << std::endl;
        }
    } else {
        std::cout << arg_str << " is a shell builtin" << std::endl;
    }
}

void dispatch(const std::string &input) {
    std::vector<std::string> parts = split(input, ' ', 1);
    Command cmd = get_command(parts[0]);
    switch (cmd) {
    case Command::EXIT:
        execute<Command::EXIT>(parts[0], parts[1]);
        break;
    case Command::ECHO:
        execute<Command::ECHO>(parts[0], parts[1]);
        break;
    case Command::TYPE:
        execute<Command::TYPE>(parts[0], parts[1]);
        break;
    default:
        execute<Command::INVALID>(parts[0], parts[1]);
    }
}