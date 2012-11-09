#include "configfile.hpp"

#include <boost/filesystem/fstream.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace fs = boost::filesystem;
namespace qi = boost::spirit::qi;

template <typename Iterator>
struct ConfigFileParser : qi::grammar<Iterator, std::map<std::string, ConfigItem>()>
{
    ConfigFileParser() : ConfigFileParser::base_type(configItems, "configItems")
    {
        using namespace qi::labels;
        using boost::phoenix::at_c;
        using boost::phoenix::stl::insert;
        using boost::phoenix::val;
        using boost::phoenix::construct;

        configItems = -(item % qi::eol);
        item = key                              [at_c<0>(_val) = _1]
            > qi::lit(':') | qi::lit('=')
            > value                             [at_c<1>(_val) = _1]
        ;
        value = *(qi::char_ - qi::eol)          [at_c<0>(_val) += _1];
        key = *(qi::char_("A-Za-z0-9"))         [_val += _1];

        configItems.name("configItems");
        item.name("item");
        value.name("value");
        key.name("key");

        qi::on_error<qi::fail>
        (
            configItems,
            std::cerr
                << val("Error while parsing Configuration! Expecting ")
                << _4   // what failed
                << val(" here: \"")
                << construct<std::string>(_3, _2) // iterators to error-pos, end
                << val("\"")
                << std::endl
        );
    }

    qi::rule<Iterator, std::map<std::string, ConfigItem>()> configItems;
    qi::rule<Iterator, std::pair<std::string, ConfigItem>()> item;
    qi::rule<Iterator, ConfigItem()> value;
    qi::rule<Iterator, std::string()> key;
};

ConfigItem::ConfigItem()
{
}

ConfigItem::ConfigItem(std::string defaultValue)
    : Default(defaultValue)
{
}

ConfigItem::ConfigItem(std::string defaultValue, std::string value)
    : Value(value), Default(defaultValue)
{
}

std::string& ConfigItem::operator()()
{
    if(this->Value == "")
        return Default;
    return Value;
}

ConfigFile::ConfigFile(fs::path filename)
    : filename_(filename)
{
    this->Load();
}

void ConfigFile::Load()
{
    using namespace boost::spirit::qi;

    fs::ifstream input(this->filename_.native());
    if(!input)
    {
        std::cerr << "Can't open file '" << this->filename_.native() << "'!";
    }

    std::string storage;
    input.unsetf(std::ios::skipws);
    std::copy(
            std::istream_iterator<char>(input),
            std::istream_iterator<char>(),
            std::back_inserter(storage));

    std::string::const_iterator begin = storage.begin();
    std::string::const_iterator end = storage.end();

    ConfigFileParser<std::string::const_iterator> grammar;

    const bool result = qi::phrase_parse(begin, end, grammar, ascii::space, this->config_);
    if(result)
    {
        return;
    }
    else
    {
        std::cerr << "Error!" << std::endl;
    }
}

void ConfigFile::Save()
{
    //TODO: Not implemented
}

ConfigItem& ConfigFile::Get(std::string key)
{
    return this->config_.at(key);
}


void ConfigFile::Set(std::string key, ConfigItem item)
{
    this->config_[key] = item;
}

ConfigItem& ConfigFile::operator[](std::string key)
{
    return this->config_[key];
}

