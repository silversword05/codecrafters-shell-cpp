#include "Util.h"
#include <unistd.h>

#include "lib/dry-comparisons.hpp"

std::string Utils::trim(const std::string &s, std::string chars) {
    size_t start = s.find_first_not_of(chars);
    size_t end = s.find_last_not_of(chars);
    if (start == std::string::npos) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

std::vector<std::string> Utils::split(const std::string &s, char delim,
                                      uint split_cnt) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            result.push_back(Utils::trim(item));
        }

        if (split_cnt && result.size() == split_cnt) {
            if (std::getline(ss, item) && !item.empty()) {
                result.push_back(Utils::trim(item));
            }
            break;
        }
    }
    return result;
}

std::vector<StringWithEndPos> Utils::parseBashTokens(const std::string &input) {
    std::vector<StringWithEndPos> commandParameters;

    bool inSingleQuotes = false;
    bool inDoubleQuotes = false;
    std::ostringstream currentParam;

    auto flushCurrentParam = [&](size_t i) {
        if (!currentParam.str().empty()) {
            commandParameters.push_back({currentParam.str(), i});
            currentParam.str("");
            currentParam.clear();
        }
    };
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '\'' && !inDoubleQuotes) {
            // Toggle single-quote state and skip the quote itself
            inSingleQuotes = !inSingleQuotes;
        } else if (c == '"' && !inSingleQuotes) {
            // Toggle double-quote state and skip the quote itself
            inDoubleQuotes = !inDoubleQuotes;
        } else if (c == '\\' && (inDoubleQuotes || !inSingleQuotes) &&
                   i + 1 < input.size()) {
            // Handle escaped characters (e.g., \" or \\)
            char next = input[++i];
            currentParam << next;
        } else if (c == ' ' && !inSingleQuotes && !inDoubleQuotes) {
            // Split parameters on spaces when outside quotes
            flushCurrentParam(i);
        } else {
            // Append regular characters
            currentParam << c;
        }
    }
    flushCurrentParam(input.size()); // Add the final parameter if any

    return commandParameters;
}

std::string Utils::join(const std::span<std::string> &words,
                        const std::string &delim) {
    std::string result = std::accumulate(
        words.begin(), words.end(), std::string(""),
        [](const std::string &a, const std::string &b) { return a + " " + b; });

    // Remove the leading delim
    if (!result.empty()) {
        result.erase(0, 1);
    }

    return result;
}

std::string Utils::refineCmd(const std::string &cmd) {
    std::string res = "";
    bool add_quotes = false;
    for (char c : cmd) {
        if (c == '"') {
            res.push_back('\\');
        }
        if (c == ' ' || c == '"' || c == '\'') {
            add_quotes = true;
        }
        res.push_back(c);
    }
    if (add_quotes) {
        res = "\"" + res + "\"";
    }
    return res;
}

std::optional<std::string> Utils::cmdExistsInPath(const std::string &cmd) {
    std::string path_env = std::getenv("PATH");
    std::vector<std::string> paths = Utils::split(path_env, ':');

    for (const std::string &path : paths) {
        std::string full_path = path + "/" + cmd;
        if (std::filesystem::exists(full_path)) {
            return (path + "/" + cmd);
        }
    }

    return std::nullopt;
}

std::pair<std::string, std::string>
Utils::getArgListAndOutputFile(std::vector<StringWithEndPos> &parts,
                               std::string input) {
    std::string output_file;
    if (parts.size() > 2 &&
        parts[parts.size() - 2].first == rollbear::any_of(">", "1>")) {
        output_file = parts[parts.size() - 1].first;

        size_t last_index = parts[parts.size() - 3].second;
        input = input.substr(0, last_index);
    }

    if (input.size() == parts[0].second) {
        return {output_file, ""};
    }
    input = input.substr(parts[0].second + 1);

    return {output_file, input};
}