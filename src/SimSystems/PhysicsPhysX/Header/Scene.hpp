#ifndef _PhysicsPhysX_Scene_hpp_
#define _PhysicsPhysX_Scene_hpp_

#include "ISystem.hpp"

namespace physx
{
class PxPhysics;
class PxScene;
}

namespace Sim
{

using namespace Framework;

namespace PhysicsPhysX
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

	void simulate(ModelTime modelTime, ModelTime dtime);
	ModelTime getModelTime() const { return modelTime_; }

	physx::PxPhysics * getSDK();
	physx::PxScene* get() { return scene_; }
private:
	System*				system_;
	std::auto_ptr<Task>	task_;
	physx::PxScene*		scene_;
	ModelTime			modelTime_;
};

}

}

#endif _PhysicsPhysX_Scene_hpp_