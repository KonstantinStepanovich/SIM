#include "stdafx.h"

#include "System.hpp"
#include "Scene.hpp"

namespace Sim {
namespace InputOIS {
Managers managers;
}
}

extern "C"
{

__declspec(dllexport) Sim::Framework::ISystem * createSystem(const char * systemName, const Managers * managersIn)
{
	if(strcmp(systemName, "InputOIS") == 0)
	{
		Sim::InputOIS::managers = *managersIn;
		return new Sim::InputOIS::System();
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
using namespace Sim::InputOIS;

System::System() : scene_(NULL)
{

}

System::~System()
{

}

const SystemName &System:: getName() const
{
	static SystemName systemName("InputOIS");
	return systemName;
}

ISystemScene * System::createScene()
{
	return scene_ = new Scene(this);
}

void System::destroyScene()
{
	delete scene_;
}