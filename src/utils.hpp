#pragma once

#include <unordered_set>
#include <vector>
#include <string>

namespace slin
{
    std::string concatStrings(const std::unordered_set<std::string> &set, std::string seperator);
    std::string concatStrings(const std::vector<std::string> &vec, std::string seperator);
    std::unordered_set<std::string> splitStrings(std::string str, char seperator);

    std::string* getWebsite(const std::string &url);
    std::string getWebsiteTitle(std::string *html);
    std::string getWebsiteTitle(const std::string &url);
    std::string getWebsiteDescription(std::string *html);
    std::string getWebsiteDescription(const std::string &url);

    std::string findConfig(std::string filename);
}
