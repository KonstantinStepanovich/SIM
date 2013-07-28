#ifndef _PhysicsPhysX_Ground_hpp_
#define _PhysicsPhysX_Ground_hpp_

#include "Object.hpp"

namespace physx
{
class PxRigidStatic;
}

namespace Sim
{

class Variant;

namespace PhysicsPhysX
{

class Ground : public Object
{
public:
	Ground(const ObjectName & objectName, Scene * scene);
	~Ground();
	virtual void load(const Variant & variant);
	virtual const ObjectClass & getClass() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
	virtual void update();

	virtual physx::PxActor * getID();
private:
	physx::PxRigidStatic * id_;
};

}

}

#endif