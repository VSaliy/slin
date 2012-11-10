// Copyright (c) 2012, Marco Schlumpp <marco.schlumpp@gmail.com>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
// SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
// OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
// CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#pragma once

#include <unordered_set>
#include <vector>
#include <string>
#include <utility>

namespace slin
{
    std::string concatStrings(const std::unordered_set<std::string> &set, std::string seperator);
    std::string concatStrings(const std::vector<std::string> &vec, std::string seperator);
    std::unordered_set<std::string> splitStrings(std::string str, char seperator);
    bool isTag(std::string str);

    std::string* getWebsite(const std::string &url);
    std::string getWebsiteTitle(std::string *html);
    std::string getWebsiteTitle(const std::string &url);
    std::string getWebsiteDescription(std::string *html);
    std::string getWebsiteDescription(const std::string &url);

    std::string findConfig(std::string filename);
    std::pair<std::string, std::string> splitConfigEntry(std::string entry);
}
