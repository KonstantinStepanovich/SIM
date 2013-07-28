#ifndef _PhysicsPhysX_Shape_hpp_
#define _PhysicsPhysX_Shape_hpp_

#include "Object.hpp"

#include "Utils/Body.hpp"
#include "Utils/Geometry.hpp"

namespace physx
{
class PxShape;
}

namespace Sim
{

namespace Interfaces
{
class IPosition;
class IRotation;
class IMass;
class IShape;
}

using Interfaces::IPosition; 
using Interfaces::IRotation;
using Interfaces::IMass;
using Interfaces::IShape;

namespace PhysicsPhysX
{

class Shape :	public Object,
				public IPosition,
				public IRotation
{
public:
	Shape(const ObjectName & objectName, Scene * scene);
	~Shape();
	virtual const ObjectClass & getClass() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);
	virtual void getPosition(IPosition::Data & data) const;
	virtual void getRotation(IRotation::Data & data) const;
	void onMassChanged(IMass * mass);
	void onShapeChanged(IShape * shape);
	virtual void update();
	virtual void onBodyMove(const Body * body);
	physx::PxActor * getActor();
private:
	Body body_;
	physx::PxShape * shape_;
};

}

}

#endif _PhysicsPhysX_Shape_hpp_