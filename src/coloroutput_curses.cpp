#include "coloroutput.hpp"

#include <curses.h>
#include <term.h>

const std::string setColor(Color color)
{
	return tparm(set_a_foreground, color);
}

const std::string resetAttr()
{
	return tparm(exit_attribute_mode);
}

const std::string setBold()
{
	return tparm(enter_bold_mode);
}

void initTerminal()
{
	setupterm(NULL, 1, NULL);
}