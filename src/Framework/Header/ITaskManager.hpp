#ifndef _TaskManager_hpp_
#define _TaskManager_hpp_

#include "Time.hpp"
#include "Core/ModelTime/ModelTime.hpp"

namespace Sim
{

namespace Framework
{

class ISystemTask;

class ITaskManager
{
public:
	virtual ~ITaskManager() {;}
	virtual void distributeSystemTasks(ISystemTask ** systemTask, size_t count, Time time, ModelTime modelTime) = 0;
	virtual void waitForSystemTasks(ISystemTask ** systemTask, size_t count) = 0;
};

}

}

#endif _TaskManager_hpp_