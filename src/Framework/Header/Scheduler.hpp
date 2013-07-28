#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "ISystem.hpp"

namespace Sim
{

namespace Framework
{

class ITaskManager;
class LinkingScene;

class Scheduler
{
public:
	Scheduler(ITaskManager * taskManager);
	void start();
	void execute(const LinkingScene * linkingScene);
private:
	ITaskManager*	taskManager_;
};

}

}

#endif _Scheduler_hpp_