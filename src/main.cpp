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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#ifdef _linux_
#include <stdlib.h> // getenv()
#endif

#include "database.hpp"
#include "link.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "coloroutput.hpp"
#include "configfile.hpp"

slin::Database *db;

using namespace std;
namespace fs = boost::filesystem;

template<class T>
bool tryDelete(T *p)
{
    if(p != nullptr)
    {
        delete p;
        return true;
    }
    else
    {
        return false;
    }
}

void add_link(string *title, string *url, string *description, const vector<string> &tags)
{
    if(db->ExistsUrl(*url))
    {
        cout << setColor(Color::Red) << "Link already in Database.";
            cout << resetAttr() << endl;
        return;
    }

    slin::Link link(*title, *url, description == nullptr ? "" : *description);
    for(auto &tag : tags)
        link.Tag(tag);
    db->AddLink(link);
    cout << setColor(Color::LightGreen) << "Added Link \"" << link.Title << "\"" << endl << "Link ID: " << link.GetID();
        cout << resetAttr() << endl;
}

void url_link(const vector<string> &args)
{
    slin::Link link;
    for(auto &arg : args)
    {
        if(slin::isTag(arg))
        {
            // If it's a tag add it to the previous link
            link.Tag(arg);
            db->UpdateLink(link);
        }
        else
        {
            if(db->ExistsUrl(arg))
            {
                cout << setColor(Color::Red) << "Link already in Database.";
                    cout << resetAttr() << endl;
                continue;
            }

            link = slin::Link();
            auto html = slin::getWebsite(arg);

            link.Title = slin::getWebsiteTitle(html);
            link.Url = arg;
            link.Description = slin::getWebsiteDescription(html);
            db->AddLink(link);
            cout << setColor(Color::LightGreen) << "Added Link \"" << link.Title << "\"" << endl << "Link ID: " << link.GetID();
                cout << resetAttr() << endl;

            delete html;
        }
    }
}

void tag_link(int *id, const vector<string> &tags)
{
    slin::Link link;
    try
    {
        link = db->GetLink(*id);
        for(auto &tag : tags)
        {
            link.Tag(tag);
        }
        db->UpdateLink(link);
        cout << setColor(Color::LightGreen) << "Updated Link" << endl << "ID: " << *id;
            cout << resetAttr() << endl;
    }
    catch(const string &e)
    {
        cout << setColor(Color::Red) << e;
            cout << resetAttr() << endl;
    }

}

void view_link(int id, bool quit=false)
{
    try
    {
        slin::Link link = db->GetLink(id);
        cout << setColor(Color::Green) << setw(4) << link.GetID() << ": ";
            cout << setBold() << setColor(Color::LightBlue) << link.Title << " (" << link.Url << ")";
        cout << ((quit||(link.Description.empty() && link.Tags.empty())) ? "":":") << endl;

        cout << resetAttr();
        if(!quit)
        {
            if(link.Description != "")
            {
                cout << setColor(Color::Orange);
                cout << "      Description: " << setColor(Color::Yellow) << link.Description << endl;
            }

            if(not link.Tags.empty())
            {
                cout << setColor(Color::Orange);
                cout << "      Tags:";
                cout << setColor(Color::Yellow);
                for(auto &tag : link.Tags)
                    cout << " #" << tag;
                cout << endl;
            }
        }
        cout << resetAttr();

    }
    catch(const string &e)
    {
        cout << setColor(Color::Red) << e;
            cout << resetAttr() << endl;
    }
}

void view_link(const vector<int> &ids)
{
    for(auto &id : ids)
    {
        view_link(id);
    }
}

void search_link(const vector<string> &args, bool quit=false)
{
    for(auto &arg : args)
    {
        if(boost::algorithm::starts_with(arg, "#"))
        {
            for(auto &link : db->SearchTag(arg))
                view_link(link.GetID(), quit);
        }
        else
        {
            for(auto &link : db->Search(arg))
                view_link(link.GetID(), quit);
        }
    }
}

void remove_link(const vector<int> &ids)
{
    for(auto &id : ids)
    {
        db->RemoveLink(id);
        cout << setColor(Color::LightGreen) << "Removed Link" << endl << "ID: " << id;
            cout << resetAttr() << endl;
    }
}

