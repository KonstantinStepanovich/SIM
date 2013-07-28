#ifndef _SingleThreadedTaskManager_hpp_
#define _SingleThreadedTaskManager_hpp_

#include "ITaskManager.hpp"
#include <vector>

namespace Sim
{

namespace Framework
{

class ISystemTask;

class SingleThreadedTaskManager : public ITaskManager
{
public:
	SingleThreadedTaskManager();
	virtual void distributeSystemTasks(ISystemTask ** systemTask, size_t count, Time time, ModelTime modelTime);
	virtual void waitForSystemTasks(ISystemTask ** systemTask, size_t count);
private:
	typedef std::vector<ISystemTask*> SystemTaskQueue;
	SystemTaskQueue systemTaskQueue_;
	Time time_;
	ModelTime modelTime_;
};

}

}

#endif _SingleThreadedTaskManager_hpp_