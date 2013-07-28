#include "stdafx.h"

#include "Framework.hpp"
#include "EnvironmentManager.hpp"
#include "PlatformManager.hpp"
#include "Scheduler.hpp"
#include "SingleThreadedTaskManager.hpp"
#include "SyncChangeController.hpp"
#include "Linking.hpp"

extern "C"
{
#include "FrameworkAPI.hpp"
};

#include "String/StringStream.hpp"
#include "Algorithm/Exception.hpp"

void executeScenery(const SystemData * systemData,
					size_t systemCount,
					const char * sceneName)
{
	try
	{
		Sim::Framework::Framework framework;
		framework.initialize(systemData, systemCount, sceneName);
		framework.execute();
		framework.release();
	}
	catch (Sim::Exception & e)
	{
		printf("Exception: %s\nthrown from %s\n", e.what(), e.where());
		MessageBoxA(NULL, OSS(e.what() << "\nthrown from " << e.where()), "Exception", MB_OK | MB_TASKMODAL | MB_TOPMOST);
	}
	catch (std::exception & e)
	{
		printf("Exception: %s\n", e.what());
		MessageBoxA(NULL, e.what(), "Exception", MB_OK | MB_TASKMODAL | MB_TOPMOST);
	}
	catch(...)
	{
		MessageBoxA(NULL, "Unknown", "Exception", MB_OK | MB_TASKMODAL | MB_TOPMOST);
	}
}

using namespace Sim::Framework;

Framework::Framework() :	taskManager_(new SingleThreadedTaskManager()),
							scheduler_(new Scheduler(taskManager_.get())),
							sceneChangeController_(NULL),
							linkingScene_(NULL),
							objectChangeController_(NULL)

{
	
}

Framework::~Framework()
{
	release();
}

void Framework::initialize(const SystemData * systemData, size_t systemCount, const char * sceneName)
{
	for(size_t i = 0; i < systemCount; i++)
	{
		ISystem * system = PlatformManager::instance().loadSystemLibrary(systemData[i].name, systemData[i].libName);
		systems_.push_back(system);
	}
	sceneChangeController_.reset(new SyncChangeController());
	objectChangeController_.reset(new SyncChangeController());
	linkingScene_.reset(new LinkingScene(sceneChangeController_.get(), objectChangeController_.get()));
	for(Systems::const_iterator itS = systems_.begin();
		itS != systems_.end();
		itS++)
	{
		linkingScene_->extend(*itS);
	}
	sceneName_ = sceneName;
	linkingScene_->load(sceneName);
	processChanges_();
	scheduler_->start();
}

void Framework::execute()
{
	while(!EnvironmentManager::instance().isFinish())
	{
		if(EnvironmentManager::instance().getReloadScene())
		{
			reloadScene();
			EnvironmentManager::instance().resetReloadScene();
		}
		PlatformManager::instance().processMessages();
		scheduler_->execute(linkingScene_.get());
		processChanges_();
	}
}

void Framework::release()
{
	linkingScene_.reset(NULL);
	sceneChangeController_.reset(NULL);;
	objectChangeController_.reset(NULL);;
	systems_.clear();
}

void Framework::reloadScene()
{
	linkingScene_.reset(new LinkingScene(sceneChangeController_.get(), objectChangeController_.get()));
	for(Systems::const_iterator itS = systems_.begin();
		itS != systems_.end();
		itS++)
	{
		linkingScene_->extend(*itS);
	}
	linkingScene_->load(sceneName_.c_str());
	processChanges_();
	scheduler_->start();
}

void Framework::processChanges_()
{
	sceneChangeController_->execute();
	objectChangeController_->execute();
}