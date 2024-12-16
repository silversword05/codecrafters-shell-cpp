#include "Util.h"

std::string trim(const std::string &s, std::string chars) {
    size_t start = s.find_first_not_of(chars);
    size_t end = s.find_last_not_of(chars);
    if (start == std::string::npos) {
        return "";
    }
    return s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string &s, char delim,
                               uint split_cnt) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            result.push_back(trim(item));
        }

        if (split_cnt && result.size() == split_cnt) {
            if (std::getline(ss, item) && !item.empty()) {
                result.push_back(trim(item));
            }
            break;
        }
    }
    return result;
}

std::vector<std::string> splitWithQuotes(const std::string &input) {
    std::vector<std::string> result;
    std::string current;
    char quote_type = '\0';
    bool backslash_within_double_quotes = false;

    for (char c : input) {
        if (backslash_within_double_quotes) {
            assert(quote_type == '"');
            backslash_within_double_quotes = false;

            if (c != '\\' && c != '"' && c != '$') {
                current += '\\';
            }
            current += c;
        } else if (c == ' ' || c == '\\' || c == '"' || c == '\'') {
            if (quote_type == '\0') {
                if (c == ' ') {
                    if (!current.empty()) {
                        result.push_back(current);
                        current.clear();
                    }
                } else {
                    quote_type = c;
                }
            } else {
                if (quote_type == '\\') {
                    quote_type = '\0';
                    current += c;
                } else if (c == quote_type) {
                    quote_type = '\0';
                } else {
                    if (quote_type == '"' && c == '\\') {
                        backslash_within_double_quotes = true;
                    } else {
                        current += c;
                    }
                }
            }
        } else {
            current += c;
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

std::string join(const std::vector<std::string> &words,
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

std::optional<std::string> cmdExistsInPath(const std::string &cmd) {
    std::string path_env = std::getenv("PATH");
    std::vector<std::string> paths = split(path_env, ':');

    for (const std::string &path : paths) {
        std::string full_path = path + "/" + cmd;
        if (std::filesystem::exists(full_path)) {
            return full_path;
        }
    }

    return std::nullopt;
}