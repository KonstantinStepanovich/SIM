#include "stdafx.h"

#include "Objects/BodyObject.hpp"
#include "Scene.hpp"

#include "Interface/IEntity.hpp"
#include "Interface/IBody.hpp"
#include "Interface/IMass.hpp"
#include "Interface/IDynamicBody.hpp"
#include "Interface/IAnimated.hpp"
#include "Events.hpp"

#include "Math/Box3.hpp"

#include <ode/objects.h>
#include <ode/collision.h>

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::PhysicsODE;

BodyObject::BodyObject(const ObjectName & objectName, Scene * scene) :
	Object(objectName, scene), body_(scene->getWorldID(), scene->getSpaceID(), this)
{
}

BodyObject::~BodyObject()
{
}

const ObjectClass & BodyObject::getClass() const
{
	static const ObjectClass bodyObjectClass("BodyObject");
	return bodyObjectClass;
}

Event BodyObject::getEventsOfInterest() const
{
	return Object::getEventsOfInterest() | Interfaces::EVENT_TYPE_ID_CHANGED | Interfaces::EVENT_ANIMATION_CHANGED;
}

void BodyObject::onEvent(ISubject * subject, Event event)
{
	if(event & EVENT_TYPE_ID_CHANGED)
	{
		body_.onEntityChanged(dynamic_cast<IEntity*>(subject));
	}
	if(event & EVENT_POSITION_CHANGED)
	{
		body_.onPositionChanged(dynamic_cast<IPosition*>(subject));
	}
	if(event & EVENT_ROTATION_CHANGED)
	{
		body_.onRotationChanged(dynamic_cast<IRotation*>(subject));
	}
	if(event & EVENT_DYNAMIC_BODY_CHANGED)
	{
		body_.onDynamicChanged(dynamic_cast<IDynamicBody*>(subject));
	}
	if(event & EVENT_MASS_CHANGED)
	{
		body_.onMassChanged(dynamic_cast<IMass*>(subject));
	}
	if(event & EVENT_ANIMATION_CHANGED)
	{
		body_.onAnimationChanged(dynamic_cast<IAnimated*>(subject));
		notifyEvent(EVENT_OBB_CHANGED);
	}
}

void BodyObject::getPosition(IPosition::Data & data) const
{
	body_.getPosition(scene_->getModelTime(), data);
}

void BodyObject::getRotation(IRotation::Data & data) const
{
	body_.getRotation(scene_->getModelTime(), data);
}

bool BodyObject::getArgs(Args & args) const
{
	return false;
}

Math::Box3 BodyObject::getOBB() const
{
	return body_.getOBB();
}

void BodyObject::update()
{
	body_.update();
}

void BodyObject::onBodyMove(const Body * body)
{
	if(!body_.isKinematic())
		notifyEvent(EVENT_POSITION_CHANGED | EVENT_ROTATION_CHANGED);
}