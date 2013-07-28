#include "stdafx.h"

#include "System.hpp"
#include "Scene.hpp"

#include <ode/odeinit.h>

extern "C"
{

__declspec(dllexport) Sim::Framework::ISystem * createSystem(const char * systemName, const Managers * managers)
{
	if(strcmp(systemName, "PhysicsODE") == 0)
		return new Sim::PhysicsODE::System();
	else
		return NULL;
}

__declspec(dllexport) void destroySystem(Sim::Framework::ISystem * system)
{
	delete system;
}

}

using namespace Sim::Framework;
using namespace Sim::PhysicsODE;

System::System() : scene_(NULL)
{
	dInitODE();
}

System::~System()
{
	dCloseODE();
}

const SystemName &System:: getName() const
{
	static SystemName systemName("PhysicsODE");
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