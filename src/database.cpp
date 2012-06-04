#include "database.hpp"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

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
    delete this->sql;
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

void slin::Database::AddLink(Link &link)
{
    if(link.GetID() > 0) // The Link is probably already in the DB
        return;

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

void slin::Database::UpdateLink(const Link &link)
{
    if(link.GetID() < 0) // The Link is probably not in the DB
        return;

    string tags  = concatStrings(link.Tags, "\x91");
    string notes = concatStrings(link.Notes, "\x91");
    *this->sql << "update Links set title=:title,url=:url,description=:description,tags=:tags,notes=:notes where id=:id",
                  use(link.Title),
                  use(link.Url),
                  use(link.Description),
                  use(tags),
                  use(notes),
                  use(link.GetID());
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
    char delim = (char)*("\x91");

    link.m_id = id;
    *(this->sql) << "select title,url,description,tags,notes from Links where id = :id",
                    into(link.Title), into(link.Url), into(link.Description), into(tags), into(notes), use(id);
    if(not this->sql->got_data())
        throw string("There is no Link with ID=" + boost::lexical_cast<string>(id));
    
    link.Tags  = slin::splitStrings(tags , delim);
    link.Notes = slin::splitStrings(notes, delim);
    return link;
}

set<slin::Link> slin::Database::Search(string query)
{
    set<slin::Link> ret;
    for(auto &item : this->SearchTitle(query))
        ret.insert(item);
    for(auto &item : this->SearchLink(query))
        ret.insert(item);
    for(auto &item : this->SearchDescription(query))
        ret.insert(item);

    return ret;
}
set<slin::Link> slin::Database::SearchTitle(string query)
{
    set<slin::Link> res;
    rowset<int> rs = ((this->sql->prepare) << "select rowid from Links where title like :q", use("%" + query + "%"));
    for(auto &id : rs)
    {
        res.insert(this->GetLink(id));
    }
    return res;
}
set<slin::Link> slin::Database::SearchLink(string query)
{
    set<slin::Link> res;
    rowset<int> rs = (this->sql->prepare << "select rowid from Links where url like :q", use("%" + query + "%"));
    for(auto &id : rs)
    {
        res.insert(this->GetLink(id));
    }
    return res;
}
set<slin::Link> slin::Database::SearchDescription(string query)
{
    set<slin::Link> res;
    rowset<int> rs = (this->sql->prepare << "select rowid from Links where description like :q", use("%" + query + "%"));
    for(auto &id : rs)
    {
        res.insert(this->GetLink(id));
    }
    return res;
}
set<slin::Link> slin::Database::SearchTag(string query)
{
    if(!boost::algorithm::starts_with(query, "#"))
        return set<slin::Link>();
    query.erase(0, 1); // Delete the first character("#")

    set<slin::Link> res;
    rowset<int> rs = (this->sql->prepare << "select rowid from Links where tags like :q", use("%" + query + "%"));
    for(auto &id : rs)
    {
        res.insert(this->GetLink(id));
    }
    return res;
}
