#include "stdafx.h"

#include "Actors/Actor.hpp"

#include "Database/Variant.hpp"

namespace Sim
{

namespace ShapeGen
{

ActivityScheduler activityScheduler;

Actor::Actor()
{
}

Actor::Actor(const Variant & variant)// : name(variant.key("name"))
{	
}

Actor::~Actor()
{
}

void Actor::release()
{
	delete this;
}

ActorFactory::ActorFactory()
{
}

ActorFactory & ActorFactory::instance()
{
	static ActorFactory actorFactory;
	return actorFactory;
}

void ActorFactory::reg(const std::string & type, ActorFactory::Create create)
{
	createMap_[type] = create;
}

Actor * ActorFactory::create(const std::string & type, const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	CreateMap::iterator itCM = createMap_.find(type);
	if(itCM == createMap_.end())
		THROW_EXCEPTION(Exception, OSS("Unknown actor type \"" << type << "\""));
	Actor  * actor = (*itCM->second)(nameIn, modelTime, variant);
	return actor;
}

RegisterActor::RegisterActor(const std::string & type, ActorFactory::Create create, void(*initFunc)())
{
	ActorFactory::instance().reg(type, create);	
	if(initFunc != NULL)
		(*initFunc)();
}

}

}