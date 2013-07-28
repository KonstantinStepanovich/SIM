#ifndef _SystemNameToReplace_System_hpp_
#define _SystemNameToReplace_System_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace SystemNameToReplace
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

#endif _SystemNameToReplace_System_hpp_