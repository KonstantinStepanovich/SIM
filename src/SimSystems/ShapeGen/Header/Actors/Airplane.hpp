#ifndef _ShapeGen_Airplane_hpp_
#define _ShapeGen_Airplane_hpp_

#include "Actors/Actor.hpp"

#include "Physic/Atmosphere/IAtmosphere.hpp"

namespace Sim
{

class Variant;

namespace SFM
{

namespace Airplane {

class Object;

}

}

namespace ShapeGen
{

class ObjectAirplane;

class Airplane : public Actor
{
public:
	Airplane(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	~Airplane();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void simulate(ModelTime dtime);
private:
	ObjectAirplane * object;
	class Activity;
	std::auto_ptr<Activity> activity_;	
	mutable AtmPoint atmPoint_;
	std::auto_ptr<SFM::Airplane::Object> airplane_;
};

}

}

#endif _ShapeGen_Airplane_hpp_