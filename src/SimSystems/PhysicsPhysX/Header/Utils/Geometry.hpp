#ifndef _PhysicsPhysX_Geometry_hpp_
#define _PhysicsPhysX_Geometry_hpp_

#include "Interface/IBody.hpp"

#include "PhysXAPI/PxRigidActor.h"
//#include "PxFoundation/PxSimpleTypes.h"

namespace physx
{
class PxRigidActor;
class PxShape;
}

namespace Sim
{

class Variant;

namespace Interfaces
{
class IShape;
}

using Interfaces::IShape;

namespace PhysicsPhysX
{

class Body;

//Wrapper for physx::PxShape
class Geometry
{
	Geometry(const Geometry & geometry);
	Geometry & operator = (const Geometry & geometry);
public:
	Geometry(physx::PxShape * & shape);
	void reset();
	void setSphere(physx::PxRigidActor * rigidActor, physx::PxReal radius);
	void setBox(physx::PxRigidActor * rigidActor, physx::PxReal dx, physx::PxReal dy, physx::PxReal dz);
	void setCylinder(physx::PxRigidActor * rigidActor, physx::PxReal radius, physx::PxReal height);
	void load(physx::PxRigidActor * rigidActor, const Sim::Variant & variant);
	void onShapeChanged(physx::PxRigidActor * rigidActor, IShape * shape);
	inline physx::PxShape * get() const { return shape_; }
	inline bool empty() { return shape_ == NULL; }
private:
	physx::PxShape * & shape_;
};

}

}

#endif _PhysicsPhysX_Geometry_hpp_