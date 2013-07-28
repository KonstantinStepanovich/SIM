#ifndef _PhysicsODE_Object_hpp_
#define _PhysicsODE_Object_hpp_

#include "ISystem.hpp"

#include <ode/common.h>

namespace Sim
{

using namespace Framework;

namespace PhysicsODE
{

class Scene;
class Body;

class Object :	public ISystemObject
{
public:
	Object(const ObjectName & objectName, Scene * scene);
	virtual ~Object();
	virtual ISystemScene * getSystemScene() const;
	virtual Event getPotentialEvents() const;
	virtual Event getEventsOfInterest() const;
	virtual void onBodyMove(const Body * body);
	virtual void update() = 0;
protected:
	Scene * scene_;
};

}

}

#endif _PhysicsODE_Object_hpp_