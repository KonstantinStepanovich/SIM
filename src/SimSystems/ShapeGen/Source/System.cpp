#include "stdafx.h"

#include "System.hpp"
#include "Scene.hpp"

#include <ode/odeinit.h>

#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"
#include "Utils/Units.hpp"

extern "C"
{

__declspec(dllexport) Sim::Framework::ISystem * createSystem(const char * systemName, const Managers * managers)
{
	if(strcmp(systemName, "ShapeGen") == 0)
		return new Sim::ShapeGen::System();
	else
		return NULL;
}

__declspec(dllexport) void destroySystem(Sim::Framework::ISystem * system)
{
	delete system;
}

}


using namespace Sim::Framework;
using namespace Sim::ShapeGen;

System::System() : scene_(NULL)
{
	dInitODE();
	{
		Sim::VariantRoot variant;
		Sim::loadLua("Common/Config/Units.lua", "Units", variant);
		Sim::UnitManager::instance().load(variant);
	}
}

System::~System()
{
	dCloseODE();
}

const SystemName &System:: getName() const
{
	static SystemName systemName("ShapeGen");
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