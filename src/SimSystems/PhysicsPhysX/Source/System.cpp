#include "stdafx.h"

#include "System.hpp"
#include "Scene.hpp"
#include "Utils/Materials.hpp"

#include "Algorithm/Exception.hpp"

#include "PhysXAPI/PxPhysicsAPI.h"
#include "PhysXAPI/extensions/PxExtensionsAPI.h"

extern "C"
{

__declspec(dllexport) Sim::Framework::ISystem * createSystem(const char * systemName, const Managers * managers)
{
	if(strcmp(systemName, "PhysicsPhysX") == 0)
		return new Sim::PhysicsPhysX::System();
	else
		return NULL;
}

__declspec(dllexport) void destroySystem(Sim::Framework::ISystem * system)
{
	delete system;
}

}

using namespace physx;

using namespace Sim::Framework;
using namespace Sim::PhysicsPhysX;

PxMaterial * Sim::PhysicsPhysX::mainMaterial;

System::System() : scene_(NULL)
{
	bool recordMemoryAllocations = true;

	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	SDK_ = PxCreatePhysics(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback, PxTolerancesScale(), recordMemoryAllocations );
	if(SDK_ == NULL)
		THROW_EXCEPTION(Exception, "PxCreatePhysics failed!");

	if(!PxInitExtensions(*SDK_))
		THROW_EXCEPTION(Exception, "PxInitExtensions failed!");

	mainMaterial = SDK_->createMaterial(0.03f, 0.03f, 0.1f); 
}

System::~System()
{
	SDK_->release();
	SDK_ = NULL;
}

const SystemName &System:: getName() const
{
	static SystemName systemName("PhysicsPhysX");
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