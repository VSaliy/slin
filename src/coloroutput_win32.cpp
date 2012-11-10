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

#include "coloroutput.hpp"

#include <map>
#include <iostream>

#include <windows.h>

static HANDLE hConsole;

std::map<Color, WORD> colorMap =
{
	{Color::Black, 0},
	{Color::Red,         FOREGROUND_RED},
	{Color::Green,       FOREGROUND_GREEN},
	{Color::Orange,      FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY},
	{Color::Blue,        FOREGROUND_BLUE},
	{Color::Magenta,     FOREGROUND_RED | FOREGROUND_BLUE},
	{Color::Cyan,        FOREGROUND_GREEN | FOREGROUND_BLUE},
	{Color::Gray,        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN},
	{Color::DarkGray,    FOREGROUND_INTENSITY},
	{Color::LightRed,    FOREGROUND_RED | FOREGROUND_INTENSITY},
	{Color::LightGreen,  FOREGROUND_GREEN | FOREGROUND_INTENSITY},
	{Color::Yellow,      FOREGROUND_RED | FOREGROUND_GREEN},
	{Color::LightBlue,   FOREGROUND_BLUE | FOREGROUND_INTENSITY},
	{Color::LightMagenta,FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY},
	{Color::LightCyan,   FOREGROUND_GREEN | FOREGROUND_BLUE},
	{Color::White,       FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY}
};

const std::string setColor(Color color)
{
	SetConsoleTextAttribute(hConsole, colorMap[color]);
	return "";
}

const std::string resetAttr()
{
	setColor(Color::Gray);
	return "";
}

const std::string setBold()
{
	return "";
}

void initTerminal()
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
}
