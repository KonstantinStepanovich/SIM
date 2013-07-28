#ifndef _IGenericScene_hpp_
#define _IGenericScene_hpp_

#include "ISystem.hpp"

namespace Sim
{

namespace Framework
{

struct CreateObjectData
{
	CreateObjectData(const ObjectName & objectNameIn, const ObjectClass & objectClassIn, void *	dataIn) :
		objectName(objectNameIn), objectClass(objectClassIn), data(dataIn)
	{
	}
	ObjectName	objectName;
	ObjectClass	objectClass;
	void *		data;
};

typedef std::vector<CreateObjectData> CreateObjectsData;

struct DestroyObjectData
{
	DestroyObjectData(const ObjectName & objectNameIn, void * dataIn) :
		objectName(objectNameIn), data(dataIn)
	{
	}
	ObjectName	objectName;
	void *		data;
};

typedef std::vector<DestroyObjectData> DestroyObjectsData;

class IGenericScene : public ISystemScene
{
public:
	enum
	{
		EVENT_CREATE_OBJECT = 1 << 0,
		EVENT_DESTROY_OBJECT = 1 << 1,
		EVENT_GENERIC = EVENT_CREATE_OBJECT | EVENT_DESTROY_OBJECT
	};
	virtual ~IGenericScene() {;}
	
	virtual ISystemObject * extend(const ObjectName & objectName, const ObjectClass & objectClass, void * data) = 0;
	virtual void unextend(ISystemObject * systemObject) = 0;

	virtual const CreateObjectsData & getCreateObjectsData() = 0;
	virtual const DestroyObjectsData & getDestroyObjectsData() = 0;
};

}

}

#endif _IGenericScene_hpp_