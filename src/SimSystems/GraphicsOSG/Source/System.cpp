#include "stdafx.h"

#include "System.hpp"
#include "Scene.hpp"

#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>

namespace Sim {
namespace GraphicsOSG {
Managers managers;
}
}

extern "C"
{

__declspec(dllexport) Sim::Framework::ISystem * createSystem(const char * systemName, const Managers * managersIn)
{
	if(strcmp(systemName, "GraphicsOSG") == 0)
	{
		Sim::GraphicsOSG::managers = *managersIn;
		return new Sim::GraphicsOSG::System();
	}
	else
		return NULL;
}

__declspec(dllexport) void destroySystem(Sim::Framework::ISystem * system)
{
	delete system;
}

}

using namespace Sim::Framework;
using namespace Sim::GraphicsOSG;

System::System() : _viewer(new osgViewer::Viewer()), _scene(NULL)
{
	_viewer->setUpViewInWindow(50, 50, 640, 480);
	_viewer->setReleaseContextAtEndOfFrameHint(false);
}

System::~System()
{
}

const SystemName & System::getName() const
{
	static const SystemName systemName("GraphicsOSG");
	return systemName;
}

ISystemScene * System::createScene()
{
	return _scene = new Scene(_viewer, this);
}

void System::destroyScene()
{
	delete _scene;
}
