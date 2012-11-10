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
#include <vector>
#include <set>
#include <boost/filesystem.hpp>

// Prevent warnings from SOCI
#pragma GCC system_header
#include <soci.h>
#pragma GCC system_header
#include <soci-sqlite3.h>

#include "link.hpp"
#include "utils.hpp"

namespace slin
{
    class Database
    {
    public:
        Database(std::string filename);
        ~Database();
        void InitDB();
        //std::vector<Link> Query(std::string query);

        void AddLink(Link &link);
        void UpdateLink(const Link &link);
        void RemoveLink(int id);
        Link GetLink(int id);

        // Query functions
        std::set<Link> Search(std::string query);
        std::set<Link> SearchTitle(std::string query);
        std::set<Link> SearchLink(std::string query);
        std::set<Link> SearchDescription(std::string query);
        std::set<Link> SearchTag(std::string query);

        bool ExistsUrl(std::string url);
    private:
        std::string m_filename;
        soci::session *sql;
    };
}
