#ifndef _SystemNameToReplace_Scene_hpp_
#define _SystemNameToReplace_Scene_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace SystemNameToReplace
{

class System;
class Task;

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

	virtual void onEvent(ISubject* subject, Event event);
	virtual Event getEventsOfInterest() const;
	virtual Event getPotentialEvents() const;
private:
	System*				system_;
	std::auto_ptr<Task>	task_;
};

}

}

#endif _SystemNameToReplace_Scene_hpp_