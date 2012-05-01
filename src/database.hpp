#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include <soci.h>
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

        void AddLink(Link& link);
        void RemoveLink(int id); 
        Link GetLink(int id);
        
        // Query functions
        std::unordered_set<Link> Search(std::string query);
        std::unordered_set<Link> SearchTitle(std::string query);
        std::unordered_set<Link> SearchLink(std::string query);
        std::unordered_set<Link> SearchDescription(std::string query);
        std::unordered_set<Link> SearchTag(std::string query);

    private:
        std::string m_filename;
        soci::session *sql;
    };
}
