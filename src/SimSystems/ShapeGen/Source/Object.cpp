#include "stdafx.h"

#include "Scene.hpp"
#include "Object.hpp"

using Sim::ModelTime;
using namespace Sim::Framework;
using namespace Sim::ShapeGen;

Object::Object(const ObjectName & objectName, ModelTime birthTime) : ISystemObject(objectName), birthTime_(birthTime)
{
}

Object::~Object()
{
}

ISystemScene * Object::getSystemScene() const
{
	return Scene::instance();
}

Event Object::getEventsOfInterest() const
{
	return NO_EVENT;
}

void Object::onEvent(ISubject* subject, Event event)
{

}