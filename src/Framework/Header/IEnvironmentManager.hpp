#ifndef _IEnvironmentManager_hpp_
#define _IEnvironmentManager_hpp_

#include "Core/ModelTime/ModelTime.hpp"

namespace Sim
{

namespace Framework
{

class IEnvironmentManager
{
public:
	virtual ~IEnvironmentManager() {;}
	virtual void setFinish() = 0;
	virtual void setPause(bool pause) = 0;
	virtual void setTimeNormal() = 0;
	virtual void setTimeAcceleration(long scale) = 0;
	virtual void setTimeDecelleration(long scale) = 0;
	virtual void setReloadScene() = 0;
	virtual void resetReloadScene() = 0;

	virtual bool isFinish() const = 0;
	virtual bool isPause() const = 0;
	virtual long getTimeAccelleration() const = 0;
	virtual long getTimeDecelleration() const = 0;
	virtual bool getReloadScene() const = 0;
};

}

}

#endif _IEnvironmentManager_hpp_