#ifndef _PhysicsPhysX_Unit_hpp_
#define _PhysicsPhysX_Unit_hpp_

#include "Object.hpp"

#include "Database/Entity.hpp"

#include "Interface/IBody.hpp"

#include <vector>

namespace Sim
{

class Variant;

namespace Interfaces
{
class IEntity;
}

using Interfaces::IPosition;
using Interfaces::IRotation;

namespace PhysicsPhysX
{

class MultiGeometryBody;

class Structure :	public Object,
					public IPosition,
					public IRotation
{
public:
	Structure(const ObjectName & objectName, Scene * scene);
	~Structure();
	virtual const ObjectClass & getClass() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);
	virtual void getPosition(IPosition::Data & data) const;
	virtual void getRotation(IRotation::Data & data) const;
	virtual void update();
	virtual void onBodyMove(const Body * body);
	void load(const Variant & variant);
	virtual physx::PxActor * getID();
private:
	size_t mainBodyIndex_;
	typedef std::vector<MultiGeometryBody*> MultiGeometryBodies;
	MultiGeometryBodies bodies_;
};

}

}

#endif _PhysicsPhysX_Unit_hpp_