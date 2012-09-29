#pragma once

#include <string>

enum Color
{
    Black,
    Red,
    Green,
    Orange,
    Blue,
    Magenta,
    Cyan,
    Gray,
    DarkGray,
    LightRed,
    LightGreen,
    Yellow,
    LightBlue,
    LightMagenta,
    LightCyan,
    White
};

const std::string setColor(Color color);
const std::string resetAttr();
const std::string setBold();
void initTerminal();
