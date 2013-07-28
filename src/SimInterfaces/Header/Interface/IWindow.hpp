#ifndef _IWindow_hpp_
#define _IWindow_hpp_

#include <windows.h>

namespace Sim
{

namespace Interfaces
{

class IWindow
{
public:
	virtual ~IWindow() {;}
	virtual HWND getWindowHandle() const = 0;
};

}

}

#endif _IWindow_hpp_