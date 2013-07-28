#ifndef _Framework_hpp_
#define _Framework_hpp_

#include "ISystem.hpp"

struct SystemData;

namespace Sim
{

namespace Framework
{

class ITaskManager;
class Scheduler;
class LinkingScene;
//typedef ::SystemData SystemData;

class Framework
{
public:
	Framework();
	~Framework();
	void initialize(const SystemData * systemData,
					size_t systemCount,
					const char * sceneName);
	void execute();
	void release();
	void reloadScene();
private:
	void processChanges_();
	std::auto_ptr<ITaskManager> taskManager_;
	std::auto_ptr<Scheduler> scheduler_;
	std::auto_ptr<LinkingScene> linkingScene_;
	std::auto_ptr<IChangeController> sceneChangeController_;
	std::auto_ptr<IChangeController> objectChangeController_;
	Systems systems_;
	std::string sceneName_;
};

}

}

#endif _Framework_hpp_