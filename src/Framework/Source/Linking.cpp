#include "stdafx.h"

#include "Synchronization.hpp"
#include "Linking.hpp"
#include "IGenericScene.hpp"

#include <assert.h>

using namespace Sim::Framework;

//-=LiningScene=-

LinkingScene::LinkingScene(	IChangeController * sceneChangeController,
							IChangeController * objectChangeController) :
	sceneChangeController_(sceneChangeController),
	objectChangeController_(objectChangeController),
	evenstOfInterest_(IGenericScene::EVENT_GENERIC),
	dirtyEvenstOfInterest_(false)
{
}

LinkingScene::~LinkingScene()
{
	LinkingObjects::const_iterator itLO = linkingObjects_.begin();
	while(itLO != linkingObjects_.end())
	{
		delete itLO->second;
		itLO = linkingObjects_.erase(itLO);
	}
	while(!systemScenes_.empty())
	{
		unextend(systemScenes_.begin()->first);
	}
}

void LinkingScene::load(const char * scene)
{
	for(SystemScenes::const_iterator itSS = systemScenes_.begin();
		itSS != systemScenes_.end();
		itSS++)
	{
		if(strlen(scene) > 0)
			itSS->second->load(scene);
		else
			itSS->second->load("");
		//itSS->second->notifyEvent(itSS->second->getPotentialEvents());
	}
}

void LinkingScene::extend(ISystem * system)
{
	ISystemScene * systemScene = system->createScene();
	systemScenes_[system] = systemScene;
	dirtyEvenstOfInterest_ = true;
	//if(systemScene->getPotentialEvents() & getEventsOfInterest())
	//	sceneChangeController_->registerObserver(systemScene, this);
	for(SystemScenes::const_iterator itSS = systemScenes_.begin();
		itSS != systemScenes_.end();
		itSS++)
	{
		if(itSS->second->getPotentialEvents() & getEventsOfInterest())
			sceneChangeController_->registerObserver(itSS->second, this);
	}
}

void LinkingScene::unextend(ISystem * system)
{
	SystemScenes::const_iterator itSS = systemScenes_.find(system);
	assert(itSS != systemScenes_.end());
	for(LinkingObjects::const_iterator itLO = linkingObjects_.begin();
		itLO != linkingObjects_.end();
		itLO++)
	{
		itLO->second->unextend(itSS->second);
	}
	itSS->second->setObserver(NULL);
	systemScenes_.erase(itSS);
	dirtyEvenstOfInterest_ = true;
	system->destroyScene();
}

void LinkingScene::createObjects(IGenericScene * genericScene)
{
	const CreateObjectsData & createObjectsData = genericScene->getCreateObjectsData();
	for(CreateObjectsData::const_iterator itCOD = createObjectsData.begin();
		itCOD != createObjectsData.end();
		itCOD++)
	{
		const CreateObjectData & createObjectData = *itCOD;
		createObject_(genericScene, createObjectData);
	}
}

void LinkingScene::destroyObjects(IGenericScene * genericScene)
{
	const DestroyObjectsData & destroyObjectsData = genericScene->getDestroyObjectsData();
	for(DestroyObjectsData::const_iterator itDOD = destroyObjectsData.begin();
		itDOD != destroyObjectsData.end();
		itDOD++)
	{
		const DestroyObjectData & destroyObjectData = *itDOD;
		destroyObject_(destroyObjectData);
	}
}

Event LinkingScene::getEventsOfInterest() const
{
	if(dirtyEvenstOfInterest_)
		updateEventsOfInterest_();
	return evenstOfInterest_;
}

void LinkingScene::onEvent(ISubject * subject, Event event)
{
	if(event & IGenericScene::EVENT_CREATE_OBJECT)
		createObjects(static_cast<IGenericScene*>(subject));
	if(event & IGenericScene::EVENT_DESTROY_OBJECT)
		destroyObjects(static_cast<IGenericScene*>(subject));
	for(SystemScenes::const_iterator itSS = systemScenes_.begin();
		itSS != systemScenes_.end();
		itSS++)
	{
		if(itSS->second != subject)
			itSS->second->onEvent(subject, event);
	}
}

