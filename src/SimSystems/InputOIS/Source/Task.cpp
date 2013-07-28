#include "stdafx.h"

#include "Task.hpp"
#include "Scene.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::InputOIS;

Task::Task(Scene * scene) : scene_(scene)
{

}

ISystemScene * Task::getSystemScene() const
{
	return scene_;
}

void Task::operator()(Time time, ModelTime modelTime)
{
	scene_->update(time, modelTime);
}