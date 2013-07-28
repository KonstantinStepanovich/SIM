#ifndef _PhysicsPhysX_Utilities_hpp_
#define _PhysicsPhysX_Utilities_hpp_

#include "Math/Quat.hpp"
#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"
#include "Utils/VariantMathUtil.hpp"

#include "PhysXAPI/PxPhysics.h"
//#include "PxFoundation/PxVec3.h"
//#include "PxFoundation/PxQuat.h"

using namespace physx;

namespace Sim
{

class Variant;

namespace PhysicsPhysX
{

inline void loadVec(const Sim::Variant & variant, PxVec3 & pos)
{
	pos.x = variant.index(0u);
	pos.y = variant.index(1u);
	pos.z = variant.index(2u);
}

inline void loadQuat(const Sim::Variant & variant, PxQuat & quat)
{
	quat.x = variant.index(0u);
	quat.y = variant.index(1u);
	quat.z = variant.index(2u);
	quat.w = variant.index(3u);
}

inline bool loadQuatEx(const Sim::Variant & variant, PxQuat & pxQuat)
{
	Sim::Math::Quat quat;
	if(tryLoadQuat(variant, quat))
	{
		pxQuat = PxQuat((PxReal)quat.x(), (PxReal)quat.y(), (PxReal)quat.z(), (PxReal)quat.w());
		return true;
	}
	else
	{
		pxQuat = PxQuat::createIdentity();
		return false;
	}
}

}

}

#endif _PhysicsPhysX_Utilities_hpp_