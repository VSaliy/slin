#include "database.hpp"

#include <algorithm>

using namespace std;
using namespace soci;

slin::Database::Database(string filename)
    : m_filename{filename}
{
    this->sql = new session(sqlite3, filename);
    this->InitDB();
}

slin::Database::~Database()
{
}

void slin::Database::InitDB()
{
    *(this->sql) << "create table if not exists Links"
            "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,"
            "title TEXT NOT NULL,"
            "url TEXT NOT NULL,"
            "description TEXT,"
            "tags TEXT,"
            "notes TEXT);";
}

void slin::Database::AddLink(Link& link)
{
    string tags = concatStrings(link.Tags, "\x91"); //0x91 unicode: private use one
    string notes = concatStrings(link.Notes, "\x91");
    *(this->sql) << "insert into Links(title,url,description,tags,notes) values"
                "(:title,:url,:description,:tags,:notes);",
                use(link.Title),
                use(link.Url),
                use(link.Description),
                use(tags),
                use(notes);

    *(this->sql) << "select last_insert_rowid();", into(link.m_id);
}

void slin::Database::RemoveLink(int id)
{
    *(this->sql) << "delete from Links where id = :id", use(id);
}

slin::Link slin::Database::GetLink(int id)
{
    slin::Link link;
    string tags;
    string notes;
    char delim = (char)*("\0x91");

    link.m_id = id;
    *(this->sql) << "select title,url,description,tags,notes from Links where id = :id",
                    into(link.Title), into(link.Url), into(link.Description), into(tags), into(notes), use(id);
    
    link.Tags  = slin::splitStrings(tags , delim);
    link.Notes = slin::splitStrings(notes, delim);
    return link;
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
