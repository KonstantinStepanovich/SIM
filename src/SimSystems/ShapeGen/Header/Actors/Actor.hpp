#ifndef _ShapeGen_Actor_hpp_
#define _ShapeGen_Actor_hpp_

#include "ISystem.hpp"

#include "Core/ModelTime/ActivityScheduler.hpp"
#include "Core/ModelTime/ModelTime.hpp"

#include <string>
#include <map>

namespace Sim
{

using namespace Framework;

class Variant;

namespace ShapeGen
{

extern ActivityScheduler activityScheduler;

class Object;
class Scene;

class Actor
{
public:
	Actor();
	Actor(const Variant & variant);
	virtual ~Actor();
	virtual void release();
	inline void setName(const std::string & nameIn) { name = nameIn; }
	inline const std::string & getName() const { return name; }
protected:
	std::string name;
};

class ActorFactory
{
	ActorFactory();
public:
	typedef Actor* (*Create)(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void reg(const std::string & type, Create create);
	static ActorFactory & instance();
	Actor * create(const std::string & type, const std::string & nameIn, ModelTime modelTime, const Variant & variant);
private:
	typedef std::map<std::string, Create> CreateMap;
	CreateMap createMap_;
};

class RegisterActor
{
public:
	RegisterActor(const std::string & type, ActorFactory::Create create, void(*initFunc)() = NULL);
};

#define REGISER_ACTOR(ActorType) static RegisterActor reg##ActorType(#ActorType, &ActorType::create)

#define REGISER_ACTOR2(ActorType, func) static RegisterActor reg##ActorType(#ActorType, &ActorType::create, func)

}

}

#endif _ShapeGen_Actor_hpp_