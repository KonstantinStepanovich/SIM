#include "stdafx.h"

#include "Objects/BodyObject.hpp"

#include "Events.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::ShapeGen;

BodyObject::BodyObject(	const ObjectName & objectName, const TypeId & typeId,
						const Math::Vec3 & position, const Math::Vec3 & velocity,
						const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
						const IMass::Data & mass,
						ModelTime birthTime) :
							Object(objectName, birthTime),
							Entity(typeId), Mass(mass),
							Interfaces::Body(	birthTime, position, velocity,
												rotation, angularVelocity)
{
}

BodyObject::~BodyObject()
{
}

const ObjectClass & BodyObject::getClass() const
{
	static ObjectClass objectClass("Body");
	return objectClass;
}

Math::Box3 BodyObject::getOBB() const
{
	return obb;
}

Event BodyObject::getPotentialEvents() const
{
	return EVENT_BODY_CHANGED | EVENT_MASS_CHANGED | EVENT_TYPE_ID_CHANGED | EVENT_ANIMATION_CHANGED;
}

Event BodyObject::getEventsOfInterest() const
{
	return EVENT_BODY_CHANGED | EVENT_OBB_CHANGED;
}

void BodyObject::onEvent(ISubject * subject, Event event)
{
	if(event & EVENT_POSITION_CHANGED)
	{
		IPosition * position = dynamic_cast<IPosition*>(subject);		
		Interfaces::IPosition::Data data;
		position->getPosition(data);
		Interfaces::Body::position_ = data.position;
		Interfaces::Body::velocity_ = data.velocity;
	}
	if(event & EVENT_OBB_CHANGED)
	{
		IAnimated * animated = dynamic_cast<IAnimated*>(subject);
		obb = animated->getOBB();
	}
}