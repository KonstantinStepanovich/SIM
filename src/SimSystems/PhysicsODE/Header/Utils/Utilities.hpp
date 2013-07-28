#ifndef _PhysicsODE_Utilities_hpp_
#define _PhysicsODE_Utilities_hpp_

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"

#include <ode/objects.h>
#include <ode/rotation.h>

namespace Sim
{

class Variant;

namespace PhysicsODE
{

inline void loadVec(const Sim::Variant & variant, dVector3 pos)
{
	load<dReal, 3>(variant, pos);
}

inline void loadQuat(const Sim::Variant & variant, dQuaternion quat)
{
	for(int i = 0; i < 3; i++)
		quat[i] = variant.index(i);
	quat[0] = variant.index(3);
}

inline bool loadQuatEx(const Sim::Variant & variant, dQuaternion quat)
{
	if(Sim::Variant vQuat = variant.tryKey("quat"))
		loadQuat(vQuat, quat);
	else if(Sim::Variant vRot = variant.tryKey("rot"))
	{
		dVector3 axis;
		loadFromTable(vRot, "axis", axis, &loadVec);
		dReal angle = vRot.key("angle");
		dQFromAxisAndAngle(quat, axis[0], axis[1], axis[2], angle);
	}
	else if(Sim::Variant vEuler = variant.tryKey("euler"))
	{
		return false;
	}
	else
	{
		return false;
	}
	return true;
}


inline void loadMatrix(const Sim::Variant & variant, dMatrix3 mat)
{
	for(int i = 0; i < 3; i++)
	{
		const Sim::Variant row = variant.index(i);
		for(int j = 0; j < 3; j++)
			mat[i * 4 + j] = row.index(j);
	}
}

}

}

#endif _PhysicsODE_Utilities_hpp_