#ifndef _PhysicsPhysX_System_hpp_
#define _PhysicsPhysX_System_hpp_

#include "ISystem.hpp"

namespace physx
{
class PxPhysics;
}

namespace Sim
{

using namespace Framework;

namespace PhysicsPhysX
{

class Scene;

class System : public ISystem
{
public:
	System();
	~System();
	virtual const SystemName & getName() const;
	virtual ISystemScene * createScene();
	virtual void destroyScene();
	physx::PxPhysics * getSDK() { return SDK_; }
private:
	Scene * scene_;
	physx::PxPhysics * SDK_;
};

}

}

#endif _PhysicsPhysX_System_hpp_