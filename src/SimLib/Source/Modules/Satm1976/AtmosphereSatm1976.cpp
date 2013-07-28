#include "Modules/Satm1976/AtmosphereSatm1976.hpp"

#include <math.h>
#include <algorithm>

#include "Physic/Gas.hpp"

#include "Modules/Satm1976/Satm1976.hpp"

using namespace Sim;

static const float T0 = Satm1976::T0;
static const float P0 = Satm1976::P0;
static const float rho0 = Satm1976::rho0;

AtmosphereSatm1976::AtmosphereSatm1976() : baseWind_(0.0f, 0.0f, 0.0f)
{

}

AtmosphereSatm1976::AtmosphereSatm1976(const Math::Vec3 & baseWind) : baseWind_(baseWind)
{

}

AtmosphereSatm1976::~AtmosphereSatm1976()
{

}

void AtmosphereSatm1976::setPoint(const Math::Vec3 & point, AtmPoint & atmPoint) const
{
	atmPoint.point = point;
	atmPoint.data =  (void*)Satm1976::getLevelByAltKm(point.getY() * 0.001f);
}

void AtmosphereSatm1976::updatePoint(const Math::Vec3 & point, AtmPoint & atmPoint) const
{
	atmPoint.point = point;
	atmPoint.data =  (const void*)Satm1976::updateLevelByAltKm((unsigned int)atmPoint.data, point.getY() * 0.001f);
}

void AtmosphereSatm1976::getParam(const AtmPoint & atmPoint, unsigned int flags, AtmParam & atmParam) const
{
	atmParam.flags = flags;

	if((flags & AtmParam::PARAM_ALL) == 0)
		return;
	
	unsigned int level = (unsigned int)atmPoint.data;
	float altKm = atmPoint.point.getY() * 0.001f;
	Satm1976::AtmParam satm1976AtmParam;
	getAtmParamByAltKm(altKm, level, satm1976AtmParam);
	atmParam.T = satm1976AtmParam.Tr * T0;
	atmParam.P = satm1976AtmParam.Pr * P0;
	atmParam.rho = satm1976AtmParam.rhor * rho0;
	atmParam.Vs = getSoundVel(atmParam.T);
}

Math::Vec3 AtmosphereSatm1976::getWind(const AtmPoint & atmPoint) const
{
	return baseWind_;
}