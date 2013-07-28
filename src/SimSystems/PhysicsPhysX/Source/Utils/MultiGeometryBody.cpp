#include "stdafx.h"

#include "Utils/MultiGeometryBody.hpp"

#include "Interface/IEntity.hpp"

#include "Database/Variant.hpp"

#include "PhysXAPI/PxScene.h"
#include "PhysXAPI/PxRigidDynamic.h"

namespace physx
{
class PxShape;
}

using namespace Sim;
using namespace Sim::Interfaces;
using namespace Sim::PhysicsPhysX;

MultiGeometryBody::MultiGeometryBody(Object * object, physx::PxPhysics * SDK, physx::PxScene * scene) : body_(object, SDK)
{
	scene->addActor(*body_.get());
}

MultiGeometryBody::~MultiGeometryBody()
{
}

void MultiGeometryBody::load(const Sim::Variant & variant)
{
	body_.load(variant);
	if(const Sim::Variant vGeometry = variant.key("geometry"))
	{
		for(Sim::Variant::ConstIterator itG = vGeometry.begin();
			itG != vGeometry.end();
			++itG)
		{
			physx::PxShape * shape = NULL;
			Geometry(shape).load(body_.get(), *itG);
		}
	}
}
