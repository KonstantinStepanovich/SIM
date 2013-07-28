#include "stdafx.h"
#include "Scheduler.hpp"
#include "ModelTimer.hpp"
#include "EnvironmentManager.hpp"
#include "PlatformManager.hpp"
#include "ITaskManager.hpp"
#include "Linking.hpp"

using namespace Sim::Framework;

Scheduler::Scheduler(ITaskManager * taskManager) : taskManager_(taskManager)
{
	
}

void Scheduler::start()
{
	ModelTimer::instance().start(PlatformManager::instance().getSystemTime());
}

void Scheduler::execute(const LinkingScene * linkingScene)
{
	static const size_t systemTaskMaxCount = 5;
	ISystemTask * systemTasks[systemTaskMaxCount];
	size_t taskCount = 0;
	const LinkingScene::SystemScenes & systemScenes = linkingScene->getSystemScenes();
	for(LinkingScene::SystemScenes::const_iterator itSS = systemScenes.begin();
		itSS != systemScenes.end() && taskCount < systemTaskMaxCount;
		itSS++)
	{
		ISystemTask * systemTask = itSS->second->getSystemTask();
		if(systemTask != NULL)
			systemTasks[taskCount++] = systemTask;
	}
	ModelTimer::instance().pause(EnvironmentManager::instance().isPause());
	if(EnvironmentManager::instance().getTimeAccelleration() > 1)
		ModelTimer::instance().accelerate(EnvironmentManager::instance().getTimeAccelleration());
	else if(EnvironmentManager::instance().getTimeDecelleration() > 1)
		ModelTimer::instance().deccelerate(EnvironmentManager::instance().getTimeDecelleration());
	else
		ModelTimer::instance().normalize();
	
	if(taskCount > 0)
	{
		Time time = PlatformManager::instance().getSystemTime();
		ModelTime modelTime = ModelTimer::instance().execute(time);
		taskManager_->distributeSystemTasks(&systemTasks[0], taskCount, time, modelTime);
		taskManager_->waitForSystemTasks(&systemTasks[0], taskCount);
	}
};