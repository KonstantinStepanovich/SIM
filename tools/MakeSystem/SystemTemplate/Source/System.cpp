#include "stdafx.h"

#include "System.hpp"
#include "Scene.hpp"

extern "C"
{

__declspec(dllexport) Sim::Framework::ISystem * createSystem(const char * systemName, const Managers * managers)
{
	if(strcmp(systemName, "SystemNameToReplace") == 0)
		return new Sim::SystemNameToReplace::System();
	else
		return NULL;
}

__declspec(dllexport) void destroySystem(Sim::Framework::ISystem * system)
{
	delete system;
}

}


using namespace Sim::Framework;
using namespace Sim::SystemNameToReplace;

System::System() : scene_(NULL)
{

}

System::~System()
{

}

const SystemName &System:: getName() const
{
	static SystemName systemName("SystemNameToReplace");
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