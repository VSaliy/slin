#include "database.hpp"

#include <algorithm>

using namespace std;

slin::Database::Database(string filename)
    : m_filename{filename}
{
    this->check_sq_err(true, sqlite3_open(filename.c_str(), &this->m_db));
    this->InitDB();

    // Initialize prepared statements
    this->check_sq_err(true, sqlite3_prepare_v2(this->m_db, "INSERT INTO Links "
            "(title,url,description,tags,notes) VALUES"
            "(?,?,?,?,?);", -1, &this->add_stmt, 0));
    this->check_sq_err(true, sqlite3_prepare_v2(this->m_db, "SELECT last_insert_rowid();",
                -1, &this->last_id_stmt, 0));
    this->check_sq_err(true, sqlite3_prepare_v2(this->m_db, "DELETE FROM Links WHERE id = ?;",
                -1, &this->delete_stmt, 0));
    this->check_sq_err(true, sqlite3_prepare_v2(this->m_db, "SELECT * FROM Links WHERE ? = '%?%';",
                -1, &this->search_stmt, 0));
    this->check_sq_err(true, sqlite3_prepare_v2(this->m_db, "SELECT * FROM Links WHERE id = ?;",
                -1, &this->get_stmt, 0));
}

slin::Database::~Database()
{
    sqlite3_finalize(this->add_stmt);
    sqlite3_finalize(this->last_id_stmt);
    sqlite3_finalize(this->delete_stmt);
    sqlite3_close(this->m_db);
}

void slin::Database::InitDB()
{
    char *err_msg = nullptr;
    auto ret = sqlite3_exec(this->m_db, "CREATE TABLE IF NOT EXISTS Links"
            "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,"
            "title TEXT NOT NULL,"
            "url TEXT NOT NULL,"
            "description TEXT,"
            "tags TEXT,"
            "notes TEXT);", NULL, 0, &err_msg);
    if(ret)
    {
        std::cerr << "SQL ZError: " << err_msg;
        sqlite3_free(err_msg);
    }
}

void slin::Database::AddLink(Link& link)
{
    char *err_msg = nullptr;
    check_sq_err(true, sqlite3_bind_text(this->add_stmt, 1, link.Title.c_str(), -1, SQLITE_TRANSIENT));
    check_sq_err(true, sqlite3_bind_text(this->add_stmt, 2, link.Url.c_str(), -1, SQLITE_TRANSIENT));
    check_sq_err(true, sqlite3_bind_text(this->add_stmt, 3, link.Description.c_str(), -1, SQLITE_TRANSIENT));

    string conc_tags = concatStrings(link.Tags, "§");
    check_sq_err(true, sqlite3_bind_text(this->add_stmt, 4, conc_tags.c_str(), -1, SQLITE_TRANSIENT));

    string conc_notes = concatStrings(link.Notes, "§");
    check_sq_err(true, sqlite3_bind_text(this->add_stmt, 5, conc_notes.c_str(), -1, SQLITE_TRANSIENT));
    
    sqlite3_step(this->add_stmt);

    check_sq_err(true, sqlite3_reset(this->add_stmt));

    link.m_id = this->get_last_id();
}

void slin::Database::RemoveLink(int id)
{
    check_sq_err(true, sqlite3_bind_int(this->delete_stmt, 1, id));
    sqlite3_step(this->delete_stmt);
    check_sq_err(true, sqlite3_reset(this->delete_stmt));
}

slin::Link slin::Database::GetLink(int id)
{
    check_sq_err(true, sqlite3_bind_int(this->get_stmt, 1, id));
    int ret = 0;
    while(true)
    {
        ret = sqlite3_step(this->get_stmt);
        if(ret == SQLITE_ROW)
        {
            // We got a value
            


            check_sq_err(true, sqlite3_reset(this->delete_stmt));
            return link;
        }
    }
    check_sq_err(true, sqlite3_reset(this->delete_stmt));
    throw string("Link not found");
}

unordered_set<slin::Link> slin::Database::Search(string query)
{
    unordered_set<slin::Link> ret;
    for(auto &item : this->SearchTitle(query))
        ret.insert(item);
    for(auto &item : this->SearchLink(query))
        ret.insert(item);
    for(auto &item : this->SearchDescription(query))
        ret.insert(item);

    return ret;
}
unordered_set<slin::Link> slin::Database::SearchTitle(string query)
{
}
unordered_set<slin::Link> slin::Database::SearchLink(string query)
{
}
unordered_set<slin::Link> slin::Database::SearchDescription(string query)
{
}
unordered_set<slin::Link> slin::Database::SearchTag(string query)
{
}

int slin::Database::get_last_id()
{
    int ret;
    while(true) // Step until we get a row
    {
        ret = sqlite3_step(this->last_id_stmt);
        if(ret == SQLITE_ROW)
        {
            return sqlite3_column_int(this->last_id_stmt, 0);
        }
        else
        {
            this->check_sq_err(true, ret);
        }
    }
}

void slin::Database::check_sq_err(bool throw_ex, int err)
{
    if(!(err == SQLITE_OK))
    {
        cerr << "SQL Error(" << err << "): " << sqlite3_errmsg(this->m_db) << endl;
        if(throw_ex)
        {
            throw string("SQLite3 Error: " + string(sqlite3_errmsg(this->m_db)));
        }
    }
}
