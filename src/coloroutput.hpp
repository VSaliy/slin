#pragma once

#ifdef WITH_COLOR
#include <utio.h>

utio::CTerminfo ti;

const ustl::string setColor_(utio::EColor fg)
{
    if(config["color"].as<bool>() == true)
    {
        return ti.Color(fg);
    }
    else
    {
        return "";
    }
}

utio::CTerminfo::capout_t resetAttr_()
{
    if(config["color"].as<bool>() == true)
    {
        return ti.AllAttrsOff();
    }
    else
    {
        return "";
    }
}

void terminalLoad()
{
    ti.Load();
}

#define setColor(FG) setColor_(FG)
#define resetAttr() resetAttr_()
#else // WITH_COLOR
#define setColor(FG) ""
#define resetAttr() ""
void terminalLoad()
{
}
#endif // WITH_COLOR