void LinkingScene::createObject_(IGenericScene * genericScene, const CreateObjectData & createObjectData)
{
	LinkingObject * linkingObject = new LinkingObject(	createObjectData.objectName,
														createObjectData.objectClass,
														genericScene,
														objectChangeController_);
	for(SystemScenes::const_iterator itSS = systemScenes_.begin();
		itSS != systemScenes_.end();
		itSS++)
	{
		if(itSS->second != genericScene)
			linkingObject->extend(itSS->second);
	}
	ISystemObject * primaryObject = genericScene->extend(createObjectData.objectName, createObjectData.objectClass, createObjectData.data);
	linkingObject->extend(primaryObject);
	linkingObjects_[createObjectData.objectName] = linkingObject;
	primaryObject->notifyEvent(primaryObject->getPotentialEvents());
}

void LinkingScene::destroyObject_(const DestroyObjectData & destroyObjectData)
{
	LinkingObjects::iterator itLO = linkingObjects_.find(destroyObjectData.objectName);
	assert(itLO != linkingObjects_.end());
	if(itLO != linkingObjects_.end())
	{
		delete itLO->second;
		linkingObjects_.erase(itLO);
	}
}

void LinkingScene::updateEventsOfInterest_() const
{
	evenstOfInterest_ = IGenericScene::EVENT_GENERIC;
	for(SystemScenes::const_iterator itSS = systemScenes_.begin();
		itSS != systemScenes_.end();
		itSS++)
	{
		evenstOfInterest_ |= itSS->second->getEventsOfInterest();
	}
	dirtyEvenstOfInterest_ = false;
}

//-=LinkingObject=-

LinkingObject::LinkingObject(	const ObjectName & objectName,
								const ObjectName & objectClass,
								IGenericScene * genericScene,
								IChangeController * changeController) :
									name_(objectName),
									class_(objectClass),
									genericScene_(genericScene),
									changeController_(changeController),
									evenstOfInterest_(NO_EVENT),
									dirtyEvenstOfInterest_(false)
{
	assert(genericScene_ != NULL);
}

LinkingObject::~LinkingObject()
{
	SystemObjects::const_iterator itSS = systemObjects_.begin();
	while(itSS != systemObjects_.end())
	{
		changeController_->unregisterObserver(itSS->second, this);
		if(itSS->first == genericScene_)
			genericScene_->unextend(itSS->second);
		else
			itSS->first->destroyObject(itSS->second);
		itSS = systemObjects_.erase(itSS);
	}
	dirtyEvenstOfInterest_ = true;
}

void LinkingObject::extend(ISystemScene * systemScene)
{
	ISystemObject * systemObject = systemScene->createObject(name_, class_);
	if(systemObject != NULL)
		extend(systemObject);
}

void LinkingObject::extend(ISystemObject * systemObject)
{
	systemObjects_[systemObject->getSystemScene()] = systemObject;
	dirtyEvenstOfInterest_ = true;

	//if(systemObject->getPotentialEvents() && getEventsOfInterest())
	//	changeController_->registerObserver(systemObject, this);

	for(SystemObjects::const_iterator itSS = systemObjects_.begin();
		itSS != systemObjects_.end();
		itSS++)
	{
		if(itSS->second->getPotentialEvents() && getEventsOfInterest())
			changeController_->registerObserver(itSS->second, this);
	}
}

void LinkingObject::unextend(ISystemScene * systemScene)
{
	SystemObjects::const_iterator itSS = systemObjects_.find(systemScene);
	if(itSS != systemObjects_.end())
	{
		changeController_->unregisterObserver(itSS->second, this);
		if(systemScene == genericScene_)
			genericScene_->unextend(itSS->second);
		else
			itSS->first->destroyObject(itSS->second);
		systemObjects_.erase(itSS);
	}
	dirtyEvenstOfInterest_ = true;

}

Event LinkingObject::getEventsOfInterest() const
{
	if(dirtyEvenstOfInterest_)
		updateEventsOfInterest_();
	return evenstOfInterest_;
}

void LinkingObject::onEvent(ISubject * subject, Event event)
{
	for(SystemObjects::const_iterator itSS = systemObjects_.begin();
		itSS != systemObjects_.end();
		itSS++)
	{
		if(itSS->second != subject)
			itSS->second->onEvent(subject, event);
	}
}

void LinkingObject::updateEventsOfInterest_() const
{
	evenstOfInterest_ = NO_EVENT;
	for(SystemObjects::const_iterator itSS = systemObjects_.begin();
		itSS != systemObjects_.end();
		itSS++)
	{
		evenstOfInterest_ |= itSS->second->getEventsOfInterest();
	}
	dirtyEvenstOfInterest_ = false;
}