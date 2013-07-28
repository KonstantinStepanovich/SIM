#include "stdafx.h"

#include "Objects/Ground.hpp"
#include "Scene.hpp"
#include "Utils/Materials.hpp"

#include "Interface/IEntity.hpp"

#include "Events.hpp"

#include "PhysXAPI/PxPhysics.h"
#include "PhysXAPI/PxScene.h"
#include "PhysXAPI/PxMaterial.h"
#include "PhysXAPI/PxRigidStatic.h"
#include "PhysXAPI/geometry/PxPlaneGeometry.h"
#include "PxFoundation/PxTransform.h"

using namespace Sim;
using namespace Sim::PhysicsPhysX;

using namespace physx;

Ground::Ground(const ObjectName & objectName, Scene * scene) : Object(objectName, scene),
	id_(scene_->getSDK()->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f),
														PxQuat(PxMat33(	PxVec3(0.0f, 1.0f, 0.0f),
																		PxVec3(-1.0f, 0.0f, 0.0f),
																		PxVec3(0.0f, 0.0f, 1.0f))))))
{
}

Ground::~Ground()
{
	id_->release();
}

void Ground::load(const Variant & variant)
{
}

const ObjectClass & Ground::getClass() const
{
	static ObjectClass groundClass("Ground");
	return groundClass;
}

Event Ground::getEventsOfInterest() const
{
	return Interfaces::EVENT_TYPE_ID_CHANGED;
}

void Ground::onEvent(ISubject* subject, Event event)
{
	if(event & Interfaces::EVENT_TYPE_ID_CHANGED)
	{
		Interfaces::IEntity * entity = dynamic_cast<Interfaces::IEntity*>(subject);
		if(entity->getTypeId() != "FlatTerrain")
			THROW_EXCEPTION(Exception, OSS("Unknown terrain " << entity->getTypeId().c_str()));
		id_->createShape(PxPlaneGeometry(), *mainMaterial);
		scene_->get()->addActor(*id_);
	}
}

void Ground::update()
{
}

physx::PxActor * Ground::getID()
{
	return id_;
}