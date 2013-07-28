#ifndef _Linking_hpp_
#define _Linking_hpp_

#include "Synchronization.hpp"
#include "ISystem.hpp"
#include <map>

namespace Sim
{

namespace Framework
{

//LinkingScene
//Links system scenes and provides data exchange between them

class LinkingObject;

class IGenericScene;
struct CreateObjectData;
struct DestroyObjectData;

class LinkingScene : public IObserver
{
public:
	typedef std::map<ISystem*, ISystemScene*> SystemScenes;
	LinkingScene(	IChangeController * sceneChangeController,
					IChangeController * objectChangeController);
	~LinkingScene();
	const SystemScenes & getSystemScenes() const { return systemScenes_; }
	void load(const char * scene);
	void extend(ISystem * system);
	void unextend(ISystem * system);
	void createObjects(IGenericScene * genericScene);
	void destroyObjects(IGenericScene * genericScene);
	//IObserver interface
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);
private:
	void createObject_(IGenericScene * genericScene, const CreateObjectData & createObjectData);
	void destroyObject_(const DestroyObjectData & destroyObjectData);
	void updateEventsOfInterest_() const;
	SystemScenes systemScenes_;
	typedef std::map<ObjectName, LinkingObject*> LinkingObjects;
	LinkingObjects linkingObjects_;
	IChangeController * const sceneChangeController_;
	IChangeController * const objectChangeController_;
	mutable bool dirtyEvenstOfInterest_;
	mutable Event evenstOfInterest_;
};

//LinkingObject
//Links object representations in different systems and provides data exchange between them

class LinkingObject : public IObserver
{
public:
	LinkingObject(	const ObjectName & objectName,
					const ObjectClass & objectClass,
					IGenericScene * genericScene,
					IChangeController * changeController);
	~LinkingObject();
	void extend(ISystemScene * systemScene);
	void extend(ISystemObject * systemObject);
	void unextend(ISystemScene * systemScene);
	//IObserver interface
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);
private:
	void updateEventsOfInterest_() const;
	const ObjectName name_;
	const ObjectName class_;
	IGenericScene * const genericScene_;
	typedef std::map<ISystemScene*, ISystemObject*> SystemObjects;
	SystemObjects systemObjects_;
	IChangeController * const changeController_;
	mutable bool dirtyEvenstOfInterest_;
	mutable Event evenstOfInterest_;
};

}

}

#endif _Linking_hpp_