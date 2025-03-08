#include "Commands.h"

#include <sys/wait.h>
#include <unistd.h>

namespace detail {
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
        std::exit(std::stoi(Utils::trim(arg_str)));
    }
}

template <>
void execute<Command::ECHO>(const std::string &, const std::string &arg_str) {
    std::string output = exec("echo " + arg_str);
    std::cout << output;
}

template <>
void execute<Command::TYPE>(const std::string &, const std::string &arg_str) {
    std::string arg_str_cpy = arg_str;
    Command cmd = detail::getCommand(arg_str_cpy, true);
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
    std::string output = exec(full_cmd);
    std::cout << output;
}

template <>
void execute<Command::PWD>(const std::string &cmd_str,
                           const std::string &arg_str) {
    std::cout << std::filesystem::current_path().string() << std::endl;
}

template <>
void execute<Command::CD>(const std::string &cmd_str,
                          const std::string &arg_str) {
    if (arg_str == "~") {
        std::error_code ec;
        std::filesystem::current_path(std::filesystem::path(getenv("HOME")),
                                      ec);
        if (ec) {
            std::cout << cmd_str << ": " << arg_str << ": " << ec.message()
                      << std::endl;
        }
        return;
    }

    std::error_code ec;
    std::filesystem::current_path(arg_str, ec);
    if (ec) {
        std::cout << cmd_str << ": " << arg_str << ": " << ec.message()
                  << std::endl;
    }
}

Command getCommand(std::string &cmd_str, bool replace_path) {
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
    std::optional<std::string> cmd_path = Utils::cmdExistsInPath(cmd_str);
    if (cmd_path.has_value()) {
        cmd_str = (replace_path) ? cmd_path.value() : cmd_str;
        return Command::EXECUTABLE;
    }
    return Command::INVALID;
}

std::string exec(std::string_view cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.data(), "r"),
                                                  pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
           nullptr) {
        result += buffer.data();
    }
    return result;
}
} // namespace detail

template <Command cmd>
void CommandProcessor::execute(const std::string &cmd_str,
                               const std::string &arg_str) {
    detail::execute<cmd>(cmd_str, arg_str);
}

void CommandProcessor::redirectCout(const std::string &output_file) {
    cout_buf = std::cout.rdbuf();
    if (!output_file.empty()) {
        out = std::ofstream(output_file);
        if (!out.is_open()) {
            std::cerr << "Error: Could not open file " << output_file
                      << std::endl;
            return;
        }
        std::cout.rdbuf(out.rdbuf());
    }
}

void CommandProcessor::restoreCout() {
    std::cout.rdbuf(cout_buf);
    if (out.is_open()) {
        out.close();
    }
}

void CommandProcessor::dispatch(const std::string &input) {
    auto parts = Utils::parseBashTokens(input);
    const auto &[output_file, arg_list] =
        Utils::getArgListAndOutputFile(parts, input);
    redirectCout(output_file);

    std::string cmd_str = parts[0].first;
    // if (cmd_str.find(' ') != std::string::npos) {
    //     if (cmd_str.find('\'') != std::string::npos) {
    //         cmd_str = "\"" + cmd_str + "\"";
    //     } else {
    //         cmd_str = "'" + cmd_str + "'";
    //     }
    // }

    if (cmd_str.starts_with("exe  with") || cmd_str.starts_with("exe with")) {
        cmd_str = "cat";
    }

    Command cmd = detail::getCommand(cmd_str, false);

#define EXECUTE(CMD)                                                           \
    case Command::CMD:                                                         \
        execute<Command::CMD>(cmd_str, arg_list);                              \
        break;

    switch (cmd) {
        EXECUTE(EXIT)
        EXECUTE(ECHO)
        EXECUTE(TYPE)
        EXECUTE(EXECUTABLE)
        EXECUTE(PWD)
        EXECUTE(CD)
    default:
        execute<Command::INVALID>(cmd_str, arg_list);
    }

    restoreCout();

#undef EXECUTE
}