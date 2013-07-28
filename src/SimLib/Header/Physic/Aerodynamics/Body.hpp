#ifndef _IBODY_H_
#define _IBODY_H_

#include "Math/Vec3.hpp"

namespace Aerodynamics
{

class IBody
{
public:
	virtual ~IBody() {;}
	virtual void blow(	const Vec3 & flowLocal, float density, float M,
						Vec3 & forceLocal, Vec3 & torqueLocal) const = 0; // Дунуть, прям так дунуть ...
};

}

#endif _IBODY_H_