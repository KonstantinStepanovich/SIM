#ifndef _StandardAtmosphere_hpp_
#define _StandardAtmosphere_hpp_

#include "Actors/Actor.hpp"

#include "Core/ModelTime/ModelTime.hpp"

#include "Modules/Satm1976/AtmosphereSatm1976.hpp"

namespace Sim
{

class Variant;

namespace ShapeGen
{

class StandardAtmosphere : public Actor
{
	StandardAtmosphere();
	virtual void release();
public:
	static StandardAtmosphere & instance();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void load(const Variant & variant);
	const AtmosphereSatm1976 & get() const { return impl_; }
private:
	AtmosphereSatm1976 impl_;
};

}

}

#endif _StandardAtmosphere_hpp_