#ifndef _MathModelSFM_hpp_
#define _MathModelSFM_hpp_

extern "C"
{
#include "ModelTestAPI.h"
}

#include "Value.hpp"

#include <vector>

#include "Modules/SFM/Model/Airplane.hpp"

namespace DataModel
{
class Value;
}
using DataModel::Value;

namespace MathModelSFM
{

struct ModelParameters
{
	Sim::SFM::Airplane::Desc airplaneDesc;
};

enum AirSpeedType
{
	PARAMETER_M,
	PARAMETER_TAS,
	PARAMETER_IAS,
	PARAMETER_AS_MAX
};

struct TestCommonParameters
{
	Sim::SFM::Airplane::State	state;
	AirSpeedType				airSpeedType;
};

class TestBase;

class Test
{
public:
	typedef TestCommonParameters CommonParameters;
	class DataParameters;
	class DataResult;

	Test(ModelParameters * modelParameters, TestBase * testBase);
	~Test();
	virtual const char * getName() const = 0;
	void enable();
	void disable();
	bool isEnabled() const;
	Value * getParameters();
	virtual Value * getResult();
	void setResultChangeCallback(TestResultChangeCallback callback, void * data);
	virtual void update() = 0;
protected:
	ModelParameters * modelParameters_;
	TestBase * testBase_;
	CommonParameters commonParameters_;

	class ChangeHandler : public DataModel::ChangeHandler
	{
	public:
		ChangeHandler(Test * test);
		//virtual DataModel::ChangeHandler * clone() const;
		virtual void operator()(Value * value);
	private:
		Test * test_;
	};
	friend class ChangeHandler;
	ChangeHandler * ñhangeHandler;

	void notifyResultChanged(size_t * path, size_t pathSize);

private:
	bool enabled_;
	
	TestResultChangeCallback callback_;
	void * data_;

	Value * getCommonParameters_();	
	virtual Value * getSpecificParameters_();	
public:
	static const char *		getName			(TestHandle handle);
	static void 			enable			(TestHandle handle);
	static void 			disable			(TestHandle handle);
	static bool 			isEnabled		(TestHandle handle);
	static dmValueHandle	getParameters	(TestHandle handle);
	static dmValueHandle	getResult		(TestHandle handle);
	static void				setResultChangeCallback(TestHandle handle, TestResultChangeCallback callback, void * data);
};

class TestBase
{
public:
	TestBase(ModelParameters * modelParameters);
	void initTests();
	size_t getSize() const;
	Test * getTest(size_t index) const;
	void updateTests();
private:
	ModelParameters * modelParameters_;
	class ChangeHandler;
	friend class ChangeHandler;
	ChangeHandler * ñhangeHandler_;
	//Tests
	enum { SIZE = 7 };
	Test * tests_[SIZE];
public:
	static size_t		getSize			(TestBaseHandle testBaseHandle);
	static TestHandle	getTest			(TestBaseHandle testBaseHandle, size_t index);

};

class Model
{
public:
	Model(const std::string & name);	
	~Model();
	const char *	getName() const;
	const char *	getType() const;
	bool			load();
	void			unload();
	bool			isLoaded() const;
	void			save();
	Value *			getParameters();
	TestBase *		getTestBase();
private:
	ModelParameters parameters_;

	class ChangeHandler;
	friend class ChangeHandler;
	ChangeHandler * ñhangeHandler_;

	TestBase testBase_;
	std::string name_;
	bool loaded_;
public:
	static const char *		getName			(ModelHandle handle);
	static const char *		getType			(ModelHandle handle);
	static bool				load			(ModelHandle handle);
	static void				unload			(ModelHandle handle);
	static bool				isLoaded		(ModelHandle handle);
	static void				save			(ModelHandle handle);
	static dmValueHandle	getParameters	(ModelHandle handle);
	static TestBaseHandle	getTestBase	(ModelHandle handle);
};

class ModelBase
{
	ModelBase();
public:
	static ModelBase & instance();
	size_t getSize() const;
	Model* getModel(size_t index);
	Model* add(const char * name);
public:
	static size_t		getSizeS		();
	static ModelHandle	getModelS		(size_t index);
	static ModelHandle	addS			(const char * name);
private:
	typedef std::vector<Model*> Models;
	Models models_;
};

}

#endif _MathModelSFM_hpp_