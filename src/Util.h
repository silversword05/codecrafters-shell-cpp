#pragma once

#include <bits/stdc++.h>

using StringWithEndPos = std::pair<std::string, size_t>;

class Utils {
  public:
    static std::string trim(const std::string &s, std::string chars = " \t\n");
    static std::vector<std::string> split(const std::string &s, char delim,
                                          uint split_cnt = 0);
    static std::vector<StringWithEndPos>
    parseBashTokens(const std::string &command);

    static std::string join(const std::span<std::string> &v,
                            const std::string &delim);

    static std::optional<std::string> cmdExistsInPath(const std::string &cmd);
    static std::string refineCmd(const std::string &cmd);

    static std::pair<std::string, std::string>
    getArgListAndOutputFile(std::vector<StringWithEndPos> &parts,
                            std::string input);
};