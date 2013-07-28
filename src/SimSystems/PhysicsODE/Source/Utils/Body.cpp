#include "stdafx.h"

#include "Utils/Body.hpp"
#include "Utils/Utilities.hpp"
#include "Object.hpp"

#include "Interface/IEntity.hpp"
#include "Interface/IBody.hpp"
#include "Interface/IMass.hpp"
#include "Interface/IDynamicBody.hpp"
#include "Events.hpp"

#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"
#include "Debug/CheckValue.hpp"
#include "Debug/CheckMath.hpp"
#include "Algorithm/Exception.hpp"

#include <limits>
#include <ode/objects.h>
#include <ode/mass.h>

#include <map>

using namespace Sim;
using namespace Sim::PhysicsODE;

void moveCallback(dBodyID bodyID)
{
	Body * body = (Body*)(dBodyGetData(bodyID));
	body->onMove();
}

std::string Body::path = "Common/Body/";

Body::Body(dWorldID worldID, dSpaceID spaceID, Object * object) :
	id_(dBodyCreate(worldID)),
	force_(Math::Vec3(0.0, 0.0, 0.0)),
	torque_(Math::Vec3(0.0, 0.0, 0.0)),
	prevVelocity_(0.0f, 0.0f, 0.0f),
	prevVelocityTime_(0.0),
	acceleration_(0.0f, 0.0f, 0.0f),
	geometry_(spaceID, id_),
	object_(object)
{
	dBodySetKinematic(id_);
	dBodySetData(id_, this);
	dBodySetMovedCallback(id_, &moveCallback);
	dBodySetLinearDamping(id_, 0);
	dBodySetAngularDamping(id_, 0);
	dBodySetMaxAngularSpeed(id_, dInfinity);
	dBodySetGyroscopicMode(id_, 0);
}

Body::~Body()
{
	dBodySetData(id_, 0);
	dBodyDestroy(id_);
}

void Body::getPosition(ModelTime time, IPosition::Data & data) const
{
	assert(id_ != NULL);
	data.time = time;
	const dReal * pos = dBodyGetPosition(id_);
	validate(pos, 3);
	data.position.setValue(pos[0], pos[1], pos[2]);
	const dReal * vel = dBodyGetLinearVel(id_);
	validate(vel, 3);
	data.velocity.setValue(vel[0], vel[1], vel[2]);
	assert(data.velocity.length() < 1.0E5);
	if(time > prevVelocityTime_)
	{
		acceleration_ = (data.velocity - prevVelocity_) / (time - prevVelocityTime_);
		Math::Vec3 acc = acceleration_;
		prevVelocity_ = data.velocity;
		prevVelocityTime_ = time;
	}
	data.acceleration = acceleration_;
}

void Body::getRotation(ModelTime time, IRotation::Data & data) const
{
	assert(id_ != NULL);
	data.time = time;
	const dReal * quat = dBodyGetQuaternion(id_);
	validate(quat, 4);
	data.rotation.setValue(quat[1], quat[2], quat[3], quat[0]);
	const dReal * angularVel = dBodyGetAngularVel(id_);
	validate(angularVel, 3);
	data.angularVelocity.setValue(angularVel[0], angularVel[1], angularVel[2]);
	assert(data.angularVelocity.length() < 1.0E5);
}

const Math::Box3 & Body::getOBB() const
{
	return geometry_.getOBB();
}

void Body::onEntityChanged(IEntity * entity)
{
	//const TypeId & typeId = entity->getTypeId();
	//VariantRoot variant;
	//loadLua((path + typeId + ".lua").c_str(), "body", variant);
	//load(variant);
	geometry_.onEntityChanged(entity);
}

void Body::onPositionChanged(IPosition * position)
{
	assert(id_ != NULL);
	IPosition::Data data;
	position->getPosition(data);
	validate(data.position);
	validate(data.velocity);
	dBodySetPosition(id_, data.position.x(), data.position.y(), data.position.z());
	dBodySetLinearVel(id_, data.velocity.x(), data.velocity.y(), data.velocity.z());
}

void Body::onRotationChanged(IRotation * rotation)
{
	assert(id_ != NULL);
	IRotation::Data data;
	rotation->getRotation(data);
	validate(data.rotation);
	validate(data.angularAcceleration);
	dQuaternion quat = { data.rotation.w(), data.rotation.x(), data.rotation.y(), data.rotation.z() };
	dBodySetQuaternion(id_, quat);
	dBodySetAngularVel(id_, data.angularVelocity.x(), data.angularVelocity.y(), data.angularVelocity.z());
}

void Body::onMassChanged(IMass * mass)
{
	assert(id_ != NULL);
	IMass::Data data;
	if(mass->getMass(data))
	{
		if(dBodyIsKinematic(id_))
		{
			dBodySetDynamic(id_);
		}

		dMass dmass;
		dBodyGetMass(id_, &dmass);

		dmass.mass = data.mass;
		
		dmass.c[0] = data.centerOfMass.x();
		dmass.c[1] = data.centerOfMass.y();
		dmass.c[2] = data.centerOfMass.z();

		for(int i = 0; i < 3; i++)
		{
			const Math::Vec3 & row = data.inertiaTensor.getRow(i);
			dmass.I[4 * i + 0] = row.x();
			dmass.I[4 * i + 1] = row.y();
			dmass.I[4 * i + 2] = row.z();
		}

		dBodySetMass(id_, &dmass);
		
		geometry_.setDynamic(true);
	}
	else
	{
		if(!dBodyIsKinematic(id_))
			dBodySetKinematic(id_);

		geometry_.setDynamic(false);
	}
}

void Body::onDynamicChanged(IDynamicBody * dynamicBody)
{
	assert(id_ != NULL);
	force_ = dynamicBody->getForce();
	validate(force_);

	torque_ = dynamicBody->getTorque();
	validate(torque_);
}

void Body::onAnimationChanged(IAnimated * animated)
{
	geometry_.onAnimationChanged(animated);
}

void Body::update()
{
	dBodyAddRelForce(id_, force_.x(), force_.y(), force_.z());
	dBodyAddRelTorque(id_, torque_.x(), torque_.y(), torque_.z());
}

void Body::onMove()
{
	if(object_ != NULL)
		object_->onBodyMove(this);
}

bool Body::isKinematic() const
{
	assert(id_ != NULL);
	return dBodyIsKinematic(id_) != 0;
}