#ifndef _AtmosphereSatm1976_hpp_
#define _AtmosphereSatm1976_hpp_

#include "Physic/Atmosphere/IAtmosphere.hpp"

namespace Sim
{

//USA Standart athmosphere 1976
class AtmosphereSatm1976 : public IAtmosphere
{
public:
	AtmosphereSatm1976();
	AtmosphereSatm1976(const Math::Vec3 & baseWind);
	~AtmosphereSatm1976();
	void setBaseWind(const Math::Vec3 & baseWind) { baseWind_ = baseWind; }
	virtual void setPoint(const Math::Vec3 & point, AtmPoint & atmPoint) const;
	virtual void updatePoint(const Math::Vec3 & point, AtmPoint & atmPoint) const;
	virtual void getParam(const AtmPoint & atmPoint, unsigned int flags, AtmParam & atmParam) const;
	virtual Math::Vec3 getWind(const AtmPoint & atmPoint) const;
private:
	Math::Vec3 baseWind_;
};

}

#endif _AtmosphereSatm1976_hpp_