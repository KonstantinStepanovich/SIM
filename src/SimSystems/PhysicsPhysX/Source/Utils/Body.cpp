#include "stdafx.h"

#include "Utils/Body.hpp"
#include "Utils/Utilities.hpp"
#include "Object.hpp"

#include "Interface/IBody.hpp"
#include "Interface/IMass.hpp"
#include "Interface/IDynamicBody.hpp"
#include "Events.hpp"

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"
#include "Utils/VariantMathUtil.hpp"
#include "Debug/CheckValue.hpp"
#include "Debug/CheckMath.hpp"
#include "Algorithm/Exception.hpp"

#include <limits>

#include "PhysXAPI/PxPhysics.h"
#include "PhysXAPI/PxRigidDynamic.h"
#include "PxFoundation/PxTransform.h"

using namespace Sim;
using namespace Sim::PhysicsPhysX;

using namespace physx;

Body::Body(Object * object, physx::PxPhysics * SDK) :
	object_(object),
	id_(SDK->createRigidDynamic(PxTransform::createIdentity())),
	force_(Math::Vec3(0.0, 0.0, 0.0)),
	torque_(Math::Vec3(0.0, 0.0, 0.0))
{
	id_->setLinearDamping(0.0);
	id_->setAngularDamping(0.0);
	id_->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
	id_->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
	id_->wakeUp(0.4);
	id_->userData = this;
}

Body::~Body()
{
	if(id_ != NULL)
	{
		id_->release();
		id_ = NULL;
	}
}

void Body::load(const Sim::Variant & variant)
{
	assert(id_ != NULL);
	if(Variant vMass = variant.key("mass"))
	{
		const PxReal mass = vMass.key("mass");
		Math::Vec3 centerOfMass;
		loadFromTable(vMass, "center", centerOfMass);
		Math::Matrix3x3 inertiaTensor;
		loadFromTable(vMass, "inertiaTensor", inertiaTensor);

		id_->setMass(mass);
		id_->setCMassLocalPose(PxTransform(PxVec3(centerOfMass.x(), centerOfMass.y(), centerOfMass.z())));
		id_->setMassSpaceInertiaTensor(PxVec3(inertiaTensor[0].x(), inertiaTensor[1].y(), inertiaTensor[2].z()));
		id_->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);
	}
	else
		id_->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
}

void Body::getPosition(ModelTime time, IPosition::Data & data) const
{
	assert(id_ != NULL);
	data.time = time;
	const PxTransform pose = id_->getGlobalPose();
	data.position.setValue(pose.p.x, pose.p.y, pose.p.z);
	const PxVec3 velocity = id_->getLinearVelocity();
	data.velocity.setValue(velocity.x, velocity.y, velocity.z);
	data.acceleration = Math::Vec3(0.0f, 0.0f, 0.0f);
}

void Body::getRotation(ModelTime time, IRotation::Data & data) const
{
	assert(id_ != NULL);
	data.time = time;
	const PxTransform pose = id_->getGlobalPose();
	data.rotation.setValue(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
	const PxVec3 angularVelocity = id_->getAngularVelocity();
	data.angularVelocity.setValue(angularVelocity.x, angularVelocity.y, angularVelocity.z);
	data.angularAcceleration = Math::Vec3(0.0f, 0.0f, 0.0f);
}

void Body::onMove(ModelTime modelTime, ModelTime dtime)
{
	if(object_ != NULL)
		object_->onBodyMove(this);
}

void Body::onPositionChanged(IPosition * position)
{
	assert(id_ != NULL);
	IPosition::Data data;
	position->getPosition(data);
	validate(data.position);
	validate(data.velocity);
	const PxTransform pose(	PxVec3(data.position.x(), data.position.y(), data.position.z()),
							id_->getGlobalPose().q);
	id_->setGlobalPose(pose);
	id_->setLinearVelocity(PxVec3(data.velocity.x(), data.velocity.y(), data.velocity.z()));
}

void Body::onRotationChanged(IRotation * rotation)
{
	assert(id_ != NULL);
	IRotation::Data data;
	rotation->getRotation(data);
	validate(data.rotation);
	validate(data.angularAcceleration);
	const PxTransform pose(	id_->getGlobalPose().p,
							PxQuat(data.rotation.x(), data.rotation.y(), data.rotation.z(), data.rotation.w()));
	id_->setGlobalPose(pose);
	id_->setAngularVelocity(PxVec3(data.angularVelocity.x(), data.angularVelocity.y(), data.angularVelocity.z()));
}

void Body::onMassChanged(IMass * mass)
{
	assert(id_ != NULL);
	IMass::Data data;
	if(mass->getMass(data))
	{
		if(id_->getRigidDynamicFlags() & PxRigidDynamicFlag::eKINEMATIC)
			id_->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, false);

		id_->setMass(data.mass);
		id_->setCMassLocalPose(PxTransform(PxVec3(data.centerOfMass.x(), data.centerOfMass.y(), data.centerOfMass.z())));
		id_->setMassSpaceInertiaTensor(PxVec3(data.inertiaTensor[0].x(), data.inertiaTensor[1].y(), data.inertiaTensor[2].z()));
	}
	else
	{
		if(!(id_->getRigidDynamicFlags() & PxRigidDynamicFlag::eKINEMATIC))
			id_->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);
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

void Body::update()
{
	if(!(id_->getRigidDynamicFlags() & PxRigidDynamicFlag::eKINEMATIC))
	{
		const PxTransform pose = id_->getGlobalPose();
		const PxVec3 forceLoc = pose.q.rotate(PxVec3(force_.x(), force_.y(), force_.z()));
		const PxVec3 torqueLoc = pose.q.rotate(PxVec3(torque_.x(), torque_.y(), torque_.z()));
		id_->addForce(forceLoc);
		id_->addTorque(torqueLoc);
	}
}

bool Body::isKinematic() const
{
	assert(id_ != NULL);
	return id_->isRigidStatic() != NULL;
}
