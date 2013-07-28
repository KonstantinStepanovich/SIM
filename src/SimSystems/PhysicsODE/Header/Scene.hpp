#ifndef _Scene_hpp_
#define _Scene_hpp_

#include "ISystem.hpp"

#include <ode/common.h>

namespace Sim
{

using namespace Framework;

namespace PhysicsODE
{

class System;
class Task;
class Object;

class Scene : public ISystemScene
{
public:
	Scene(System * system);
	~Scene();
	virtual void load(const char * scene);

	virtual ISystem * getSystem();
	virtual ISystemTask * getSystemTask();

	virtual ISystemObject * createObject(const ObjectName & objectName, const ObjectClass & objectClass);
	virtual void destroyObject(ISystemObject * systemObject);

	virtual Event getPotentialEvents() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);

	inline dWorldID getWorldID() const { return worldID_; }
	inline dSpaceID getSpaceID() { return spaceID_; }
	inline ModelTime getModelTime() const { return modelTime_; }
	void update(ModelTime modelTime);

private:

	Object * addObject_(Object * object);

	System *			system_;
	std::auto_ptr<Task>	task_;

	typedef std::list<Object*> Objects;
	Objects		objects_;

	dWorldID	worldID_;
	dSpaceID	spaceID_;
	ModelTime	modelTime_;
};

}

}

#endif _Scene_hpp_