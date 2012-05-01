#include "utils.hpp"

#include <sstream>

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
