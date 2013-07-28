#ifndef _Animated_hpp_
#define _Animated_hpp_

#include "Interface/IAnimated.hpp"

#include "Math/Box3.hpp"

namespace Sim
{

namespace Interfaces
{

class Animated : public IAnimated
{
public:
	void clearArgs()
	{
		args.clear();
	}
	void addArg(ArgIndex index, ArgValue value)
	{
		Arg arg = { index, value };
		args.push_back(arg);
	}
	virtual bool getArgs(Args & argsOut) const
	{
		argsOut = args;
		return !args.empty();
	}
	virtual Math::Box3 getOBB() const
	{
		return obb;
	}
protected:
	Math::Box3 obb;
	Args args;
};

}

}

#endif _IAnimated_hpp_