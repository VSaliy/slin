#pragma once

#include <string>
#include <unordered_set>
#include <boost/xpressive/xpressive.hpp>

namespace slin
{
  class Link
  {
  friend class Database;
  public:
    Link();
    Link(std::string title, std::string link, std::string description);
    ~Link() = default;

    int GetID() const;
    
    void Tag(const std::string &tag);
    void AddNote(std::string text);

    std::string Title;
    std::string Url;
    std::string Description;

    std::unordered_set<std::string> Tags;
    std::unordered_set<std::string> Notes;
  private:
    int m_id;
  };
}


namespace std
{
    // Needed for std::set
    template<>
    struct less<slin::Link>
    {
        bool operator()(const slin::Link &lhs, const slin::Link &rhs) const
        {
            return lhs.GetID() < rhs.GetID();
        }
    };
}
