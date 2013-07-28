#include "stdafx.h"

#include "Scene.hpp"
#include "Objects/Airplane.hpp"
#include "Implementation/Commands.hpp"

#include "Core/ModelTime/Activity.hpp"

#include "Events.hpp"

using Sim::ModelTime;
using namespace Sim::Framework;
using namespace Sim::InputOIS;

static const ModelTime dtime = 0.02;

class Airplane::Activity : public PeriodicActivity
{
public:
	Activity(Airplane * airplane, ModelTime dtime) : PeriodicActivity(dtime), airplane_(airplane) {;}
	virtual ModelTime operator()(ModelTime time)
	{
		airplane_->update();
		return PeriodicActivity::operator()(time);
	}
private:
	Airplane * airplane_;
};

Airplane::Airplane(const ObjectName & objectName, Scene * scene) : Object(objectName, scene), scene_(scene),
	aileronKeyAxis_(2.0f, 0.3, -1.0f, 1.0f), elevatorKeyAxis_(2.0f, 0.3, -1.0f, 1.0f),
	rudderKeyAxis_(2.0f, 0.3, -1.0f, 1.0f), throttleKeyAxis_(2.0f, 0.3, 0.0f, 1.0f)
{
	activity_.reset(new Activity(this, dtime));
	scene_->addActivity(0.0, activity_.get());
}

Airplane::~Airplane()
{
	scene_->removeActivity(activity_.get());
}

ISystemScene * Airplane::getSystemScene() const
{
	return scene_;
}

Event Airplane::getEventsOfInterest() const
{
	return NO_EVENT;
}

Event Airplane::getPotentialEvents() const
{
	return Interfaces::EVENT_AIRPLANE_CHANGED;
}

const ObjectClass & Airplane::getClass() const
{
	static const ObjectClass airplaneClass("Airplane");
	return airplaneClass;
}

void Airplane::onEvent(ISubject* subject, Event event)
{

}

void Airplane::onCommand(Command command)
{
	switch(command)
	{
		case COMMAND_AIRPLANE_AILERONS_LEFT:
			aileronKeyAxis_.setKeyDecrease(true);
			break;
		case COMMAND_AIRPLANE_AILERONS_LEFT_RELEASE:
			aileronKeyAxis_.setKeyDecrease(false);
			break;
		case COMMAND_AIRPLANE_AILERONS_RIGHT:
			aileronKeyAxis_.setKeyIncrease(true);
			break;
		case COMMAND_AIRPLANE_AILERONS_RIGHT_RELEASE:
			aileronKeyAxis_.setKeyIncrease(false);
			break;
		case COMMAND_AIRPLANE_RUDDER_LEFT:
			rudderKeyAxis_.setKeyDecrease(true);
			break;
		case COMMAND_AIRPLANE_RUDDER_LEFT_RELEASE:
			rudderKeyAxis_.setKeyDecrease(false);
			break;
		case COMMAND_AIRPLANE_RUDDER_RIGHT:
			rudderKeyAxis_.setKeyIncrease(true);
			break;
		case COMMAND_AIRPLANE_RUDDER_RIGHT_RELEASE:
			rudderKeyAxis_.setKeyIncrease(false);
			break;
		case COMMAND_AIRPLANE_ELEVATOR_UP:
			elevatorKeyAxis_.setKeyDecrease(true);
			break;
		case COMMAND_AIRPLANE_ELEVATOR_UP_RELEASE:
			elevatorKeyAxis_.setKeyDecrease(false);
			break;
		case COMMAND_AIRPLANE_ELEVATOR_DOWN:
			elevatorKeyAxis_.setKeyIncrease(true);
			break;
		case COMMAND_AIRPLANE_ELEVATOR_DOWN_RELEASE:
			elevatorKeyAxis_.setKeyIncrease(false);
			break;
		case COMMAND_AIRPLANE_THROTTLE_INCREASE:
			throttleKeyAxis_.setKeyIncrease(true);
			break;
		case COMMAND_AIRPLANE_THROTTLE_INCREASE_RELEASE:
			throttleKeyAxis_.setKeyIncrease(false);
			break;
		case COMMAND_AIRPLANE_THROTTLE_DECREASE:
			throttleKeyAxis_.setKeyDecrease(true);
			break;
		case COMMAND_AIRPLANE_THROTTLE_DECREASE_RELEASE:
			throttleKeyAxis_.setKeyDecrease(false);
			break;
		case COMMAND_AIRPLANE_FLAPS:
			flaps_ = flaps_ > 0.5f ? 0.0f : 1.0f;
			notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
			break;
		case COMMAND_AIRPLANE_GEARS:
			gear_ = !gear_;
			notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
			break;
	}
}

void Airplane::onAxisCommand(const AxisCommand & axisCommand)
{
	switch(axisCommand.command)
	{
		case COMMAND_AIRPLANE_AILERONS:
			aileronAxis_ = axisCommand.value;
			notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
			break;
		case COMMAND_AIRPLANE_RUDDER:
			rudderAxis_ = axisCommand.value;
			notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
			break;
		case COMMAND_AIRPLANE_ELEVATOR:
			elevatorAxis_ = axisCommand.value;
			notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
			break;
		case COMMAND_AIRPLANE_THROTTLE:
			float axisValue = 1.0f - 0.5f * (axisCommand.value + 1.0f);
			afterburner_ = axisValue > 0.75f;
			throttleAxis_ = afterburner_ ? axisValue : axisValue / 0.75f;
			notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
			break;
	}
}

void Airplane::update()
{
	//aileronAxis_ = aileronKeyAxis_.simulate(dtime);
	//elevatorAxis_ = elevatorKeyAxis_.simulate(dtime);
	//rudderAxis_ = rudderKeyAxis_.simulate(dtime);
	//throttleAxis_ = throttleKeyAxis_.simulate(dtime);
	//if(	aileronKeyAxis_.isMoving() ||
	//	elevatorKeyAxis_.isMoving() ||
	//	rudderKeyAxis_.isMoving() ||
	//	throttleKeyAxis_.isMoving())
	//	notifyEvent(Interfaces::EVENT_AIRPLANE_CHANGED);
}