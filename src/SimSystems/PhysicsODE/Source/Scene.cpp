#include "stdafx.h"

#include "Scene.hpp"
#include "System.hpp"
#include "Task.hpp"
#include "Objects/BodyObject.hpp"

#include <algorithm>

#include "ode/objects.h"
#include "ode/collision_space.h"
#include "ode/collision.h"

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"
#include "Database/ParserLua.hpp"

using namespace Sim;
using namespace Sim::PhysicsODE;
using namespace Sim::Framework;

Scene::Scene(System * system) : modelTime_(0.0), system_(system)
{
	worldID_ = dWorldCreate();
	dWorldSetGravity(worldID_, 0.0, -9.81, 0.0);
	dWorldSetMaxAngularSpeed(worldID_, 10.0);

	spaceID_ = dSimpleSpaceCreate(0);
	dGeomSetData((dGeomID)spaceID_, (void*)true);
	dSpaceSetCleanup(spaceID_, 1);

	task_.reset(new Task(this));
}

Scene::~Scene()
{
	assert(objects_.empty());

	dSpaceDestroy(spaceID_);
	dWorldDestroy(worldID_);
}

void Scene::load(const char * scenery)
{
	VariantRoot variant;
	loadLua(scenery, "scene", variant);
	//Body::path = variant.key("bodyPath").getString();
	if(const Variant vPhysics = variant.key("Physics"))
	{
		//Geometry::path = vPhysics.key("geometryPath").getString();
		if(const Variant vGravity = vPhysics.tryKey("gravity"))
		{
			dReal vec[3];
			::load(vGravity, vec, 3);		
			dWorldSetGravity(worldID_, vec[0], vec[1], vec[2]);
		}
	}
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
	if(	objectClass == "Body" ||
		objectClass == "Airplane")
		return addObject_(new BodyObject(objectName, this));
	else
		return NULL;
}

void Scene::destroyObject(ISystemObject * systemObject)
{
	objects_.remove(static_cast<Object*>(systemObject));
	delete systemObject;
}

Event Scene::getPotentialEvents() const
{
	return NO_EVENT;
}

Event Scene::getEventsOfInterest() const
{
	return NO_EVENT;
}

void Scene::onEvent(ISubject * subject, Event event)
{

}

void Scene::update(ModelTime modelTime)
{
	modelTime_ = modelTime;
	std::for_each(objects_.begin(), objects_.end(), std::mem_fun(&Object::update));
}

Object * Scene::addObject_(Object * object)
{
	objects_.push_back(object);
	return object;
}