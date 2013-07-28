#include "stdafx.h"

#include "Implementation/Body.hpp"

#include "Debug/CheckMath.hpp"

using namespace Sim;
using namespace Sim::Interfaces;

Body::Body(ModelTime time,
		   const Math::Vec3 & position, const Math::Vec3 & velocity,
		   const Math::Quat & rotation, const Math::Vec3 & angularVelocity) :
	time_(time), position_(position), velocity_(velocity), rotation_(rotation), angularVelocity_(angularVelocity)
{
}

void Body::set(ModelTime time,
			   const Math::Vec3 & position, const Math::Vec3 & velocity,
			   const Math::Quat & rotation, const Math::Vec3 & angularVelocity)
{
	time_ = time;
	position_ = position;
	velocity_ = velocity;
	rotation_ = rotation;
	angularVelocity_ = angularVelocity;
}

void Body::getPosition(IPosition::Data & data) const
{
	data.time = 0.0;
	data.position = position_;
	validate(data.position);
	data.velocity = velocity_;
	data.acceleration = Math::Vec3(0.0f, 0.0f, 0.0f);
}

const Math::Vec3 & Body::getForce() const
{
	return force_;
}

void Body::getRotation(IRotation::Data & data) const
{
	data.time = 0.0;
	data.rotation = rotation_;
	validate(data.rotation);
	data.angularVelocity = angularVelocity_;
	data.angularAcceleration = Math::Vec3(0.0f, 0.0f, 0.0f);
}

const Math::Vec3 & Body::getTorque() const
{
	return torque_;
}