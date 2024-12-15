#include "Util.h"

std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\n");
    size_t end = s.find_last_not_of(" \t\n");
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
            std::getline(ss, item);
            if (!item.empty()) {
                result.push_back(trim(item));
            }
            break;
        }
    }
    return result;
}