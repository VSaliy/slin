#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "database.hpp"
#include "link.hpp"

#ifdef _linux_
#include <stdlib.h> // getenv()
#endif

#include <utio/ti.h> // Keep it the last include! It's dangerous :)

using namespace std;
namespace fs = boost::filesystem;

utio::CTerminfo ti;
YAML::Node config;
slin::Database *db;

fs::path searchConfig()
{
#ifdef _WIN32
    //TODO: Implement searchConfig for Windows
    return fs::current_path() / "slin.yml";

#elif __linux__
    auto path = getenv("XDG_CONFIG_HOME");
    if(path != NULL)
    {
        return fs::path(path) / "slin.yml";
    }
    path = getenv("HOME");
    if(path != NULL)
    {
        return fs::path(path) / ".slin.yml";
    }
    return fs::current_path() / "slin.yml";

#elif __APPLE__
    //TODO: Implement searchConfig for Mac
    return fs::current_path() / "slin.yml";

#else
    #warning "Unknown Operating System!"
    cerr << "Unknown Operating System!" << endl;
    return fs::current_path() / "slin.yml";
#endif
}

void writeDefaultConfig(string filename)
{
    ofstream fout(filename);

    YAML::Node config;
    config["color"] = true;
    config["database"] = (searchConfig().parent_path() / "slin.db").native();
    
    fout << config;
    fout.close();
}

const ustl::string setColor(utio::EColor fg, utio::EColor bg = utio::color_Preserve)
{
    if(config["color"].as<bool>() == true)
    {
        return ti.Color(fg, bg);
    }
    else
    {
        return ustl::string();
    }
}

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
    slin::Link link(*title, *url, description == nullptr ? "" : *description);
    for(auto &tag : tags)
        link.Tag(tag);
    db->AddLink(link);
    cout << setColor(utio::lightgreen) << "Added" << endl << "Link ID: " << link.GetID() << ti.AllAttrsOff() << endl;
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
        cout << setColor(utio::lightgreen) << "Updated Link" << endl << "ID: " << *id << ti.AllAttrsOff() << endl;
    }
    catch(const string &e)
    {
        cout << setColor(utio::red) << e << ti.AllAttrsOff() << endl;
    }

}

void remove_link(int *id)
{
    db->RemoveLink(*id);
    cout << setColor(utio::lightgreen) << "Removed Link" << endl << "ID: " << *id << ti.AllAttrsOff() << endl;
}

int main(int argc, char **argv)
{
    // Load config file
    fs::path config_filename = searchConfig();
    if(!fs::exists(config_filename))
    {
        writeDefaultConfig(config_filename.native());
    }
    config = YAML::LoadFile(config_filename.native());

    // Initialize colors
    ti.Load();
    cout << setColor(utio::green) << "slin v0.1" << ti.AllAttrsOff() << endl;
    
    // Open Database
    if(config["database"])
    {
        string database_filename = config["database"].as<string>();
        db = new slin::Database(database_filename);
    }
    else
    {
        cerr << "Can't find 'database' key in config" << endl;
        return 1;
    }
    
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
    string *url_url     = nullptr;
    // Tag Command
    int *tag_id         = nullptr;
    vector<string> tag_tags;
    // Remove Command
    int *rem_id         = nullptr;


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
            else if(boost::algorithm::starts_with(argv[i], "#"))
            {
                add_tags.emplace_back(argv[i]);
            }
        }
        // URL Command
        //     | url  | ___
        else if(*subcommand == "url")
        {
            // | url  | ###
            if(url_url == nullptr)
            {
                url_url = new string(argv[i]);
                done = true;
            }
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
        // Remove Command
        //     |remove| ___
        else if(*subcommand == "remove")
        {
            // |remove| ###
            if(rem_id == nullptr)
            {
                try
                {
                    rem_id = new int(boost::lexical_cast<int>(argv[i]));
                    done = true;
                }
                catch(const boost::bad_lexical_cast&)
                {
                    cout << "Invalid number!" << endl;
                }
            }
        }
        else
        {
            cout << "I don't know what '" << *subcommand << "' means" << endl;
        }
    }
    // Check if there were not enough/wrong arguments
    if(!done)
    {
        cout << endl << setColor(utio::red) << "Commandline parsing failed. Probably not enough arguments. " << ti.AllAttrsOff() << endl;
        if(subcommand)
            cout << "Subcommand: " << *subcommand << endl;
        goto cleanup; // Cleanup
    }

    // Run the specified subcommand
    if(*subcommand == "add")
        add_link(add_title, add_url, add_desc, add_tags);
    else if(*subcommand == "url")
        cout << "Unimplemented" << endl;
    else if(*subcommand == "tag")
        tag_link(tag_id, tag_tags);
    else if(*subcommand == "remove")
        remove_link(rem_id);


cleanup: // I know this is not nice :)
    tryDelete(program_name);
    tryDelete(subcommand);
    tryDelete(add_title);
    tryDelete(add_url);
    tryDelete(add_desc);
    tryDelete(url_url);
    tryDelete(tag_id);
    tryDelete(rem_id);

    delete db;
    return 0;
}
