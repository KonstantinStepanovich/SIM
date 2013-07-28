#ifndef _Utilities_hpp_
#define _Utilities_hpp_

#include <QColor.h>

#include "Database/Config.hpp"

void load(Sim::Config & config, QColor & color)
{
	int r = config.get<int>(1);
	int g = config.get<int>(2);
	int b = config.get<int>(3);
	int a = !config.empty(4) ? config.get<int>(4) : 255;
	color = QColor(r, g, b, a);
}

#endif _Utilities_hpp_