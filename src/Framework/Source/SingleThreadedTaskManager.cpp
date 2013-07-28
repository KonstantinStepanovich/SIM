#include "stdafx.h"
#include "SingleThreadedTaskManager.hpp"
#include "ISystem.hpp"

using namespace Sim;
using namespace Sim::Framework;

SingleThreadedTaskManager::SingleThreadedTaskManager() : time_(0), modelTime_(0.0)
{
}

void SingleThreadedTaskManager::distributeSystemTasks(ISystemTask ** systemTask, size_t count, Time time, ModelTime modelTime)
{
	if(count > systemTaskQueue_.capacity())
		systemTaskQueue_.reserve(count);
	systemTaskQueue_.resize(count);
	std::copy(systemTask, systemTask + count, systemTaskQueue_.begin());
	time_ = time;
	modelTime_ = modelTime;
}

void SingleThreadedTaskManager::waitForSystemTasks(ISystemTask ** systemTask, size_t count)
{
	for(SystemTaskQueue::iterator itST = systemTaskQueue_.begin();
		itST != systemTaskQueue_.end();
		itST++)
	{
		ISystemTask* systemTask = *itST;
		(*systemTask)(time_, modelTime_);
	}
}