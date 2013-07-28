#ifndef _PhysicsODE_System_hpp_
#define _PhysicsODE_System_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace PhysicsODE
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
private:
	Scene * scene_;
};

}

}

#endif _PhysicsODE_System_hpp_