void set_link(int *id, const vector<string> &args)
{
    for(const auto &arg : args)
    {
        try
        {
            auto entry = slin::splitConfigEntry(arg);
            slin::Link link = db->GetLink(*id);
            if(entry.first == "description")
            {
                link.Description = entry.second;
            }
            else if(entry.first == "title")
            {
                link.Title = entry.second;
            }
            else if(entry.first == "url")
            {
                link.Url = entry.second;
            }
            db->UpdateLink(link);
        }
        catch(const string& e)
        {
            cout << setColor(Color::Red) << e;
                cout << resetAttr() << endl;
            return;
        }
    }
    cout << setColor(Color::LightGreen) << "Updated Link" << endl << "ID: " << *id;
        cout <<resetAttr() << endl;
}

void show_version()
{
    cout << setColor(Color::Green) << "Slin v0.1" << resetAttr() << endl;
}

inline static void _print_help(string command, string arguments, string description)
{
    #define HELP_ALIGN 10
    cout << setColor(Color::Yellow) << setw(HELP_ALIGN) << command + " ";
        cout << setColor(Color::LightBlue) << arguments << endl;
    cout << setColor(Color::White) << string(HELP_ALIGN, ' ') << description << endl;
}

void show_help()
{
    cout << setColor(Color::Green) << "Help: " << endl;

    _print_help("add",      "<title> <url> [<description> [#<tag>...]]", "Adds a link to the database.");
    _print_help("url",      "<url> [#<tag>...] [<url> [#<tag>...]]...", "Adds a link to the database based on an url.");
    _print_help("tag",      "<id> [#[+|-]<tag>...]", "Adds or Removes an tag from a link.");
    _print_help("search",   "<query|#[+|-]tag>...", "Search links.");
    _print_help("searchq",  "<query|#[+|-]tag>...", "Search links but only display ID, name and URL.");
    _print_help("view",     "<id>...", "Display information about a link.");
    _print_help("remove",   "<id>...", "Removes a link from the database.");
    _print_help("set",      "<id> [<key>=<value>]...", "Set a property of a link.('title', 'url', 'description')");
    _print_help("version",  "", "Shows the version of slin.");
    _print_help("help",     "", "Shows this help.");

    cout << resetAttr();
}

