#ifndef _IDynamicBody_hpp_
#define _IDynamicBody_hpp_

#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"

namespace Sim
{

namespace Interfaces
{

class IDynamicBody
{
public:
	virtual const Math::Vec3 & getForce() const = 0;
	virtual const Math::Vec3 & getTorque() const = 0;
};

}

}

#endif _IDynamicBody_hpp_