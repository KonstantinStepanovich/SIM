#include "stdafx.h"

#include "Task.hpp"
#include "Scene.hpp"

#include "IEnvironmentManager.hpp"

#include <osgViewer/Viewer>

using namespace Sim::Framework;
using namespace Sim::GraphicsOSG;

namespace Sim{
namespace GraphicsOSG {
extern Managers managers;
}
}

Task::Task(osgViewer::Viewer* viewer, Scene * scene) : _viewer(viewer), scene_(scene)
{
	
}

ISystemScene * Task::getSystemScene() const
{
	return scene_;
}

void Task::operator()(Time time, Sim::ModelTime modelTime)
{
	_viewer->frame(modelTime);
	if(_viewer->done())
		managers.environmentManager->setFinish();
}