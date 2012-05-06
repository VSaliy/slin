#include "link.hpp"

slin::Link::Link()
{}

slin::Link::Link(std::string title, std::string link, std::string description)
    : Title{title}, Url{link}, Description{description}, m_id{-1}
{
}

int slin::Link::GetID() const
{
    return this->m_id;
}

void slin::Link::Tag(const std::string &tag)
{
    using namespace boost::xpressive;
    // Regex: #(+|-)?(.*)
    static sregex tagf = as_xpr("#") >> !(s1=(as_xpr("+") | "-")) >> (s2=+_);
    smatch what;
    bool remove = false;
    if(regex_match(tag, what, tagf))
    {
        if(what[1].matched)
            remove = what[1].str() == "-";

        if(remove)
        {
            this->Tags.erase(what[2].str());
        }
        else
        {
            this->Tags.insert(what[2].str());
        }
    }
    else
    {
        std::cout << "Wrong tag format" << std::endl;
    }
}

void slin::Link::AddNote(std::string text)
{
    this->Notes.insert(text);
}
