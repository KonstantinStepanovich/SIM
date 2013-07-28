#ifndef _ModelTestAPI_h_
#define _ModelTestAPI_h_

#include "DataModelAPI.h"

typedef void * ModelHandle;

typedef void * TestBaseHandle;

struct Model
{
	const char*		(*getName)		(ModelHandle handle);
	const char*		(*getType)		(ModelHandle handle);
	bool			(*load)			(ModelHandle handle);
	void			(*unload)		(ModelHandle handle);
	bool			(*isLoaded)		(ModelHandle handle);
	void			(*save)			(ModelHandle handle);
	dmValueHandle	(*getParameters)(ModelHandle handle);
	TestBaseHandle	(*getTestBase)(ModelHandle handle);
};

struct ModelBase
{
	size_t		(*getSize)		();
	ModelHandle	(*getModel)		(size_t index);
	ModelHandle (*add)			(const char * name);
};

typedef void * TestHandle;

typedef void(*TestResultChangeCallback)(void * data, size_t * path, size_t pathSize);

struct Test
{
	const char*		(*getName)		(TestHandle handle);
	void 			(*enable)		(TestHandle handle);
	void 			(*disable)		(TestHandle handle);
	bool 			(*isEnabled)	(TestHandle handle);
	dmValueHandle	(*getParameters)(TestHandle handle);
	dmValueHandle	(*getResult)	(TestHandle handle);
	void			(*setResultChangeCallback)(TestHandle handle, TestResultChangeCallback callback, void * data);
};

struct TestBase
{
	size_t		(*getSize)		(TestBaseHandle testBaseHandle);
	TestHandle	(*getTest)		(TestBaseHandle testBaseHandle, size_t index);
};

struct ModelTestAPI
{
	Model			model;
	ModelBase		modelBase;
	Test			test;
	TestBase		testBase;
	DataModelAPI 	dataModelAPI;
};

typedef void (*ModelTestAPIInitialize)(ModelTestAPI * modelTestAPI);
typedef void (*ModelTestAPIRelease)();

#endif _ModelTestAPI_h_