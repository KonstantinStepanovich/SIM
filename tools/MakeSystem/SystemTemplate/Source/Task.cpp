#include "stdafx.h"

#include "Task.hpp"
#include "Scene.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::SystemNameToReplace;

Task::Task(Scene * scene) : scene_(scene)
{

}

ISystemScene * Task::getSystemScene() const
{
	return scene_;
}

void Task::operator()(Time time, ModelTime modelTime)
{

}