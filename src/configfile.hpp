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
