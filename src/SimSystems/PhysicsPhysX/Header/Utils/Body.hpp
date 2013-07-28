#ifndef _PhysicsPhysX_Body_hpp_
#define _PhysicsPhysX_Body_hpp_

#include "Interface/IBody.hpp"

#include "Math/Vec3.hpp"
#include "Core/ModelTime/ModelTime.hpp"

namespace physx
{
class PxPhysics;
class PxRigidDynamic;
}

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

namespace PhysicsPhysX
{

class Object;

class Body
{
	Body(const Body & body);
	Body & operator = (const Body & body);
public:
	Body(Object * object, physx::PxPhysics * SDK);
	~Body();
	void load(const Variant & variant);

	void getPosition(ModelTime time, IPosition::Data & data) const;
	void getRotation(ModelTime time, IRotation::Data & data) const;

	void onPositionChanged(IPosition * position);
	void onRotationChanged(IRotation * rotation);
	void onMassChanged(IMass * mass);
	void onDynamicChanged(IDynamicBody * dynamicBody);

	void setForce(const Math::Vec3 & force) { force_ = force; }

	void update();
	void onMove(ModelTime modelTime, ModelTime dtime);
	bool isKinematic() const;
	inline physx::PxRigidDynamic * get() const { return id_; }

private:
	physx::PxRigidDynamic * id_;
	Object *			object_;
	Math::Vec3			force_;
	Math::Vec3			torque_;
};

}

}

#endif _PhysicsPhysX_SingleGeometryBody_hpp_