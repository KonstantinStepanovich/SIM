#include "stdafx.h"

#include "Scene.hpp"
#include "Object.hpp"

#include "Interface/IBody.hpp"
#include "Interface/IMass.hpp"
#include "Events.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::PhysicsODE;

Object::Object(const ObjectName & objectName, Scene * scene) : ISystemObject(objectName), scene_(scene)
{
}

Object::~Object()
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

void Object::onBodyMove(const Body * body)
{

}