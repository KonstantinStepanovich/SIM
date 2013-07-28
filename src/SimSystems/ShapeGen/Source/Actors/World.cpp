#include "stdafx.h"

#include "Actors/Actor.hpp"
#include "Actors/World.hpp"

#include <assert.h>

#include "Database/Variant.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

World::World()
{
}

World::~World()
{
	assert(actors_.empty());
}

World & World::instance()
{
	static World world;
	return world;
}

void World::load(ModelTime modelTime, const Variant & variant)
{
	for(Variant::ConstIterator itV = variant.begin();
		itV != variant.end();
		++itV)
	{
		Variant vActor = *itV;
		Actor * actor = ActorFactory::instance().create(vActor.key("type"), itV.key(), modelTime, vActor);
		addActor(actor);
	}
}

void World::addActor(Actor * actor)
{
	if(std::find(actors_.begin(), actors_.end(), actor) == actors_.end())
		actors_.push_back(actor);
}

void World::release()
{
	for(Actors::const_iterator itA = actors_.begin();
		itA != actors_.end();
		++itA)
		(*itA)->release();
	actors_.clear();
}