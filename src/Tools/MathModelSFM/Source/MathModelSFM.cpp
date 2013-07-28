#include "MathModelSFM.hpp"

#include <math.h>

#include "boost/filesystem.hpp"

#include "Math/Scalar.hpp"
#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"
#include "Database/ParserJson.hpp"
#include "Database/DatabaseUtil.hpp"
#include "Utils/Units.hpp"

#include "ValueMath.hpp"
#include "Limits.hpp"

using namespace MathModelSFM;
using namespace DataModel;

namespace DataModel
{

ValueTable * createValue(Sim::SFM::AirframeForce::Desc::Param & param, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("cx0",		createValue(param.cx0, changeHandler)),
		ItemTable("dCydAoA",	createValue(param.dCydAoA, changeHandler)),
		ItemTable("CyMax",		createValue(param.CyMax, changeHandler)),
		ItemTable("bPol",		createValue(param.bPol2, changeHandler)),
		ItemTable("bPol4",		createValue(param.bPol4, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::AirframeForce::Desc & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("table",				createValueLinearInterpolator(desc.table, "M", "param", changeHandler)),
		ItemTable("Cy0",				createValue(desc.Cy0, changeHandler)),
		ItemTable("flaps Cx",			createValue(desc.flapsCx, changeHandler)),
		ItemTable("flaps Cy",			createValue(desc.flapsCy, changeHandler)),
		ItemTable("air brake Cx",		createValue(desc.airBrakeCx, changeHandler)),
		ItemTable("drag parachute Cx",	createValue(desc.dragParachuteCx, changeHandler)),
		ItemTable("gear Cx",			createValue(desc.gearCx, changeHandler)),
		ItemTable("wing area",				createValue(desc.S, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::AirframeTorque::DescMxMy & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("coeffCL",			createValue(desc.coeffCL, changeHandler)),
		ItemTable("coeffMxAoS",			createValue(desc.coeffMxAoS, changeHandler)),
		ItemTable("wingV",				createValue(desc.wingV, changeHandler)),
		ItemTable("coeffAdverseYaw",	createValue(desc.coeffAdverseYaw, changeHandler)),
		ItemTable("slipRangeWhereRudderEffective",	createValue(desc.slipRangeWhereRudderEffective, changeHandler)),
		ItemTable("coeffRudder",		createValue(desc.coeffRudder, changeHandler)),
		ItemTable("coeffMxOmegaX",		createValue(desc.coeffMxOmegaX, changeHandler)),
		ItemTable("dMydOmegaY",			createValue(desc.dMydOmegaY, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::AirframeTorque::DescMz & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("coeffCL",			createValue(desc.coeffCL, changeHandler)),
		ItemTable("coeffCD",			createValue(desc.coeffCD, changeHandler)),
		ItemTable("coeffDiveMomentWhenStabilizierLost",	createValue(desc.coeffDiveMomentWhenStabilizierLost, changeHandler)),
		ItemTable("aoaRangeWhereRudderEffective",	createValue(desc.aoaRangeWhereRudderEffective, changeHandler)),
		ItemTable("elevatorEffectiveness",	createValue(desc.elevatorEffectiveness, changeHandler)),
		ItemTable("dMzdOmegaZ",			createValue(desc.dMzdOmegaZ, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::AirframeTorque::DescriptorCommon & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("wing span",				createValue(desc.wingSpan, changeHandler)),
		ItemTable("mean aerodynamic chord",	createValue(desc.MAC, changeHandler)),
		ItemTable("wing area",				createValue(desc.S, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::AirframeTorque::Desc & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("Mx, My",					createValue(desc.MxMy, changeHandler)),
		ItemTable("Mz",						createValue(desc.Mz, changeHandler)),
		ItemTable("common",					createValue(desc.common, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::Airframe::Desc & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("force",			createValue(desc.force, changeHandler)),
		ItemTable("torque",			createValue(desc.torque, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::FuelSystemDesc & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("internal fuel max",	createValue(desc.internalFuelMax,	changeHandler)),
		ItemTable("fuel dumping",		createValue(desc.fuelDumping,		changeHandler, &limitsBool)),
		ItemTable("fuel dump rate max",	createValue(desc.fuelDumpRateMax,	changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::JetEngine::Desc::Thrust & thrust, ChangeHandler * changeHandler)
{
	if(Sim::Math::isInvalid(thrust.afterburner))
		return new ValueTable(ItemTable("max", createValue(thrust.maximal,		changeHandler)), changeHandler);
	else
	{
		const ItemTable itemTable[] = {
			ItemTable("max",				createValue(thrust.maximal,		changeHandler)),
			ItemTable("aft",				createValue(thrust.afterburner,	changeHandler))
		};
		return new ValueTable(itemTable, changeHandler);
	}
}

ValueTable * createValue(Sim::SFM::JetEngine::Desc::Cons & cons, ChangeHandler * changeHandler)
{
	if(Sim::Math::isInvalid(cons.afterburner))
		return new ValueTable(ItemTable("max", createValue(cons.maximal,		changeHandler)), changeHandler);
	else
	{
		const ItemTable itemTable[] = {
			ItemTable("max",				createValue(cons.maximal,		changeHandler)),
			ItemTable("aft",				createValue(cons.afterburner,	changeHandler))
		};
		return new ValueTable(itemTable, changeHandler);
	}
}

ValueTable * createValue(Sim::SFM::JetEngine::Desc::Param & element, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("thrust",				createValue(element.thrust,		changeHandler)),
		ItemTable("cons",				createValue(element.cons,		changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::JetEngine::Desc & desc, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("has afterburner",createValue(desc.hasAfterburner, changeHandler)),
		ItemTable("table",			createValueBilinearInterpolator(desc.table, "H", "M", "P", changeHandler)),
		ItemTable("Kdr",			createValue(desc.kDr, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(Sim::SFM::Airplane::Desc & desc, ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable("engine count",	createValue(desc.engineCount, changeHandler)),
		ItemTable("empty mass",		createValue(desc.mass, changeHandler)),
		ItemTable("center of mass",	createValue(desc.centerOfMass, changeHandler)),
		ItemTable("relative inertia tensor", createValue(desc.relInertiaTensor, changeHandler)),
		ItemTable("airframe",		createValue(Sim::SFM::Airframe::Database::instance().regDesc(desc.airframeId), changeHandler)),
		ItemTable("jet engine",		createValue(Sim::SFM::JetEngine::Database::instance().regDesc(desc.jetEngineId), changeHandler)),
		ItemTable("fuel system",	createValue(desc.fuelSystem, changeHandler)),		
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(ModelParameters & modelParameters, ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable("airplane",		createValue(modelParameters.airplaneDesc, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

}

static const char * airSpeedTypeName[] = {"M", "TAS", "IAS"};

static const dmLimitsInt limitsAirSpeedType = {
	LIMIT_INT_TYPE_VARIANTS,
	{
		{},
		{
			3,
				airSpeedTypeName
		}
	}
};

//-=Test=-

MathModelSFM::Test::ChangeHandler::ChangeHandler(Test * test) : test_(test)
{
}

//MathModelSFM::Test::ChangeHandler * MathModelSFM::Test::ChangeHandler::clone() const
//{
//	return new ChangeHandler(*this);
//}

void MathModelSFM::Test::ChangeHandler::operator()(Value * value)
{	
	if(test_->isEnabled())
		test_->update();
}

class InternalFuelRatio : public Value
{
public:
	InternalFuelRatio(	float * internalFuel, float internalFuelMax,
						ChangeHandler * changeHandlerIn) : Value(changeHandlerIn), internalFuel_(internalFuel), internalFuelMax_(internalFuelMax)
	{
	}
	virtual Type getType() const
	{
		return VALUE_TYPE_REAL;
	}
	virtual dmReal getReal() const
	{
		return (dmInteger)*internalFuel_ / internalFuelMax_;
	};
	virtual const dmLimitsReal * getLimitsReal() const
	{
		return NULL;
	}
	virtual void setReal(dmReal val)
	{
		*internalFuel_ = (float)val * internalFuelMax_;
	}
private:
	float * internalFuel_;
	const float internalFuelMax_;
};

MathModelSFM::Test::Test(ModelParameters * modelParameters, TestBase * testBase) : 
	modelParameters_(modelParameters), testBase_(testBase),
	enabled_(false), callback_(NULL), data_(NULL)
{
	commonParameters_.state = Sim::SFM::Airplane::defaultState;
	commonParameters_.state.fuelSystem.internalFuel = 0.5f * modelParameters_->airplaneDesc.fuelSystem.internalFuelMax;
	commonParameters_.airSpeedType = PARAMETER_TAS;
	ñhangeHandler = new ChangeHandler(this);
}

MathModelSFM::Test::~Test()
{
	delete ñhangeHandler;
}

void MathModelSFM::Test::enable()
{
	enabled_ = true;
}

void MathModelSFM::Test::disable()
{
	enabled_ = false;
}

bool MathModelSFM::Test::isEnabled() const
{
	return enabled_;
}

Value * MathModelSFM::Test::getParameters()
{
	Value * specificParameters = getSpecificParameters_();
	if(specificParameters != NULL)
	{
		const ItemTable itemTable[] = {
			ItemTable("common", getCommonParameters_()),
			ItemTable("specific", specificParameters),
		};
		return new ValueTable(itemTable, ñhangeHandler);
	}
	else
		return getCommonParameters_();
}

Value * MathModelSFM::Test::getResult()
{
	return NULL;
}

void MathModelSFM::Test::setResultChangeCallback(TestHandle handle, TestResultChangeCallback callback, void * data)
{
	((MathModelSFM::Test*)handle)->setResultChangeCallback(callback, data);
}

void MathModelSFM::Test::notifyResultChanged(size_t * path, size_t pathSize)
{
	if(callback_ != NULL)
		(*callback_)(data_, path, pathSize);
}

Value * MathModelSFM::Test::getCommonParameters_()
{
	const ItemTable itemTable[] = {
		ItemTable("payload mass",			createValue(commonParameters_.state.payloadMass, ñhangeHandler)),
		ItemTable("internal fuel ratio",	new InternalFuelRatio(&commonParameters_.state.fuelSystem.internalFuel, modelParameters_->airplaneDesc.fuelSystem.internalFuelMax, ñhangeHandler)),
		ItemTable("flaps",					createValue(commonParameters_.state.airframe.force.flaps, ñhangeHandler, &limitsPercent)),
		ItemTable("gear",					createValue(commonParameters_.state.airframe.force.gear, ñhangeHandler, &limitsPercent)),
		ItemTable("air speed type",			createValueInteger(commonParameters_.airSpeedType, ñhangeHandler, &limitsAirSpeedType))
	};
	return new ValueTable(itemTable, ñhangeHandler);
}

Value * MathModelSFM::Test::getSpecificParameters_()
{
	return NULL;
}

const char * MathModelSFM::Test::getName(TestHandle handle)
{
	return ((MathModelSFM::Test*)handle)->getName();
}

void MathModelSFM::Test::enable(TestHandle handle)
{
	MathModelSFM::Test * test = (MathModelSFM::Test*)handle;
	test->enable();
	test->update();
}

void MathModelSFM::Test::disable(TestHandle handle)
{
	((MathModelSFM::Test*)handle)->disable();
}

bool MathModelSFM::Test::isEnabled(TestHandle handle)
{
	return ((MathModelSFM::Test*)handle)->isEnabled();
}

dmValueHandle MathModelSFM::Test::getParameters(TestHandle handle)
{
	return ((MathModelSFM::Test*)handle)->getParameters();
}

dmValueHandle MathModelSFM::Test::getResult(TestHandle handle)
{
	return ((MathModelSFM::Test*)handle)->getResult();
}

void MathModelSFM::Test::setResultChangeCallback(TestResultChangeCallback callback, void * data)
{
	callback_ = callback;
	data_ = data;
}

//-=TestBase=-

class MathModelSFM::TestBase::ChangeHandler : public DataModel::ChangeHandler
{
public:
	ChangeHandler(TestBase * testBase) : testBase_(testBase)
	{
	}
	virtual DataModel::ChangeHandler * clone() const
	{
		return new ChangeHandler(*this);
	}
	virtual void operator()(Value * value)
	{	
		testBase_->updateTests();
	}
private:
	TestBase * testBase_;
};

//-=TestBase=-

MathModelSFM::TestBase::TestBase(ModelParameters * modelParameters) : modelParameters_(modelParameters)
{
	ñhangeHandler_ = new ChangeHandler(this);
}

size_t MathModelSFM::TestBase::getSize() const
{
	return SIZE;
}

MathModelSFM::Test * MathModelSFM::TestBase::getTest(size_t index) const
{
	return tests_[index];
}

void MathModelSFM::TestBase::updateTests()
{
	for(size_t i = 0; i < SIZE; ++i)
	{
		if(tests_[i]->isEnabled())
			tests_[i]->update();
	}
}

size_t MathModelSFM::TestBase::getSize(TestBaseHandle testBaseHandle)
{
	return ((TestBase*)(testBaseHandle))->getSize();
}

TestHandle MathModelSFM::TestBase::getTest(TestBaseHandle testBaseHandle, size_t index)
{
	return ((TestBase*)(testBaseHandle))->getTest(index);
}

//-=ModelBase=-

//void load2(const char * path, const char * table, Sim::Variant & variant)
//{
//	try
//	{
//		Sim::loadLua(path, table, variant);
//	}
//	catch (...)
//	{
//		Sim::loadJson(path, table, variant);
//	}
//}

template<class Desc>
void loadDescFile2(const char * name, const char * dir, const char * luaTable, Desc & desc)
{
	try
	{	
		loadDescFile(name, dir, ".js", &Sim::loadJson, "", desc);
	}
	catch(...)
	{
		loadDescFile(name, dir, ".lua", &Sim::loadLua, luaTable, desc);
	}
}

MathModelSFM::ModelBase::ModelBase()
{
	try
	{
		boost::filesystem::path path("SFM/Airplanes");
		if(	boost::filesystem::exists(path) &&
			boost::filesystem::is_directory(path))
		{
			for(boost::filesystem::directory_iterator it = boost::filesystem::directory_iterator(path);
				it != boost::filesystem::directory_iterator();
				++it)
			{
				const boost::filesystem::path & item = *it;
				if(	boost::filesystem::is_regular_file(item) &&
					(item.extension() == ".js" || item.extension() == ".lua"))
				{
					struct FindModelByName
					{
						FindModelByName(const char * name) : name_(name)
						{
						}
						bool operator()(const Model * model) const
						{
							return strcmp(model->getName(), name_) == 0;
						}
					private:
						const char * name_;
					};
					std::string modelName = item.stem().c_str();
					if(std::find_if(models_.begin(), models_.end(), FindModelByName(modelName.c_str())) == models_.end())
						models_.push_back(new Model(modelName.c_str()));
				}	
			}
		}
	}
	catch (Sim::Exception & e)
	{
		printException(e);
	}
}

MathModelSFM::ModelBase & MathModelSFM::ModelBase::instance()
{
	static ModelBase modelBase;
	return modelBase;
}

size_t MathModelSFM::ModelBase::getSize() const
{
	return models_.size();
}

MathModelSFM::Model * MathModelSFM::ModelBase::getModel(size_t index)
{
	return models_[index];
}

MathModelSFM::Model * MathModelSFM::ModelBase::add(const char * name)
{
	return NULL;
}

size_t MathModelSFM::ModelBase::getSizeS()
{
	return MathModelSFM::ModelBase::instance().getSize();
}

ModelHandle	MathModelSFM::ModelBase::getModelS(size_t index)
{
	return MathModelSFM::ModelBase::instance().getModel(index);
}

ModelHandle MathModelSFM::ModelBase::addS(const char * name)
{
	return MathModelSFM::ModelBase::instance().add(name);
}

//-=Model=-

class MathModelSFM::Model::ChangeHandler : public DataModel::ChangeHandler
{
public:
	ChangeHandler(Model * model) : model_(model)
	{
	}
	virtual DataModel::ChangeHandler * clone() const
	{
		return new ChangeHandler(*this);
	}
	virtual void operator()(Value * item)
	{	
		model_->testBase_.updateTests();
	}
private:
	Model * model_;
};

MathModelSFM::Model::Model(const std::string & name) : name_(name), loaded_(false), testBase_(&this->parameters_)
{
	ñhangeHandler_ = new ChangeHandler(this);
}

MathModelSFM::Model::~Model()
{
	delete ñhangeHandler_;
}

const char * MathModelSFM::Model::getName() const
{
	return name_.c_str();
}

const char * MathModelSFM::Model::getType() const
{
	return "SFM";
}

bool MathModelSFM::Model::load()
{
	try
	{
		loadDescFile2(name_.c_str(), "SFM/Airplanes/", "airplane", parameters_.airplaneDesc);
		Sim::SFM::Airframe::Desc & airframeDesc = Sim::SFM:: Airframe::Database::instance().regDesc(parameters_.airplaneDesc.airframeId);
		loadDescFile2(parameters_.airplaneDesc.airframeId.c_str(), "SFM/Airframes/", "airframe", airframeDesc);
		Sim::SFM::JetEngine::Desc & jetEngineDesc = Sim::SFM::JetEngine::Database::instance().regDesc(parameters_.airplaneDesc.jetEngineId);
		loadDescFile2(parameters_.airplaneDesc.jetEngineId.c_str(), "SFM/JetEngines/", "jetEngine", jetEngineDesc);
		loaded_ = true;
		testBase_.initTests();
		return true;
	}
	catch (Sim::Exception & e)
	{
		printException(e);
		return false;
	}
}

void MathModelSFM::Model::unload()
{
	loaded_ = false;
}

bool MathModelSFM::Model::isLoaded() const
{
	return loaded_;
}

void MathModelSFM::Model::save()
{
	try
	{
		saveDescFile(name_, "SFM/Airplanes/", ".js", &Sim::saveJson, "airplane", parameters_.airplaneDesc);
		Sim::SFM::Airframe::Desc & airframeDesc = Sim::SFM:: Airframe::Database::instance().regDesc(parameters_.airplaneDesc.airframeId);
		saveDescFile(parameters_.airplaneDesc.airframeId, "SFM/Airframes/", ".js", &Sim::saveJson, "airframe", airframeDesc);
		Sim::SFM::JetEngine::Desc & jetEngineDesc = Sim::SFM::JetEngine::Database::instance().regDesc(parameters_.airplaneDesc.jetEngineId);
		saveDescFile(parameters_.airplaneDesc.jetEngineId, "SFM/JetEngines/", ".js", &Sim::saveJson, "jetEngine", jetEngineDesc);
	}
	catch (Sim::Exception & e)
	{
		printException(e);
	}
}

Value * MathModelSFM::Model::getParameters()
{
	return createValue(parameters_, ñhangeHandler_);
}

MathModelSFM::TestBase * MathModelSFM::Model::getTestBase()
{
	return &testBase_;
}

const char * MathModelSFM::Model::getName(ModelHandle handle)
{
	return ((Model*)(handle))->getName();
}

const char * MathModelSFM::Model::getType(ModelHandle handle)
{
	return ((Model*)(handle))->getType();
}

bool MathModelSFM::Model::load(ModelHandle handle)
{
	return ((Model*)(handle))->load();
}

void MathModelSFM::Model::unload(ModelHandle handle)
{
	((Model*)(handle))->unload();
}

bool MathModelSFM::Model::isLoaded(ModelHandle handle)
{
	return ((Model*)(handle))->isLoaded();
}

void MathModelSFM::Model::save(ModelHandle handle)
{
	((Model*)(handle))->save();
}

dmValueHandle MathModelSFM::Model::getParameters(ModelHandle handle)
{
	return ((Model*)(handle))->getParameters();
}

TestBaseHandle MathModelSFM::Model::getTestBase(ModelHandle handle)
{
	return ((Model*)(handle))->getTestBase();
}

extern "C"
{

__declspec(dllexport) void modelTestAPIInitialize(ModelTestAPI * modelTestAPI)
{
	Sim::VariantRoot variant;
	loadLua("Common/Config/Units.lua", "Units", variant);
	Sim::UnitManager::instance().load(variant);

	modelTestAPI->model.getName				= MathModelSFM::Model::getName;
	modelTestAPI->model.getType				= MathModelSFM::Model::getType;
	modelTestAPI->model.load				= MathModelSFM::Model::load;
	modelTestAPI->model.unload				= MathModelSFM::Model::unload;
	modelTestAPI->model.isLoaded			= MathModelSFM::Model::isLoaded;
	modelTestAPI->model.save				= MathModelSFM::Model::save;
	modelTestAPI->model.getParameters		= MathModelSFM::Model::getParameters;
	modelTestAPI->model.getTestBase			= MathModelSFM::Model::getTestBase;

	modelTestAPI->modelBase.getSize			= MathModelSFM::ModelBase::getSizeS;
	modelTestAPI->modelBase.getModel		= MathModelSFM::ModelBase::getModelS;
	modelTestAPI->modelBase.add				= MathModelSFM::ModelBase::addS;

	modelTestAPI->test.getName				= MathModelSFM::Test::getName;
	modelTestAPI->test.enable				= MathModelSFM::Test::enable;
	modelTestAPI->test.disable				= MathModelSFM::Test::disable;
	modelTestAPI->test.isEnabled			= MathModelSFM::Test::isEnabled;
	modelTestAPI->test.getParameters		= MathModelSFM::Test::getParameters;
	modelTestAPI->test.getResult			= MathModelSFM::Test::getResult;
	modelTestAPI->test.setResultChangeCallback = MathModelSFM::Test::setResultChangeCallback;

	modelTestAPI->testBase.getSize			= MathModelSFM::TestBase::getSize;
	modelTestAPI->testBase.getTest			= MathModelSFM::TestBase::getTest;

	initialize(&modelTestAPI->dataModelAPI);
}

__declspec(dllexport) void modelTestAPIRelease()
{

}

}