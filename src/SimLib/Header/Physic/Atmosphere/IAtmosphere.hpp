#ifndef _IATMOSPHERE_HPP_
#define _IATMOSPHERE_HPP_

#include "Math/Vec3.hpp"

namespace Sim
{

struct AtmPoint
{
	Math::Vec3	point;
	const void* data;
};

struct AtmParam
{
	enum Flag
	{
		NO_PARAMS = 0,
		PARAM_TEMPERATURE = 1 << 0,
		PARAM_PRESSURE = 1 << 1,
		PARAM_RHO = 1 << 2,
		PARAM_HUMIDITY = 1 << 3,
		PARAM_SOUND_VEL = 1 << 4,
		PARAM_ALL = PARAM_TEMPERATURE | PARAM_PRESSURE | PARAM_RHO | PARAM_HUMIDITY | PARAM_SOUND_VEL,
	};
	typedef unsigned int Flags;
	Flags	flags;
	float	T; //temperature, K
	float	P; //pressure, Pa
	float	rho; //density, kg/m^3
	float	H; //humidity, ???
	float	Vs; //sound velocity, m/s
};

class IAtmosphere
{
public:
	virtual ~IAtmosphere() {;}
	virtual void setPoint(const Math::Vec3 & point, AtmPoint & atmPoint) const = 0;
	virtual void updatePoint(const Math::Vec3 & point, AtmPoint & atmPoint) const = 0;
	virtual void getParam(const AtmPoint & atmPoint, unsigned int flags, AtmParam & atmParam) const = 0;
	virtual Math::Vec3 getWind(const AtmPoint & atmPoint) const = 0;
};

}

#endif _IATMOSPHERE_HPP_