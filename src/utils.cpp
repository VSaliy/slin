#include "utils.hpp"

#include <sstream>
#include <curl/curl.h>
#include <boost/xpressive/xpressive.hpp>

using namespace std;

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

string slin::getWebsiteTitle(const string &url)
{
    using namespace boost::xpressive;

    string title;
    auto page = getWebsite(url);

    // Extract the title from the webpage
    static sregex titlef = as_xpr("<title>") >> (s1=-+_) >> "</title>";
    smatch what;
    if(regex_search(*page, what, titlef))
    {
        title = what[1].str();
    }
    else
    {
        title = "";
    }
    delete page;
    return title;
}
