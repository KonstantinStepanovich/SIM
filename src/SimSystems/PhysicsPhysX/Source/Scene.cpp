#include "stdafx.h"

#include "Scene.hpp"
#include "System.hpp"
#include "Task.hpp"
#include "Objects/Ground.hpp"
#include "Objects/Shape.hpp"
#include "Objects/Structure.hpp"
#include "Utils/Body.hpp"

#include "Algorithm/Exception.hpp"

#include "PhysXAPI/PxPhysicsAPI.h"
#include "PhysXAPI/extensions/PxExtensionsAPI.h"

using namespace physx;

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::PhysicsPhysX;

Scene::Scene(System * system) : system_(system), modelTime_(0.0)
{
	PxSceneDesc sceneDesc(system->getSDK()->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	if(sceneDesc.cpuDispatcher == NULL)
	{
		const PxU32 nbThreads = 1;
		pxtask::CpuDispatcher * cpuDispatcher = PxDefaultCpuDispatcherCreate(nbThreads);
		if(cpuDispatcher == NULL)
			THROW_EXCEPTION(Exception, "PxDefaultCpuDispatcherCreate failed!");
		sceneDesc.cpuDispatcher = cpuDispatcher;
	}
	//static PxDefaultSimulationFilterShader gDefaultFilterShader;
	if(!sceneDesc.filterShader)
		sceneDesc.filterShader  = &PxDefaultSimulationFilterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
	
	scene_ = system_->getSDK()->createScene(sceneDesc);

	task_.reset(new Task(this));
}

Scene::~Scene()
{
	scene_->release();
	scene_ = NULL;
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
	if(objectClass == "Ground")
	{
		Ground * ground = new Ground("Ground", this);
		return ground;
	}
	else if(objectClass == "Shape")
	{
		Shape * shape = new Shape(objectName, this);
		return shape;
	}
	else if(objectClass == "Airplane")
	{
		Structure * structure = new Structure(objectName, this);
		return structure;
	}
	else
	{
		return NULL;
	}
	return NULL;
}

void Scene::destroyObject(ISystemObject * systemObject)
{
	delete systemObject;
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

physx::PxPhysics * Scene::getSDK()
{
	return system_->getSDK();
}

void Scene::simulate(ModelTime modelTime, ModelTime dtime)
{
	const PxU32 bufferSize = 64; 
	PxActor * actors[bufferSize];
	PxU32 actorCount = scene_->getActors(PxActorTypeSelectionFlag::eRIGID_DYNAMIC, actors, bufferSize);
	for(PxActor ** actor = actors;
		actor != actors + actorCount;
		++actor)
	{
		Body* body = static_cast<Body*>((*actor)->userData);
		body->update();
	}
	scene_->simulate(dtime);
	scene_->fetchResults(true);

	PxU32 activeTransformCount;
	const PxActiveTransform * activeTransforms = scene_->getActiveTransforms(activeTransformCount);
	for(const PxActiveTransform * activeTransform = activeTransforms;
		activeTransform != activeTransforms + activeTransformCount;
		++activeTransform)
	{
		Body* body = static_cast<Body*>(activeTransform->actor->userData);
		body->onMove(modelTime, dtime);
	}
	modelTime_ = modelTime;
}