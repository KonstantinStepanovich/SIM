#ifndef _EnvironmentManager_hpp_
#define _EnvironmentManager_hpp_

#include "IEnvironmentManager.hpp"
#include "Core/ModelTime/ModelTime.hpp"

namespace Sim
{

namespace Framework
{

class EnvironmentManager : public IEnvironmentManager
{
public:
	EnvironmentManager();
	virtual void setFinish();
	virtual void setPause(bool pause);
	virtual void setTimeNormal();
	virtual void setTimeAcceleration(long scale);
	virtual void setTimeDecelleration(long scale);
	virtual void setReloadScene();
	virtual void resetReloadScene();

	virtual bool isFinish() const;
	virtual bool isPause() const;
	virtual long getTimeAccelleration() const;
	virtual long getTimeDecelleration() const;
	virtual bool getReloadScene() const;

	static EnvironmentManager & instance();
private:
	bool finish_;
	bool pause_;
	long timeAccellerationScale_;
	long timeDecellerationScale_;
	bool reloadScene;
};

}

}

#endif _EnvironmentManager_hpp_