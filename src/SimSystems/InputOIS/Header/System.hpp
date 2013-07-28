#ifndef _InputOIS_System_hpp_
#define _InputOIS_System_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace InputOIS
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

#endif _InputOIS_System_hpp_