#ifndef _PhysicsPhysX_Task_hpp_
#define _PhysicsPhysX_Task_hpp_

#include "ISystem.hpp"

#include "Core/ModelTime/ActivityScheduler.hpp"

namespace Sim
{

using namespace Framework;

namespace PhysicsPhysX
{

class Scene;

class Task : public ISystemTask
{
public:
	Task(Scene * scene);
	~Task();
	virtual ISystemScene * getSystemScene() const;
	virtual void operator()(Time time, ModelTime modelTime);
private:
	Scene * scene_;
	ModelTime stepSize_;
	Sim::ActivityScheduler scheduler_;
	class Activity;
	std::auto_ptr<Activity> activity_;
};

}

}

#endif _PhysicsPhysX_Task_hpp_