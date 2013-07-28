#include "stdafx.h"
#include "EnvironmentManager.hpp"

using namespace Sim::Framework;

EnvironmentManager::EnvironmentManager() : finish_(false), pause_(false),
	timeAccellerationScale_(1), timeDecellerationScale_(1), reloadScene(false)
{

}

void EnvironmentManager::setFinish()
{
	finish_ = true;
}

void EnvironmentManager::setPause(bool pause)
{
	pause_ = pause;
}

void EnvironmentManager::setTimeNormal()
{
	timeAccellerationScale_ = 1;
	timeDecellerationScale_ = 1;
}

void EnvironmentManager::setTimeAcceleration(long scale)
{
	timeAccellerationScale_ = scale;
	timeDecellerationScale_ = 1;
}

void EnvironmentManager::setTimeDecelleration(long scale)
{
	timeDecellerationScale_ = scale;
	timeAccellerationScale_ = 1;
}

void EnvironmentManager::setReloadScene()
{
	reloadScene = true;
}

void EnvironmentManager::resetReloadScene()
{
	reloadScene = false;
}

bool EnvironmentManager::isFinish() const
{
	return finish_;
}

bool EnvironmentManager::isPause() const
{
	return pause_;
}

long EnvironmentManager::getTimeAccelleration() const
{
	return timeAccellerationScale_;
}

long EnvironmentManager::getTimeDecelleration() const
{
	return timeDecellerationScale_;
}

bool EnvironmentManager::getReloadScene() const
{
	return reloadScene;
}

EnvironmentManager & EnvironmentManager::instance()
{
	static EnvironmentManager environmentManager;
	return environmentManager;
}