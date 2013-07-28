#ifndef _IMultiBody_hpp_
#define _IMultiBody_hpp_

#include <vector>

namespace Sim
{

namespace Interfaces
{

class IBody;

class IMultiBody
{
public:
	struct Body
	{
		size_t	index;
		IBody * body;
	};
	typedef std::vector<Body> ChangedBodies;
	virtual const ChangedBodies & getChangedBodies() const = 0;
};

}

}

#endif _IMultiBody_hpp_