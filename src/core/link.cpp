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

#include "link.hpp"

#include <iostream>

#include "regex.hpp"

slin::Link::Link()
    : m_id{-1}
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

    smatch what;
    bool remove = false;
    if(regex_match(tag, what, slin::regex::tag_format))
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
