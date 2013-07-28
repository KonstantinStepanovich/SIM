#include "stdafx.h"

#include "Scene.hpp"
#include "System.hpp"
#include "Task.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::SystemNameToReplace;

Scene::Scene(System * system) : system_(system)
{
}

Scene::~Scene()
{
}

void Scene::load(const char * scene)
{

}

ISystem * Scene::getSystem()
{
	return system_;
}

ISystemTask * Scene::getSystemTask()
{
	return task_.get();
}

ISystemObject * Scene::createObject(const ObjectName & objectName, const ObjectClass & objectClass)
{
	return NULL;
}

void Scene::destroyObject(ISystemObject * systemObject)
{

}

void Scene::onEvent(ISubject* subject, Event event)
{

}

Event Scene::getEventsOfInterest() const
{
	return NO_EVENT;
}

Event Scene::getPotentialEvents() const
{
	return NO_EVENT;
}
