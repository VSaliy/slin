#include "utils.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <curl/curl.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

string slin::concatStrings(const unordered_set<string> &set, string seperator)
{
    bool firstElem = true;
    string result = "";
    for(auto &elem : set)
    {
        if(!firstElem)
        {
            result += seperator;
        }
        firstElem = false;
        result += elem;
    }

    return result;
}

string slin::concatStrings(const vector<string> &vec, string seperator)
{
    bool firstElem = true;
    string result = "";
    for(auto &elem : vec)
    {
        if(!firstElem)
        {
            result += seperator;
        }
        firstElem = false;
        result += elem;
    }

    return result;
}

unordered_set<string> slin::splitStrings(string str, char seperator)
{
    string word;
    unordered_set<string> set;

    std::istringstream iss(str);
    while(std::getline(iss, word, seperator))
        set.insert(word);

    return set;
}

bool slin::isTag(string str)
{
    if(boost::algorithm::starts_with(str, "#"))
        return true;
    return false;
}

static size_t write_data_cb(char *buffer, size_t size, size_t nmemb, void *userp)
{
    string *tstr = static_cast<string*>(userp);
    size_t real_size = size * nmemb;
    tstr->append(buffer, real_size);

    return real_size;
}

string* slin::getWebsite(const string &url)
{
    string *page = new string;

    auto curl_h = curl_easy_init();
    if(!curl_h)
    {
        throw string("Could not initialize cURL");
    }
    // Receive the webpage
    curl_easy_setopt(curl_h, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_h, CURLOPT_FOLLOWLOCATION, 1); // We want to follow redirects
    curl_easy_setopt(curl_h, CURLOPT_WRITEFUNCTION, write_data_cb);
    curl_easy_setopt(curl_h, CURLOPT_WRITEDATA, static_cast<void*>(page));
    curl_easy_perform(curl_h);
    curl_easy_cleanup(curl_h);

    return page;
}

string slin::getWebsiteTitle(string *html)
{
    using namespace boost::xpressive;

    string title;

    // Extract the title from the webpage
    static sregex titlef = as_xpr("<title>") >> (s1=-+_) >> "</title>";
    smatch what;
    if(regex_search(*html, what, titlef))
    {
        title = what[1].str();

        // Remove all uneeded spaces
        title.erase(remove(title.begin(), title.end(), '\n'), title.end());
        boost::algorithm::trim_all(title);
    }
    else
    {
        title = "";
    }
    return title;
}

string slin::getWebsiteTitle(const string &url)
{
    auto page = getWebsite(url);
    string title = slin::getWebsiteTitle(page);
    delete page;
    return title;
}

string slin::getWebsiteDescription(string *html)
{
    using namespace boost::xpressive;

    string description;

    static sregex descriptionf = icase("<meta name=\"description\" content=\"") >> (s1=-+_) >> "\"" >> -*(~_w) >> -!as_xpr("/") >> ">";
    smatch what;
    if(regex_search(*html, what, descriptionf))
    {
        description = what[1].str();
    }
    else
    {
        description = "";
    }
    return description;
}

string slin::getWebsiteDescription(const string &url)
{
    auto page = slin::getWebsite(url);
    string description = slin::getWebsiteDescription(page);
    delete page;
    return description;
}


string slin::findConfig(string filename)
{
    fs::path path;
#ifdef _WIN32
    //TODO: Implement findConfig for Windows
    path = fs::current_path() / filename;

#elif __linux__
    auto xdg_dir = getenv("XDG_CONFIG_HOME");
    auto home_dir = getenv("HOME");
    if(xdg_dir != nullptr)
    {
        path = fs::path(xdg_dir) / filename;
    }
    else if(home_dir != nullptr)
    {
        path = fs::path(home_dir) / ("." + filename);
    }
    else
    {
        path = fs::current_path() / filename;
    }

#elif __APPLE__
    //TODO: Implement findConfig for Mac
    path = fs::current_path() / filename;

#else
    #warning "Unsupported Operating System!"
    path = fs::current_path() / filename;
#endif
    
    return path.string();
}

pair<string, string> slin::splitConfigEntry(string entry)
{
    vector<std::string> splitted;
    boost::algorithm::split(splitted, entry, boost::algorithm::is_any_of("="));
    if(splitted.size() < 2)
    {
        cerr << "Can't split string '" << entry << "'" << endl;
        throw new string("slin::splitConfigEntry: Error");
    }
    return make_pair(splitted[0], splitted[1]);
}
