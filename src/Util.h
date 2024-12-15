#pragma once

#include <bits/stdc++.h>

std::string trim(const std::string &s);
std::vector<std::string> split(const std::string &s, char delim,
                               uint split_cnt = 0);

std::optional<std::string> cmd_exists_in_path(const std::string &cmd);