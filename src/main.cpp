#include <iostream>
#include <fstream>
#include <string>
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
    slin::Link l1("Stack Overflow", "www.stackoverflow.com", "A free programming Q&A Site"); l1.AddTag("programming"); l1.AddTag("answers"); l1.AddTag("questions");
    slin::Link l2("Arch Linux", "www.archlinux.org", "A very cool Linux distro, which follows the KISS princip"); l2.AddTag("linux"); l2.AddTag("kiss"); l2.AddTag("awesome");
    slin::Link l3("Ubuntu", "www.ubuntu.com", "A popular Linux distro, which is made for Linux newbies"); l3.AddTag("linux"); l3.AddTag("ubuntu"); l3.AddTag("easy");
    slin::Link l4("Twitter", "www.twitter.com", "A very famous short message platform"); l4.AddTag("short message"); l4.AddTag("news"); l4.AddTag("twitter");
    slin::Link l5("Identi.ca", "www.identi.ca", "Yet another short message platform"); l5.AddTag("short message"); l5.AddTag("news");

    db->AddLink(l1);
    db->AddLink(l2);
    db->AddLink(l3);
    db->AddLink(l4);
    db->AddLink(l5);

    cout << "Search testing: " << endl << "===============" << endl;
    auto linux = db->Search("linux");
    cout << "Search: 'linux'" << endl;
    for(auto &item : linux)
        cout << item.Title << endl;

    delete db;
    return 0;
}
