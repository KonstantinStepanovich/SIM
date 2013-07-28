#ifndef _IDEbugObject_hpp_
#define _IDEbugObject_hpp_

namespace Sim
{

namespace Interfaces
{

class IDebugObject
{
public:
	virtual const char * getString() const = 0;
};

}

}

#endif _IMass_hpp_