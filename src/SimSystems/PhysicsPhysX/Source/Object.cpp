#include "stdafx.h"

#include "Scene.hpp"
#include "Object.hpp"

#include "Interface/IBody.hpp"
#include "Interface/IShape.hpp"
#include "Interface/IMass.hpp"
#include "Events.hpp"

using Sim::ModelTime;
using namespace Sim::Framework;
using namespace Sim::PhysicsPhysX;

Object::Object(const ObjectName & objectName, Scene * scene) : ISystemObject(objectName), scene_(scene)
{
}

ISystemScene * Object::getSystemScene() const
{
	return scene_;
}

Event Object::getPotentialEvents() const
{
	return Interfaces::EVENT_BODY_CHANGED;
}

Event Object::getEventsOfInterest() const
{
	return Interfaces::EVENT_BODY_CHANGED | Interfaces::EVENT_DYNAMIC_BODY_CHANGED | Interfaces::EVENT_MASS_CHANGED;
}

void Object::onEvent(ISubject* subject, Event event)
{

}

void Object::onBodyMove(const Body * body)
{
}