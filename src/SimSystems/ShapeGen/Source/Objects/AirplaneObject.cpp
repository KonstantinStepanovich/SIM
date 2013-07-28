#include "stdafx.h"

#include "Objects/AirplaneObject.hpp"
#include "Actors/StandardAtmosphere.hpp"

#include "Scene.hpp"

#include "Events.hpp"
#include "Interface/IAirplane.hpp"

#include "Modules/SFM/Object/AirplaneObject.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::ShapeGen;

ObjectAirplane::ObjectAirplane(const std::string & objectName, ModelTime birthTime, const TypeId & typeId, 
							   const Math::Vec3 & position, const Math::Vec3 & velocity, const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
							   SFM::Airplane::Object & airplane, const Math::Vec3 & forceCoeffs, const Math::Vec3 & torqueCoeffs) :
	Object(objectName, birthTime), Body(birthTime, position, velocity, rotation, angularVelocity), Entity(typeId),
	forceCoeffs_(forceCoeffs), torqueCoeffs_(torqueCoeffs), airplane_(airplane)
{
	for(IAnimated::ArgIndex argIndex = 0; argIndex < 5; ++argIndex)
		addArg(argIndex, 0.0);
}

const ObjectClass & ObjectAirplane::getClass() const
{
	static const ObjectClass airplaneClass("Airplane");
	return airplaneClass;
}

ObjectAirplane::~ObjectAirplane()
{
}

void ObjectAirplane::onEvent(ISubject * subject, Event event)
{
	if(event & Interfaces::EVENT_POSITION_CHANGED)
	{
		onPositionChanged_(dynamic_cast<Interfaces::IPosition*>(subject));	
	}
	if(event & Interfaces::EVENT_ROTATION_CHANGED)
	{
		onRotationChanged_(dynamic_cast<Interfaces::IRotation*>(subject));
	}
	if(event & Interfaces::EVENT_AIRPLANE_CHANGED)
	{
		onAirplaneChanged_(dynamic_cast<Interfaces::IAirplane*>(subject));
	}
}

Event ObjectAirplane::getEventsOfInterest() const
{
	return Interfaces::EVENT_BODY_CHANGED | Interfaces::EVENT_AIRPLANE_CHANGED;
}

Event ObjectAirplane::getPotentialEvents() const
{
	return	Interfaces::EVENT_BODY_CHANGED |
			Interfaces::EVENT_MASS_CHANGED |
			Interfaces::EVENT_DYNAMIC_BODY_CHANGED |
			Interfaces::EVENT_TYPE_ID_CHANGED |
			Interfaces::EVENT_DEBUG_OBJECT_CHANGED;
}

const Math::Vec3 & ObjectAirplane::getForce() const
{
	force_ = airplane_.getForce();
	for(int i = 0; i < 3; i++)
		force_[i] *= forceCoeffs_[i];
	return force_;
}

const Math::Vec3 & ObjectAirplane::getTorque() const
{
	torque_ = airplane_.getTorque();
	for(int i = 0; i < 3; i++)
		torque_[i] *= torqueCoeffs_[i];
	return torque_;
}

bool ObjectAirplane::hasMass() const
{
	return true;
}

bool ObjectAirplane::getMass(IMass::Data & data) const
{
	data.mass = airplane_.getMass();
	data.centerOfMass = Math::Vec3(0.0, 0.0, 0.0); //airplane_.getCenterOfMass();
	data.inertiaTensor = airplane_.getInertiaTensor();
	return true;
}

const char * ObjectAirplane::getString() const
{
	return debugState_.c_str();
}

void ObjectAirplane::setDebugState(const std::string & str)
{
	debugState_ = str;
}

void ObjectAirplane::update()
{
	clearArgs();
	addArg(0, airplane_.getAirleron());
	addArg(1, airplane_.getElevator());
	addArg(2, airplane_.getRudder());
	addArg(3, airplane_.getFlaps());
	addArg(4, airplane_.getGear());
	notifyEvent(Interfaces::EVENT_DYNAMIC_BODY_CHANGED | Interfaces::EVENT_MASS_CHANGED |
				Interfaces::EVENT_DEBUG_OBJECT_CHANGED | Interfaces::EVENT_ANIMATION_CHANGED);
}

void ObjectAirplane::onPositionChanged_(Interfaces::IPosition * position)
{
	Interfaces::IPosition::Data data;
	position->getPosition(data);
	position_ = data.position;
	velocity_ = data.velocity;
}

void ObjectAirplane::onRotationChanged_(Interfaces::IRotation * rotation)
{
	Interfaces::IRotation::Data data;
	rotation->getRotation(data);
	rotation_ = data.rotation;
	angularVelocity_ = data.angularVelocity;
}

void ObjectAirplane::onAirplaneChanged_(Interfaces::IAirplane * airplane)
{
	airplane_.setAileron(airplane->getAileronAxisValue());
	airplane_.setRudder(airplane->getRudderAxisValue());
	airplane_.setElevator(airplane->getElevatorAxisValue());
	if(airplane->isAfterburnerEngaged())
	{
		if(SFM::JetEngine::Database::instance().getDesc(airplane_.getDesc().jetEngineId).hasAfterburner)
			airplane_.setAfterburnerThrust(airplane->getThrottleAxisValue());
		else
			airplane_.setThrust(1.0);
	}
	else
		airplane_.setThrust(airplane->getThrottleAxisValue());
	airplane_.setFlaps(airplane->getFlaps());
	airplane_.setGear(airplane->getGear());
}