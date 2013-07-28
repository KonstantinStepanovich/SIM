#include "stdafx.h"

#include "Actors/MovingBodyStorage.hpp"

#include "Actors/World.hpp"
#include "Objects/BodyObject.hpp"
#include "Scene.hpp"

#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

REGISER_ACTOR(MovingBodyStorage);

MovingBodyStorage::Activity::Activity(MovingBodyStorage * movingBodyStorage, ModelTime dtime) : movingBodyStorage_(movingBodyStorage), PeriodicActivity(dtime)
{	
}

ModelTime MovingBodyStorage::Activity::operator()(ModelTime modelTime)
{
	movingBodyStorage_->updateBodies(modelTime);	
	return PeriodicActivity::operator ()(modelTime);
}

MovingBodyStorage::Body::Body(BodyObject * bodyObject) : bodyObject_(bodyObject), idle_(false)
{
}

bool MovingBodyStorage::Body::updateStatus(const MovingBodyStorage::Parameters & parameters)
{
	bool startIdle = true;
	ModelTime time = 0.0;
	{
		IPosition::Data positionData;
		bodyObject_->getPosition(positionData);
		if(!parameters.worldSize.isInside(positionData.position))
		{
			return false;
		}
		if(positionData.velocity.length() > parameters.minVelocity)
			startIdle = false;
		time = positionData.time;
	}
	if(startIdle)
	{
		IRotation::Data rotationData;
		bodyObject_->getRotation(rotationData);
		if(rotationData.angularVelocity.length() > parameters.minAngularVelocity)
			startIdle = false;
		time = std::max(time, rotationData.time);
	}
	if(startIdle)
	{
		if(!idle_)
		{
			idleStartTime_ = time;
			idle_ = true;
		}
		if(time - idleStartTime_ > parameters.idlePeriod)
		{
			return false;
		}
	}
	return true;
}

MovingBodyStorage::MovingBodyStorage() : activity_(this, 0.5)
{
	parameters_.minVelocity = 1.0E-3;
	parameters_.minAngularVelocity = 1.0E-3;
	parameters_.idlePeriod = 2.0;
	activityScheduler.scheduleNow(&activity_);
	World::instance().addActor(this);
}

void MovingBodyStorage::release()
{
	activityScheduler.remove(&activity_);

	for(MovingBodies::iterator itMB = movingBodies_.begin();
		itMB != movingBodies_.end();
		++itMB)
	{
		Scene::instance()->removeObject(itMB->getBodyObject());
		delete itMB->getBodyObject();
	}
}

MovingBodyStorage & MovingBodyStorage::instance()
{
	static MovingBodyStorage movingBodyStorage;
	return movingBodyStorage;
}

Actor * MovingBodyStorage::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	MovingBodyStorage::instance().load(variant);
	return &MovingBodyStorage::instance();
}

void MovingBodyStorage::load(const Variant & variant)
{
	::load(variant.key("worldSize"), parameters_.worldSize);
}

void MovingBodyStorage::remove(BodyObject * bodyObject)
{

}

void MovingBodyStorage::updateBodies(ModelTime time)
{
	MovingBodies::iterator itMB = movingBodies_.begin();
	while(itMB != movingBodies_.end())
	{
		if(!itMB->updateStatus(parameters_))
		{
			Scene::instance()->removeObject(itMB->getBodyObject());
			itMB = movingBodies_.erase(itMB);
		}
		else
			++itMB;
	}
}