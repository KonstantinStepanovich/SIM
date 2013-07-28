#ifndef _SystemNameToReplace_Task_hpp_
#define _SystemNameToReplace_Task_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace SystemNameToReplace
{

class Scene;

class Task : public ISystemTask
{
public:
	Task(Scene * scene);
	virtual ISystemScene * getSystemScene() const;
	virtual void operator()(Time time, ModelTime modelTime);
private:
	Scene * scene_;
};

}

}

#endif _SystemNameToReplace_Task_hpp_