int main(int argc, char **argv)
{
    // Load config file
    string config_filename = slin::findConfig("slin.yml");

    ConfigFile myConfig(config_filename);

    // Default Config
    myConfig.Set("database", ConfigItem{slin::findConfig("slin.db")});
    myConfig.Set("color", ConfigItem{"true"});

    initTerminal();

    string database_filename = myConfig["database"]();
    db = new slin::Database(database_filename);

    ////////////////////
    // Parse Commandline
    bool done = false;
    // Top Level
    string *program_name = nullptr;
    string *subcommand   = nullptr;
    // Add Command
    string *add_title   = nullptr;
    string *add_url     = nullptr;
    string *add_desc    = nullptr;
    vector<string> add_tags;
    // URL Command
    vector<string> url_args;
    // Tag Command
    int *tag_id         = nullptr;
    vector<string> tag_tags;
    // Search Command
    vector<string> search_args;
    // Searchq Command
    vector<string> searchq_args;
    // View Command
    vector<int> view_ids;
    // Remove Command
    vector<int> rem_ids;
    // Set Command
    int *set_id         = nullptr;
    vector<string> set_args;

    for(int i = 0; i < argc; i++)
    {
        // Program name
        // ### | ...
        if(program_name == nullptr)
        {
            program_name = new string(argv[i]);
            continue;
        }
        // Subcommand
        //     | ###  | ...
        else if(subcommand == nullptr)
        {
            subcommand = new string(argv[i]);

            if(*subcommand == "version") //HACK: Dirty Bugfix. The "done = true;" below doesn't gets called
                done = true;
            if(*subcommand == "help")
                done = true;

            continue;
        }
        // Add Command
        //     | add  | ___ | ___ | ...
        else if(*subcommand == "add")
        {
            // | add  | ### | ___ | ...
            if(add_title == nullptr)
            {
                add_title = new string(argv[i]);
            }
            // | add  |     | ### | ...
            else if(add_url == nullptr)
            {
                add_url = new string(argv[i]);
                done = true; // The following arguments are optional
            }
            // | add  |     |     | ### | ...
            else if(add_desc == nullptr)
            {
                add_desc = new string(argv[i]);
            }
            // Tags
            // | add  |     |     |     | ...
            else if(slin::isTag(argv[i]))
            {
                add_tags.emplace_back(argv[i]);
            }
        }
        // URL Command
        //     | url  | ...
        else if(*subcommand == "url")
        {
            // | url  | ...
            url_args.emplace_back(argv[i]);
            done = true;
        }
        // Tag Command
        //     | tag  | ___ | ...
        else if(*subcommand == "tag")
        {
            // | tag  | ### | ...
            if(tag_id == nullptr)
            {
                try
                {
                    tag_id = new int(boost::lexical_cast<int>(argv[i]));
                }
                catch(const boost::bad_lexical_cast&)
                {
                    cout << "Invalid number!" << endl;
                }
            }
            // | tag  |     | ...
            else if(boost::algorithm::starts_with(argv[i], "#"))
            {
                tag_tags.emplace_back(argv[i]);
                done = true;
            }
        }
        // Search Command
        //     |search| ...
        else if(*subcommand == "search")
        {
            search_args.emplace_back(argv[i]);
            done = true;
        }
        // Searchq Command
        //     |searchq| ...
        else if(*subcommand == "searchq")
        {
            searchq_args.emplace_back(argv[i]);
            done = true;
        }
        // View Command
        //     | view | ___
        else if(*subcommand == "view")
        {
            try
            {
                view_ids.emplace_back(boost::lexical_cast<int>(argv[i]));
                done = true;
            }
            catch(const boost::bad_lexical_cast&)
            {
                cout << "Invalid number!" << endl;
            }

        }
        // Remove Command
        //     |remove| ...
        else if(*subcommand == "remove")
        {
            try
            {
                rem_ids.emplace_back(boost::lexical_cast<int>(argv[i]));
                done = true;
            }
            catch(const boost::bad_lexical_cast&)
            {
                cout << "Invalid number!" << endl;
            }
        }
        // Set Command
        //     | set | ___ | ...
        else if(*subcommand == "set")
        {
            // | set | ### | ...
            if(set_id == nullptr)
            {
                try
                {
                    set_id = new int(boost::lexical_cast<int>(argv[i]));
                }
                catch(const boost::bad_lexical_cast& e)
                {
                    cout << "Invalid ID" << endl;
                }
            }
            // | set |     | ###
            else
            {
                set_args.emplace_back(argv[i]);
                done = true;
            }
        }
        // Version Command
        //     |version
        else if(*subcommand == "version")
        {
            // Nothing important
            done = true;
        }
        else if(*subcommand == "help")
        {
            done = true;
        }
        else
        {
            cout << "I don't know what '" << *subcommand << "' means" << endl;
        }
    }

    // Check if there were not enough/wrong arguments
    if(!done)
    {
        cout << endl << setColor(Color::Red) << "Commandline parsing failed. Probably not enough arguments. ";
            cout << resetAttr() << endl;
        if(subcommand)
            cout << "Subcommand: " << *subcommand << endl;

        show_help();

        goto cleanup; // Cleanup
    }

    // Run the specified subcommand
    if(*subcommand == "add")
        add_link(add_title, add_url, add_desc, add_tags);
    else if(*subcommand == "url")
        url_link(url_args);
    else if(*subcommand == "tag")
        tag_link(tag_id, tag_tags);
    else if(*subcommand == "search")
        search_link(search_args);
    else if(*subcommand == "searchq")
        search_link(searchq_args, true);
    else if(*subcommand == "view")
        view_link(view_ids);
    else if(*subcommand == "remove")
        remove_link(rem_ids);
    else if(*subcommand == "set")
        set_link(set_id, set_args);
    else if(*subcommand == "version")
        show_version();
    else if(*subcommand == "help")
        show_help();


cleanup: // I know this is not nice :)
    tryDelete(program_name);
    tryDelete(subcommand);
    tryDelete(add_title);
    tryDelete(add_url);
    tryDelete(add_desc);
    tryDelete(tag_id);
    tryDelete(set_id);

    delete db;
    return 0;
}
