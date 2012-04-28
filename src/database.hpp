#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <sqlite3.h>
#include <boost/filesystem.hpp>

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
        void check_sq_err(bool throw_ex, int err);
        int get_last_id();

        std::string m_filename;
        sqlite3 *m_db;
        // Prepared statements
        sqlite3_stmt *add_stmt;
        sqlite3_stmt *delete_stmt;
        sqlite3_stmt *get_stmt;
        sqlite3_stmt *last_id_stmt;
        sqlite3_stmt *search_stmt;
    };
}
