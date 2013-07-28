#ifndef _PhysicsODE_Body_hpp_
#define _PhysicsODE_Body_hpp_

#include "Utils/Geometry.hpp"

#include "Interface/IBody.hpp"

#include "Math/Vec3.hpp"
#include "Core/ModelTime/ModelTime.hpp"

#include <ode/common.h>

namespace Sim
{

class Variant;

namespace Interfaces
{
class IPosition;
class IRotation;
class IMass;
class IDynamicBody;
}

using Interfaces::IPosition;
using Interfaces::IRotation;
using Interfaces::IMass;
using Interfaces::IDynamicBody;
using Interfaces::IEntity;
using Interfaces::IAnimated;

namespace PhysicsODE
{

class Object;

class Body
{
	Body(const Body & body);
	Body & operator = (const Body & body);
public:
	static std::string path;
	Body(dWorldID worldID, dSpaceID spaceID, Object * object);
	~Body();

	void getPosition(ModelTime time, IPosition::Data & data) const;
	void getRotation(ModelTime time, IRotation::Data & data) const;
	const Math::Box3 & getOBB() const;

	void onEntityChanged(IEntity * entity);
	void onPositionChanged(IPosition * position);
	void onRotationChanged(IRotation * rotation);
	void onMassChanged(IMass * mass);
	void onDynamicChanged(IDynamicBody * dynamicBody);
	void onAnimationChanged(IAnimated * animated);

	void update();
	virtual void onMove();
	bool isKinematic() const;
	inline dBodyID getID() const { return id_; }
private:
	dBodyID				id_;
	Object *			object_;
	Math::Vec3			force_;
	Math::Vec3			torque_;
	mutable Math::Vec3	prevVelocity_;
	mutable ModelTime	prevVelocityTime_;
	mutable Math::Vec3	acceleration_;
	Geometry			geometry_;
};

}

}

#endif _PhysicsODE_Body_hpp_