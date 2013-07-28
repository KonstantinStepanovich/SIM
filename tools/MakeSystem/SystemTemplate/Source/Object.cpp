#include "stdafx.h"

#include "Scene.hpp"
#include "Object.hpp"

using Sim::ModelTime;
using namespace Sim::Framework;
using namespace Sim::SystemNameToReplace;

Object::Object(const ObjectName & objectName, Scene * scene) : ISystemObject(objectName), scene_(scene)
{

}

ISystemScene * Object::getSystemScene() const
{
	return scene_;
}

Event Object::getEventsOfInterest() const
{
	return NO_EVENT;
}

void Object::onEvent(ISubject* subject, Event event)
{

}