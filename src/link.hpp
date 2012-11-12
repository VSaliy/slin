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
#include <unordered_set>
#include <boost/xpressive/xpressive.hpp>
#include <soci.h>

#include "utils.hpp"

namespace slin
{
  class Link
  {
  friend class Database;
  friend struct soci::type_conversion<slin::Link>;
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

namespace soci
{
    template<>
    struct type_conversion<slin::Link>
    {
        typedef values base_type;
        static void from_base(values const &v, indicator ind, slin::Link &p)
        {
            char delim = (char)*("\x91");

            p.m_id = v.get<int>("id");
            p.Title = v.get<std::string>("title");
            p.Url = v.get<std::string>("url");
            p.Description = v.get<std::string>("description");

            p.Tags = slin::splitStrings(v.get<std::string>("tags"), delim);
            p.Notes = slin::splitStrings(v.get<std::string>("notes"), delim);
        }

        static void to_base(const slin::Link &p, values &v, indicator &ind)
        {
            v.set("id", p.m_id);
            v.set("title", p.Title);
            v.set("url", p.Url);
            v.set("description", p.Description);

            v.set("tags", slin::concatStrings(p.Tags, "\x91"));
            v.set("notes", slin::concatStrings(p.Notes, "\x91"));
            ind = i_ok;
        }
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
