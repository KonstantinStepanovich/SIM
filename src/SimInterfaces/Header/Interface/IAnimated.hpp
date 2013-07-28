#ifndef _IAnimated_hpp_
#define _IAnimated_hpp_

namespace Sim
{

namespace Math
{
	class Box3;
}

namespace Interfaces
{

class IAnimated
{
public:
	typedef unsigned int ArgIndex;
	typedef double ArgValue;
	struct Arg
	{
		ArgIndex	index;
		ArgValue	value;
	};
	typedef std::vector<Arg> Args;
	virtual ~IAnimated() {;}
	virtual bool getArgs(Args & args) const = 0;
	virtual Math::Box3 getOBB() const = 0;
};

}

}

#endif _IAnimated_hpp_