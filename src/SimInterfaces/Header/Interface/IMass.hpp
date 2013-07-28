#ifndef _IMass_hpp_
#define _IMass_hpp_

#include "Math/Vec3.hpp"
#include "Math/Mat3x3.hpp"

namespace Sim
{

namespace Interfaces
{

class IMass
{
public:
	struct Data
	{
		enum Flags {
			FLAG_NOTHING = 0,
			FLAG_MASS = 1 << 0,
			FLAG_CENTER_OF_MASS = 1 << 1,
			FLAG_INERTIA_TENSOR = 1 << 2,
			FLAG_ALL = FLAG_MASS | FLAG_CENTER_OF_MASS | FLAG_INERTIA_TENSOR
		} flags;
		double			mass;
		Math::Vec3		centerOfMass;
		Math::Matrix3x3	inertiaTensor;
	};
	virtual ~IMass() {;}
	virtual bool hasMass() const = 0;
	virtual bool getMass(Data & mass) const = 0;
};

}

}

#endif _IMass_hpp_