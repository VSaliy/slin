#pragma once

#include <unordered_set>
#include <vector>
#include <string>

namespace slin
{
    std::string concatStrings(const std::unordered_set<std::string> &set, std::string seperator);
    std::string concatStrings(const std::vector<std::string> &vec, std::string seperator);
}
