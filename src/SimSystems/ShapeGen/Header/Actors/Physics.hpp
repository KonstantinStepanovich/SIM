#ifndef _ShapeGen_Physics_hpp_
#define _ShapeGen_Physics_hpp_

#include "Actors/Actor.hpp"

#include "Math/Vec3.hpp"

namespace Sim
{

class Variant;

namespace ShapeGen
{

class Physics : public Actor
{
	Physics();
public:
	static Physics & instance();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void load(const Variant & variant);
	virtual void release();
	Math::Vec3 gravity;
};

}

}

#endif _ShapeGen_Physics_hpp_