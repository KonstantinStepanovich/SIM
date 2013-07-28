#ifndef _ShapeGen_Scene_hpp_
#define _ShapeGen_Scene_hpp_

#include "ISystem.hpp"
#include "IGenericScene.hpp"

#include <map>

#include "Math/Box3.hpp"

namespace Sim
{

using namespace Framework;

namespace ShapeGen
{

class System;
class Task;
class Object;

class Scene : public IGenericScene
{
public:
	Scene(System * system);
	~Scene();
	static Scene * instance();
	virtual void load(const char * scene);

	virtual ISystem * getSystem();
	virtual ISystemTask * getSystemTask();

	virtual ISystemObject * createObject(const ObjectName & objectName, const ObjectClass & objectClass);
	virtual void destroyObject(ISystemObject * systemObject);

	virtual void onEvent(ISubject* subject, Event event);
	virtual Event getEventsOfInterest() const;
	virtual Event getPotentialEvents() const;

	virtual ISystemObject * extend(const ObjectName & objectName, const ObjectClass & objectClass, void * data);
	virtual void unextend(ISystemObject * systemObject);

	virtual const CreateObjectsData & getCreateObjectsData();
	virtual const DestroyObjectsData & getDestroyObjectsData();

	void update(ModelTime modelTime);
	void addObject(Object * object);
	void addObject(const ObjectName & objectName, const ObjectClass & objectClass, Object * object);
	void removeObject(Object * object);
private:

	void clearGeneric_();
	
	static Scene *		instance_;
	System *			system_;
	std::auto_ptr<Task>	task_;
	CreateObjectsData	createObjectsData_;
	DestroyObjectsData	destroyObjectsData_;
};

}

}

#endif _ShapeGen_Scene_hpp_