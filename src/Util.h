#pragma once

#include <bits/stdc++.h>

std::string trim(const std::string &s, std::string chars = " \t\n");
std::vector<std::string> split(const std::string &s, char delim,
                               uint split_cnt = 0);
std::vector<std::string> splitWithQuotes(const std::string &input,
                                         uint split_cnt = 0);

std::string join(const std::vector<std::string> &v, const std::string &delim);

std::optional<std::string> cmdExistsInPath(const std::string &cmd);
std::string refineCmd(const std::string &cmd);