#include "stdafx.h"

#include "Scene.hpp"
#include "System.hpp"
#include "Task.hpp"
#include "Object.hpp"
#include "Objects/CameraObject.hpp"
#include "Actors/World.hpp"
#include "Actors/Camera.hpp"

#include "String/StringStream.hpp"
#include "DataBase/Variant.hpp"
#include "DataBase/ParserLua.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::ShapeGen;

const ModelTime dtime = 0.1;

Scene * Scene::instance_ = NULL;

Scene::Scene(System * system) : system_(system)
{
	task_.reset(new Task(this));
	assert(instance_ == NULL);
	instance_ = this;
}

Scene::~Scene()
{
	World::instance().release();
	notifyEvent(EVENT_DESTROY_OBJECT);
}

Scene * Scene::instance()
{
	return instance_;
}

void Scene::load(const char * scene)
{
	VariantRoot variant;
	Sim::loadLua(scene, "scene", variant);

	World::instance().load(0.0, variant);
	World::instance().addActor(&Camera::instance());	
	
	if(!createObjectsData_.empty())
		notifyEvent(EVENT_CREATE_OBJECT);
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
	return EVENT_GENERIC;
}

ISystemObject * Scene::extend(const ObjectName & objectName, const ObjectClass & objectClass, void * data)
{
	return (ISystemObject*)data;
}

void Scene::unextend(ISystemObject * systemObject)
{
}

const CreateObjectsData & Scene::getCreateObjectsData()
{
	return createObjectsData_;
}

const DestroyObjectsData & Scene::getDestroyObjectsData()
{
	return destroyObjectsData_;
}

void Scene::clearGeneric_()
{
	createObjectsData_.clear();
	destroyObjectsData_.clear();
}

void Scene::update(ModelTime modelTime)
{
	clearGeneric_();

	activityScheduler.run(modelTime);

	if(!createObjectsData_.empty())
		notifyEvent(EVENT_CREATE_OBJECT);

	if(!destroyObjectsData_.empty())
		notifyEvent(EVENT_DESTROY_OBJECT);
}

void Scene::addObject(Object * object)
{
	addObject(object->getName(), object->getClass(), object);
}

void Scene::addObject(const ObjectName & objectName, const ObjectClass & objectClass, Object * object)
{
	createObjectsData_.push_back(CreateObjectData(objectName, objectClass, object));
}

void Scene::removeObject(Object * object)
{
	DestroyObjectData destroyObjectData(object->getName(), NULL);
	destroyObjectsData_.push_back(destroyObjectData);
}