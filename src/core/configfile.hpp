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

#include <string>
#include <map>
#include <initializer_list>

#include <boost/filesystem.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

class ConfigItem
{
public:
    ConfigItem();
    ConfigItem(std::string defaultValue);
    ConfigItem(std::string defaultValue, std::string value);

    std::string& operator()();

    std::string Value;
    std::string Default;
};

BOOST_FUSION_ADAPT_STRUCT(
    ConfigItem,
    (std::string, Value)
)

class ConfigFile
{
public:
    ConfigFile(boost::filesystem::path filename);
    void Load();
    void Save();

    ConfigItem& Get(std::string);
    void Set(std::string key, ConfigItem item);
    ConfigItem& operator[](std::string key);

private:
    boost::filesystem::path filename_;
    std::map<std::string, ConfigItem> config_;
};
