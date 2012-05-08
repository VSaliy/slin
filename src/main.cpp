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
    cout << setColor(utio::lightgreen) << "Added Link \"" << link.Title << "\"" << endl << "Link ID: " << link.GetID() << ti.AllAttrsOff() << endl;
}

void url_link(const vector<string> &urls)
{
    for(auto &url : urls)
    {
        slin::Link link;
        link.Title = slin::getWebsiteTitle(url);
        link.Url = url;
        link.Description = ""; // From where I can get this?
        db->AddLink(link);
        cout << setColor(utio::lightgreen) << "Added Link \"" << link.Title << "\"" << endl << "Link ID: " << link.GetID() << ti.AllAttrsOff() << endl;
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
        cout << setColor(utio::lightgreen) << "Updated Link" << endl << "ID: " << *id << ti.AllAttrsOff() << endl;
    }
    catch(const string &e)
    {
        cout << setColor(utio::red) << e << ti.AllAttrsOff() << endl;
    }

}

void describe_link(int *id, string *desc)
{
    slin::Link link;
    try
    {
        link = db->GetLink(*id);
        link.Description = *desc;
        db->UpdateLink(link);
        cout << setColor(utio::lightgreen) << "Updated Link" << endl << "ID: " << *id << ti.AllAttrsOff() << endl;
    }
    catch(const string &e)
    {
        cout << setColor(utio::red) << e << ti.AllAttrsOff() << endl;
    }
}

void view_link(int id)
{
    try
    {
        slin::Link link = db->GetLink(id);
        cout << setColor(utio::blue) << link.GetID() << ": " << link.Title << "(" << link.Url << "):" << endl;
        cout << setColor(utio::lightgreen);
        if(link.Description != "")
            cout << "  Description: " << link.Description << endl;
        if(not link.Tags.empty())
        {
            cout << "  Tags:";
            for(auto &tag : link.Tags)
                cout << " #" << tag;
            cout << endl;
        }
        cout << ti.AllAttrsOff();

    }
    catch(const string &e)
    {
        cout << setColor(utio::red) << e << ti.AllAttrsOff() << endl;
    }
}

void view_link(const vector<int> &ids)
{
    for(auto &id : ids)
    {
        view_link(id);
    }
}

void search_link(const vector<string> &args)
{
    for(auto &arg : args)
    {
        if(boost::algorithm::starts_with(arg, "#"))
        {
            for(auto &link : db->SearchTag(arg))
                view_link(link.GetID());
        }
        else
        {
            for(auto &link : db->Search(arg))
                view_link(link.GetID());
        }
    }
}

void remove_link(const vector<int> &ids)
{
    for(auto &id : ids)
    {
        db->RemoveLink(id);
        cout << setColor(utio::lightgreen) << "Removed Link" << endl << "ID: " << id << ti.AllAttrsOff() << endl;
    }
}

void show_version()
{
    cout << setColor(utio::green) << "Slin v0.1" << ti.AllAttrsOff() << endl;
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
    vector<string> url_urls;
    // Tag Command
    int *tag_id         = nullptr;
    vector<string> tag_tags;
    // Describe Command
    int *desc_id        = nullptr;
    string *desc_desc   = nullptr;
    // Search Command
    vector<string> search_args;
    // View Command
    vector<int> view_ids;
    // Remove Command
    vector<int> rem_ids;


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
        //     | url  | ...
        else if(*subcommand == "url")
        {
            // | url  | ...
            url_urls.emplace_back(argv[i]);
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
        // Describe Command
        //     |describe| ___ | ___
        else if(*subcommand == "describe")
        {
            if(desc_id == nullptr)
            {
                try
                {
                    desc_id = new int(boost::lexical_cast<int>(argv[i]));
                }
                catch(const boost::bad_lexical_cast&)
                {
                    cout << "Invalid number!" << endl;
                }
            }
            else
            {
                if(desc_desc == nullptr)
                    desc_desc = new string();
                else
                    *desc_desc += " "; // Append a space if it's not the first element
                *desc_desc += argv[i];
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
        // View Command
        //     | view | ...
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
        // Version Command
        //     |version
        else if(*subcommand == "version")
        {
            // Nothing important
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
        cout << endl << setColor(utio::red) << "Commandline parsing failed. Probably not enough arguments. " << ti.AllAttrsOff() << endl;
        if(subcommand)
            cout << "Subcommand: " << *subcommand << endl;
        goto cleanup; // Cleanup
    }

    // Run the specified subcommand
    if(*subcommand == "add")
        add_link(add_title, add_url, add_desc, add_tags);
    else if(*subcommand == "url")
        url_link(url_urls);
    else if(*subcommand == "tag")
        tag_link(tag_id, tag_tags);
    else if(*subcommand == "describe")
        describe_link(desc_id, desc_desc);
    else if(*subcommand == "search")
        search_link(search_args);
    else if(*subcommand == "view")
        view_link(view_ids);
    else if(*subcommand == "remove")
        remove_link(rem_ids);
    else if(*subcommand == "version")
        show_version();


cleanup: // I know this is not nice :)
    tryDelete(program_name);
    tryDelete(subcommand);
    tryDelete(add_title);
    tryDelete(add_url);
    tryDelete(add_desc);
    tryDelete(tag_id);
    tryDelete(desc_id);
    tryDelete(desc_desc);

    delete db;
    return 0;
}
