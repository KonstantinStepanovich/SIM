#ifndef _IBody_hpp_
#define _IBody_hpp_

#include "Core/ModelTime/ModelTime.hpp"
#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"

namespace Sim
{

namespace Interfaces
{

class IPosition
{
public:
	struct Data
	{
		ModelTime time;
		Math::Vec3 position;
		Math::Vec3 velocity;
		Math::Vec3 acceleration;
	};
	virtual ~IPosition() {;}
	virtual void getPosition(Data & data) const = 0;
};

class IRotation
{
public:
	struct Data
	{
		ModelTime time;
		Math::Quat rotation;
		Math::Vec3 angularVelocity; //angular velocity in world frame reference
		Math::Vec3 angularAcceleration; //angular acceleration in world frame reference
	};
	virtual ~IRotation() {;}
	virtual void getRotation(Data & data) const = 0;
};

}

}

#endif _IBody_hpp_