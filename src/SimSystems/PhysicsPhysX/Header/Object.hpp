#ifndef _PhysicsPhysX_Object_hpp_
#define _PhysicsPhysX_Object_hpp_

#include "ISystem.hpp"

namespace physx
{
class PxActor;
}

namespace Sim
{

using namespace Framework;

namespace PhysicsPhysX
{

class Body;

class Scene;

class Object : public ISystemObject
{
public:
	Object(const ObjectName & objectName, Scene * scene);
	virtual ~Object() {;}
	virtual ISystemScene * getSystemScene() const;
	virtual Event getPotentialEvents() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
	
	virtual void onBodyMove(const Body * body);
protected:
	Scene * scene_;
};

}

}

#endif _PhysicsPhysX_Object_hpp_