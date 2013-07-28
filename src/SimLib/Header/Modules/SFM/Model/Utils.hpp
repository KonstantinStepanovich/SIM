#ifndef _SFM_Utils_hpp_
#define _SFM_Utils_hpp_

#include "Math/Vec3.hpp"

namespace Sim
{

namespace SFM
{

inline double getAoA(const Math::Vec3 & TASloc)
{
	double tas = TASloc.length();
	if(tas > 1.0E-10)
	{
		Math::Vec3 velNorm = TASloc / tas;
		return atan2(-velNorm.dot(Math::Vec3(0.0, 1.0, 0.0)), velNorm.dot(Math::Vec3(1.0, 0.0, 0.0)));
	}
	else
		return 0.0;
}

inline double getAoS(const Math::Vec3 & TASloc)
{
	double tas = TASloc.length();
	if(tas > 1.0E-10)
	{
		Math::Vec3 velNorm = TASloc / tas;
		double sinAoS = velNorm.dot(Math::Vec3(0.0f, 0.0f, 1.0f));
		return asin(sinAoS);
	}
	else
		return 0.0;
}

}

}

#endif _SFM_Utils_hpp_