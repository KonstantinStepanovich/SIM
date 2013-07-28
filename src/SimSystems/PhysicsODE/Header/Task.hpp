#ifndef _PhysicsODE_Task_hpp_
#define _PhysicsODE_Task_hpp_

#include "ISystem.hpp"
#include "Core/ModelTime/ActivityScheduler.hpp"
#include <ode/common.h>

namespace Sim
{

using namespace Framework;

namespace PhysicsODE
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
	dJointGroupID contactJointGroupID_;
	Sim::ActivityScheduler scheduler_;
	class Activity;
	std::auto_ptr<Activity> activity_;
};

}

}

#endif _PhysicsODE_Task_hpp_