#ifndef _PhysicsODE_BodyObject_hpp_
#define _PhysicsODE_BodyObject_hpp_

#include "Object.hpp"
#include "Utils/Body.hpp"
#include "Interface/IAnimated.hpp"

namespace Sim
{

namespace Math
{
class Box3;
}

namespace Interfaces
{
class IPosition;
class IRotation;
class IMass;
}

using Interfaces::IPosition; 
using Interfaces::IRotation;
using Interfaces::IMass;

namespace PhysicsODE
{

class BodyObject :	public Object,
					public IPosition,
					public IRotation,
					public IAnimated
{
public:
	BodyObject(const ObjectName & objectName, Scene * scene);
	~BodyObject();
	virtual const ObjectClass & getClass() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);
	virtual void getPosition(IPosition::Data & data) const;
	virtual void getRotation(IRotation::Data & data) const;
	virtual bool getArgs(Args & args) const;
	virtual Math::Box3 getOBB() const;
	virtual void update();
	virtual void onBodyMove(const Body * body);
private:
	Body body_;
};

}

}

#endif _PhysicsODE_BodyObject_hpp_