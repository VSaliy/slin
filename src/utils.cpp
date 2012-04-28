#include "utils.hpp"

using namespace std;

string slin::concatStrings(const unordered_set<string> &set, string seperator)
{
    bool firstElem = true;
    string result = {""};
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
    string result = {""};
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

