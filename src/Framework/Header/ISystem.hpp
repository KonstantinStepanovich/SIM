#ifndef _ISystem_hpp_
#define _ISystem_hpp_

#include "Time.hpp"
#include "Core/ModelTime/ModelTime.hpp"
#include "Synchronization.hpp"

#include <string>
#include <vector>
#include <list>

namespace Sim
{

namespace Framework
{

class ISystemScene;
class ISystemObject;
class ISystemTask;

class IEnvironmentManager;

typedef std::string SystemName;

class ISystem
{
public:
	virtual ~ISystem() {;}
	virtual const SystemName & getName() const = 0;
	virtual ISystemScene * createScene() = 0;
	virtual void destroyScene() = 0;
};

typedef std::list<ISystem*> Systems;

typedef std::string ObjectName;
typedef std::string ObjectClass;

class ISystemScene : public ISubject, public IObserver
{
public:
	virtual ~ISystemScene() {;}
	virtual void load(const char * scenery) = 0;

	virtual ISystem * getSystem() = 0;
	virtual ISystemTask * getSystemTask() = 0;
	
	virtual ISystemObject * createObject(const ObjectName & objectName, const ObjectClass & objectClass) = 0;
	virtual void destroyObject(ISystemObject * systemObject) = 0;
};

class ISystemObject : public ISubject, public IObserver
{
public:
	ISystemObject(const ObjectName & objectName) : objectName_(objectName){;}
	virtual ~ISystemObject() {;}
	inline const ObjectName & getName() const { return objectName_; }
	virtual const ObjectClass & getClass() const = 0;
	virtual ISystemScene * getSystemScene() const = 0;
protected:
	ObjectName objectName_;
};

class ISystemTask
{
public:
	virtual ~ISystemTask() {;}
	virtual ISystemScene * getSystemScene() const = 0;
	virtual void operator()(Time time, ModelTime modelTime) = 0;
};

}

}

extern "C"
{

struct Managers
{
	Sim::Framework::IEnvironmentManager * environmentManager;
};

typedef /*__declspec(dllimport)*/ Sim::Framework::ISystem * (/*__stdcall*/* CreateSystem)(const char * systemName, const Managers * managers);
typedef /*__declspec(dllimport)*/ void (/*__stdcall*/* DestroySystem)(Sim::Framework::ISystem * system);

}

#endif _ISystem_hpp_