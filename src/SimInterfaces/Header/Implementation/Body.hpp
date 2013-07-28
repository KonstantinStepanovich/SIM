#ifndef _Body_hpp_
#define _Body_hpp_

#include "Interface/IBody.hpp"

#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"

namespace Sim
{

namespace Interfaces
{

class Body :	public IPosition,
				public IRotation
{
public:
	Body(	ModelTime time,
			const Math::Vec3 & position, const Math::Vec3 & velocity,
			const Math::Quat & rotation, const Math::Vec3 & angularVelocity);
	void set(	ModelTime time,
				const Math::Vec3 & position, const Math::Vec3 & velocity,
				const Math::Quat & rotation, const Math::Vec3 & angularVelocity);
	virtual void getPosition(IPosition::Data & data) const;
	virtual const Math::Vec3 & getForce() const;
	virtual void getRotation(IRotation::Data & data) const;
	virtual const Math::Vec3 & getTorque() const;
protected:
	ModelTime time_;
	Math::Vec3 position_;
	Math::Vec3 velocity_;
	Math::Vec3 force_;
	Math::Quat rotation_;
	Math::Vec3 angularVelocity_; //angular velocity in world frame reference
	Math::Vec3 torque_;
};

}

}

#endif _Body_hpp_