#include "stdafx.h"

#include "Task.hpp"
#include "Scene.hpp"

#include "Core/ModelTime/Activity.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::PhysicsPhysX;

namespace Sim
{

namespace PhysicsPhysX
{

class Task::Activity : public Sim::PeriodicActivity
{
public:
	Activity(Scene * scene, ModelTime dtime) : scene_(scene), PeriodicActivity(dtime)
	{
	}
	virtual ModelTime operator()(ModelTime time)
	{
		scene_->simulate(time, getDTime());
		return PeriodicActivity::operator()(time);
	}
private:
	Scene * scene_;
};

}

}

Task::Task(Scene * scene) : scene_(scene), stepSize_(0.01)
{
	activity_.reset(new Activity(scene_, stepSize_));
	scheduler_.scheduleNow(activity_.get());
}

Task::~Task()
{
	scheduler_.remove(activity_.get());
}

ISystemScene * Task::getSystemScene() const
{
	return scene_;
}

void Task::operator()(Time time, ModelTime modelTime)
{
	scheduler_.run(modelTime);
}