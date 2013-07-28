#ifndef _ShapeGen_World_hpp_
#define _ShapeGen_World_hpp_

#include "Actors/Actor.hpp"

namespace Sim
{

class Variant;

namespace ShapeGen
{

class World : public Actor
{
public:
	static World & instance();
	void load(ModelTime modelTime, const Variant & variant);
	void addActor(Actor * actor);
	void release();
private:
	World();
	~World();
	typedef std::list<Actor*> Actors;
	Actors actors_;
};

}

}

#endif _ShapeGen_World_hpp_