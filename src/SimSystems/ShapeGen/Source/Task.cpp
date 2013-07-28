#include "stdafx.h"

#include "Task.hpp"
#include "Scene.hpp"

#include "Math/Vec3.hpp"
#include "String/StringStream.hpp"

#include <sstream>
#include <math.h>

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::ShapeGen;

Task::Task(Scene * scene) : scene_(scene)
{

}

ISystemScene * Task::getSystemScene() const
{
	return scene_;
}

void Task::operator()(Time time, ModelTime modelTime)
{
	scene_->update(modelTime);
}