#include "stdafx.h"

#include "Implementation/Airplane.hpp"

using namespace Sim::Interfaces;

Airplane::Airplane() :	elevatorAxis_(0.0f),
						aileronAxis_(0.0f),
						rudderAxis_(0.0f),
						afterburner_(false),
						throttleAxis_(0.0f),
						flaps_(0.0f),
						gear_(0.0f)
{
}

float Airplane::getElevatorAxisValue() const
{
	return elevatorAxis_;
}

float Airplane::getAileronAxisValue() const
{
	return aileronAxis_;
}

float Airplane::getRudderAxisValue() const
{
	return rudderAxis_;
}

bool Airplane::isAfterburnerEngaged() const
{
	return afterburner_;
}

float Airplane::getThrottleAxisValue() const
{
	return throttleAxis_;
}

float Airplane::getFlaps() const
{
	return flaps_;
}

bool Airplane::getGear() const
{
	return gear_;
}