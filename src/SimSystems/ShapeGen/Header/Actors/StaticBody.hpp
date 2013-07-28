#ifndef _StaticBody_hpp_
#define _StaticBody_hpp_

#include "Actors/Actor.hpp"

#include "Core/ModelTime/ModelTime.hpp"

namespace Sim
{

class Variant;

using namespace Sim::Framework;

namespace ShapeGen
{

class BodyObject;

class StaticBody : public Actor
{
public:
	StaticBody(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	~StaticBody();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
private:
	BodyObject * bodyObject_;
};

}

}

#endif _StaticBody_hpp_