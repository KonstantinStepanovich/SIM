#include "stdafx.h"

#include "Object.hpp"
#include "Scene.hpp"

using namespace Sim::Framework;
using namespace Sim::GraphicsOSG;

Object::Object(const ObjectName & objectName, Scene * scene) : ISystemObject(objectName), _scene(scene)
{

}

ISystemScene * Object::getSystemScene() const
{
	return _scene;
}

Event Object::getPotentialEvents() const
{
	return NO_EVENT;
}