#include <iostream>
#include <fstream>
#include <string>
#include <sqlite3.h>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>

#include "database.hpp"
#include "link.hpp"

#ifdef _linux_
#include <stdlib.h> // getenv()
#endif

#include <utio/ti.h> // Keep it the last include! Dangerous :)

using namespace std;
namespace fs = boost::filesystem;

utio::CTerminfo ti;
YAML::Node config;

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

int main()
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
    cout << setColor(utio::lightgreen) << "slin v0.1" << ti.AllAttrsOff() << endl;
    
    // Open Database
    slin::Database *db;
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
    
    // Main code
    slin::Link mylink("Google", "http://www.google.de", "A very famous search engine");
    mylink.Tags.insert("search engine");
    mylink.Tags.insert("google");
    db->AddLink(mylink);
    cout << "ID: " << mylink.GetID() << endl;
    db->RemoveLink(mylink.GetID());

    delete db;

    return 0;
}
