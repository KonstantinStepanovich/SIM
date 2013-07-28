#include "MathModelTestSFM.hpp"

#include "Utils/Units.hpp"
#include "Physic/Gas.hpp"

#include "Modules/Satm1976/Satm1976.hpp"
#include "TestAirframe.hpp"
#include "TestAirplane.hpp"

#include "Value.hpp"
#include "Limits.hpp"

using namespace MathModelSFM;
using namespace DataModel;
using namespace Sim;

static const float vStepCoeff = 1.0f / 5.0f;

static const float altStepCoeff = vStepCoeff;

void createIterationTAS(const Airframe::Desc & airframeDesc, float altitude,
						IterationRange<float> & TAS)
{
	const float altitudeKm = altitude * 1.0E-3f;
	unsigned int level = Satm1976::getLevelByAltKm(altitudeKm);
	Satm1976::AtmParam atmParam;
	Satm1976::getAtmParamByAltKm(altitudeKm, level, atmParam);

	read(airframeDesc.force.table, TAS);
	scale(TAS, (float)(getSoundVel(Satm1976::T0 * atmParam.Tr)));
	TAS.step *= vStepCoeff;
}

ValueTable * createValuePoint(	Point & point,
								const char * captionX, const char * captionY,
								ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable(captionX, createValue(point.x, changeHandler)),
		ItemTable(captionY, createValue(point.y, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueArray * createValuePoints(	Points & points,
								const char * captionX, const char * captionY,
								ChangeHandler * changeHandler)
{
	std::vector<Value*> values;
	values.resize(points.size());
	for(Points::iterator itP = points.begin();
		itP != points.end();
		++itP)
	{
		values[itP - points.begin()] = createValuePoint(*itP, captionX, captionY, changeHandler);
	}
	return new ValueArray(values.begin(), values.end(), ranIter, changeHandler);
}

//-=TestVyMax=-

MathModelSFM::TestVyMax::TestVyMax(ModelParameters * modelParameters, TestBase * testBase) : Test(modelParameters, testBase)
{
	parameters_.altitude = 0.0f;
}

const char * MathModelSFM::TestVyMax::getName() const
{
	return "VyMax";
}

static void scaleAirVel(Curve & curve, double altitude, AirSpeedType parameterAS)
{
	const float altitudeKm = altitude * 1.0E-3f;
	unsigned int level = Satm1976::getLevelByAltKm(altitudeKm);
	Satm1976::AtmParam atmParam;
	Satm1976::getAtmParamByAltKm(altitudeKm, level, atmParam);

	switch(parameterAS)
	{
		case PARAMETER_M:
			scaleX(curve, 1.0 / getSoundVel(Satm1976::T0 * atmParam.Tr));
			break;
		case PARAMETER_TAS:
			break;
		case PARAMETER_IAS:
			scaleX(curve, sqrt(atmParam.rhor));
			break;
	}
}

void MathModelSFM::TestVyMax::update()
{
	IterationRange<float> TAS;
	createIterationTAS(	Airframe::Database::instance().getDesc(modelParameters_->airplaneDesc.airframeId),
						parameters_.altitude, TAS);
	
	result_.curve.clear();
	FuncVy funcVy(modelParameters_->airplaneDesc, commonParameters_.state);
	iterateCurve(TAS, std::bind1st(FuncAltTAS<FuncVy>(funcVy), parameters_.altitude * 1.0E-3), result_.curve, true, std::bind2nd(std::greater<FuncVy::result_type>(), 0.0));
	scaleAirVel(result_.curve, parameters_.altitude, commonParameters_.airSpeedType);
	result_.airSpeedType = commonParameters_.airSpeedType;
	
	notifyResultChanged(NULL, 0);
}

ValueTable * createValue(CurveVyMax & vyMaxPoints, ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable(	"VyMax(V)",
					createValuePoints(	vyMaxPoints.curve,
										"V",
										"VyMax",
										changeHandler)),
		ItemTable(	"air speed type",
					createValueInteger(vyMaxPoints.airSpeedType, changeHandler ))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * MathModelSFM::TestVyMax::getResult()
{
	return createValue(result_, ñhangeHandler);
}

ValueTable * createValue(ParametersAlt & parametersAlt, ChangeHandler * changeHandler)
{
	return new ValueTable(ItemTable("altitude", createValue(parametersAlt.altitude, changeHandler)), changeHandler);
}

Value * MathModelSFM::TestVyMax::getSpecificParameters_()
{
	return createValue(parameters_, ñhangeHandler);
}

//-=TestVminVmax=-

MathModelSFM::TestVminVmax::TestVminVmax(ModelParameters * modelParameters, TestBase * testBase) : Test(modelParameters, testBase)
{
}

const char * MathModelSFM::TestVminVmax::getName() const
{
	return "VminVmax";
}

static void scaleAirVel(Point & point, AirSpeedType parameterAS)
{
	const float altitudeKm = point.y * 1.0E-3f;
	unsigned int level = Satm1976::getLevelByAltKm(altitudeKm);
	Satm1976::AtmParam atmParam;
	Satm1976::getAtmParamByAltKm(altitudeKm, level, atmParam);

	switch(parameterAS)
	{
		case PARAMETER_M:
			point.x = point.x / getSoundVel(Satm1976::T0 * atmParam.Tr);
			break;
		case PARAMETER_TAS:
			break;
		case PARAMETER_IAS:
			point.x = point.x * sqrt(atmParam.rhor / Satm1976::rho0);
			break;
	}
}

static void scaleAirVel(Curve & curve, AirSpeedType parameterAS)
{
	for(Curve::iterator itC = curve.begin();
		itC != curve.end();
		++itC)
		scaleAirVel(*itC, parameterAS);
}

void MathModelSFM::TestVminVmax::update()
{
	IterationRange<float> altitude;
	JetEngine::Desc & jetEngineDesc = JetEngine::Database::instance().regDesc(modelParameters_->airplaneDesc.jetEngineId);
	readRow(jetEngineDesc.table, altitude);
	scale(altitude, 1.0E3f);
	altitude.step *= altStepCoeff;

	result_.vMin.clear();
	TasMin funcTasMin(modelParameters_->airplaneDesc, commonParameters_.state);
	iterateCurve(altitude, FuncAlt<TasMin>(funcTasMin), result_.vMin, false, std::bind2nd(std::greater<FuncVy::result_type>(), 0.0));
	scaleAirVel(result_.vMin, commonParameters_.airSpeedType);

	result_.vMax.clear();
	TasMax funcTasMax(modelParameters_->airplaneDesc, commonParameters_.state);
	iterateCurve(altitude, FuncAlt<TasMax>(funcTasMax), result_.vMax, false, std::bind2nd(std::greater<FuncVy::result_type>(), 0.0));
	scaleAirVel(result_.vMin, commonParameters_.airSpeedType);

	result_.airSpeedType = commonParameters_.airSpeedType;
	
	notifyResultChanged(NULL, 0);
}

ValueTable * createValue(CurveVminVmax & pointsVminVmax, ChangeHandler * ñhangeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("Vmin",			createValuePoints(pointsVminVmax.vMin, "V", "Vmin", ñhangeHandler)),
		ItemTable("Vmax",			createValuePoints(pointsVminVmax.vMax, "V", "Vmax", ñhangeHandler)),
		ItemTable("air speed type",	createValueInteger(pointsVminVmax.airSpeedType, ñhangeHandler))
	};
	return new ValueTable(itemTable, ñhangeHandler);
}

Value * MathModelSFM::TestVminVmax::getResult()
{
	return createValue(result_, ñhangeHandler);
}

//-=TestTurnRate=-

MathModelSFM::TestTurnRate::TestTurnRate(ModelParameters * modelParameters, TestBase * testBase) : Test(modelParameters, testBase)
{
	parameters_.altitude.push_back(0.0f);
	parameters_.altitude.push_back(2000.0f);
	parameters_.altitude.push_back(5000.0f);
	parameters_.altitude.push_back(10000.0f);
}

const char * MathModelSFM::TestTurnRate::getName() const
{
	return "TurnRate";
}

static void scaleAirVel(Curves & curves, AirSpeedType parameterAS)
{
	for(Curves::iterator itC = curves.begin();
		itC != curves.end();
		++itC)
		scaleAirVel(itC->curve, parameterAS);
}

void MathModelSFM::TestTurnRate::update()
{
	IterationRange<float> TAS;
	createIterationTAS(	Airframe::Database::instance().getDesc(modelParameters_->airplaneDesc.airframeId),
						parameters_.altitude.front(), TAS);
	
	result_.curves.clear();
	FuncInstTurnNyMax funcInstTurnNyMax(modelParameters_->airplaneDesc, commonParameters_.state);
	iterateCurves(parameters_.altitude, TAS, FuncAltTAS<FuncInstTurnNyMax>(funcInstTurnNyMax), result_.curves, true, std::bind2nd(std::greater<FuncInstTurnNyMax::result_type>(), 0.0));
	scaleAirVel(result_.curves, commonParameters_.airSpeedType);
	result_.airSpeedType = commonParameters_.airSpeedType;

	notifyResultChanged(NULL, 0);
}

ValueTable * createValueCurve(CurveByParam & curveByParam,
							  const char * captionParameter, const char * captionPoints,
							  const char * captionX, const char * captionY,
							  ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable(captionParameter, createValue(curveByParam.parameter, changeHandler)),
		ItemTable(captionPoints,	createValuePoints(curveByParam.curve, captionX, captionY, changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueArray * createValueCurves(Curves & curves,
							   const char * captionParameter, const char * captionPoints,
							   const char * captionX, const char * captionY,
							   ChangeHandler * changeHandler)
{
	std::vector<Value*> values;
	values.resize(curves.size());
	for(Curves::iterator itC = curves.begin();
		itC != curves.end();
		++itC)
	{
		values[itC - curves.begin()] = createValueCurve(	*itC,
															captionParameter, captionPoints,
															captionX, captionY, changeHandler);
	}
	return new ValueArray(values.begin(), values.end(), changeHandler);
}

ValueTable * createValue(CurvesTurnRate & turnRateCurves, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable(	"Ny(H,V)",
					createValueCurves(	turnRateCurves.curves,
										"H",
										"Ny(V)",
										"V",
										"Ny",
										changeHandler)),
		ItemTable("air speed type",	createValueInteger(turnRateCurves.airSpeedType, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * MathModelSFM::TestTurnRate::getResult()
{
	return createValue(result_, ñhangeHandler);
}

ValueTable * createValue(ParametersAltArray & parametersAltArray, ChangeHandler * changeHandler)
{
	return new ValueTable(	ItemTable(	"altitude",
										createValue(parametersAltArray.altitude,
													changeHandler)),
							changeHandler);
}

Value * MathModelSFM::TestTurnRate::getSpecificParameters_()
{
	return createValue(parameters_, ñhangeHandler);
}

ValueTable * createValue(CurveAoA & ñurveAoA, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable(	"AoA(V)",
					createValuePoints(	ñurveAoA.curve,
										"V",
										"AoA",
										changeHandler)),
		ItemTable( "air speed type",
					createValueInteger(ñurveAoA.airSpeedType, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

ValueTable * createValue(ParametersAoA & parametersAoA, ChangeHandler * changeHandler)
{
	ItemTable itemTable[] = {
		ItemTable("altitude",	createValue(parametersAoA.altitude, changeHandler)),
		ItemTable("AoA0",		createValue(parametersAoA.AoA0, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

TestAoA::TestAoA(ModelParameters * modelParameters, MathModelSFM::TestBase * testBase) : Test(modelParameters, testBase)
{
	parameters_.altitude = 0.0f;
	parameters_.AoA0 = 0.0;
}

const char * TestAoA::getName() const
{
	return "AoA";
}

void TestAoA::update()
{
	IterationRange<float> TAS;
	createIterationTAS(	Airframe::Database::instance().getDesc(modelParameters_->airplaneDesc.airframeId),
						parameters_.altitude, TAS);

	result_.curve.clear();
	FuncAoA funcAoA(modelParameters_->airplaneDesc, commonParameters_.state);
	iterateCurve(TAS, std::bind1st(FuncAltTAS<FuncAoA>(funcAoA), parameters_.altitude), result_.curve, true, std::bind2nd(std::greater<FuncVy::result_type>(), 0.0));
	scaleAirVel(result_.curve, parameters_.altitude, commonParameters_.airSpeedType);
	shiftY(result_.curve, parameters_.AoA0);
	result_.airSpeedType = commonParameters_.airSpeedType;

	notifyResultChanged(NULL, 0);
}

Value * MathModelSFM::TestAoA::getResult()
{
	return createValue(result_, ñhangeHandler);
}

Value * MathModelSFM::TestAoA::getSpecificParameters_()
{
	return createValue(parameters_, ñhangeHandler);
}

TestMxMy::TestMxMy(ModelParameters * modelParameters, MathModelSFM::TestBase * testBase) : Test(modelParameters, testBase)
{
	parameters_.aileron = 0.0f;
	parameters_.rudder = 0.0f;
	parameters_.bettaStep = 3.14159 / 100.0f;
	parameters_.Cy = 1.0f;
}

const char * TestMxMy::getName() const
{
	return "MxMy";
}

void TestMxMy::update()
{
	const Airframe::Desc & airframeDesc = Airframe::Database::instance().getDesc(modelParameters_->airplaneDesc.airframeId);
	IterationRange<float> AoS;
	AoS.start = -1.57f;
	AoS.end = 1.57f;
	AoS.step = parameters_.bettaStep;

	commonParameters_.state.airframe.torque.aileron = parameters_.aileron;
	commonParameters_.state.airframe.torque.rudder = parameters_.rudder;
	result_.Mx.clear();
	iterateCurve(AoS, FuncMxAoS(airframeDesc.torque.MxMy, commonParameters_.state.airframe.torque, parameters_.Cy), result_.Mx, true, PredTrue<float>());
	result_.My.clear();
	iterateCurve(AoS, FuncMyAoS(airframeDesc.torque.MxMy, commonParameters_.state.airframe.torque, parameters_.Cy), result_.My, true, PredTrue<float>());

	notifyResultChanged(NULL, 0);
}

ValueTable * createValue(CurvesMxMy & curvesMxMy, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("Mx", createValuePoints(curvesMxMy.Mx, "AoS", "Mx", changeHandler)),
		ItemTable("My", createValuePoints(curvesMxMy.My, "AoS", "My", changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * TestMxMy::getResult()
{
	return createValue(result_, ñhangeHandler);
}

ValueTable * createValue(ParametersMxMy & parametersMxMy, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("aileron",	createValue(parametersMxMy.aileron,		changeHandler, &limitsPercent)),
		ItemTable("rudder",		createValue(parametersMxMy.rudder,		changeHandler, &limitsPercent)),
		ItemTable("AoS step",	createValue(parametersMxMy.bettaStep,	changeHandler)),
		ItemTable("Cy",			createValue(parametersMxMy.Cy,			changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * TestMxMy::getSpecificParameters_()
{
	return createValue(parameters_, ñhangeHandler);
}

TestOmegaX::TestOmegaX(ModelParameters * modelParameter, MathModelSFM::TestBase * testBase) : Test(modelParameter, testBase)
{
	parameters_.altitude = 0.0f;
	parameters_.AoA = 0.0f;
	parameters_.Cy = 1.2f;
}

const char * TestOmegaX::getName() const
{
	return "OmegaX";
}

void TestOmegaX::update()
{
	const Airframe::Desc & desc = Airframe::Database::instance().getDesc(modelParameters_->airplaneDesc.airframeId);
	IterationRange<float> TAS;
	createIterationTAS(desc, parameters_.altitude, TAS);
	
	result_.curve.clear();
	iterateCurve(TAS, FuncOmegaX(desc.torque, parameters_.AoA, 0.0, parameters_.Cy), result_.curve, true, PredTrue<float>());
	scaleAirVel(result_.curve, parameters_.altitude, commonParameters_.airSpeedType);
	result_.airSpeedType = commonParameters_.airSpeedType;

	notifyResultChanged(NULL, 0);
}

ValueTable * createValue(CurveOmegaX & curvesMxMy, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("Mx",				createValuePoints(curvesMxMy.curve, "AoS", "Mx", changeHandler)),
		ItemTable("air speed type",	createValueInteger(curvesMxMy.airSpeedType, changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * TestOmegaX::getResult()
{
	return createValue(result_, ñhangeHandler);
}

ValueTable * createValue(ParametersOmegaX & parametersOmegaX, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("altitude",	createValue(parametersOmegaX.altitude,	changeHandler)),
		ItemTable("AoA",		createValue(parametersOmegaX.AoA,		changeHandler)),
		ItemTable("Cy",			createValue(parametersOmegaX.Cy,		changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * TestOmegaX::getSpecificParameters_()
{
	return createValue(parameters_, ñhangeHandler);
}

TestMz::TestMz(ModelParameters * modelParameters, MathModelSFM::TestBase * testBase) : Test(modelParameters, testBase)
{
	parameters_.elevator = 0.0f;
	parameters_.AoAStep = 3.14159 / 100.0f;
}

const char * TestMz::getName() const
{
	return "Mz";
}

void TestMz::update()
{
	const Airframe::Desc & airframeDesc = Airframe::Database::instance().getDesc(modelParameters_->airplaneDesc.airframeId);
	IterationRange<float> AoA;
	AoA.start = -1.57;
	AoA.end = 1.57f;
	AoA.step = parameters_.AoAStep;
	
	result_.Mz.clear();
	commonParameters_.state.airframe.torque.elevator = parameters_.elevator;
	iterateCurve(AoA, FuncMzAoA(airframeDesc.torque.Mz, commonParameters_.state.airframe.torque), result_.Mz, true, PredTrue<float>());
	
	notifyResultChanged(NULL, 0);
}

ValueTable * createValue(CurveMz & curveMz, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("Mz",			createValuePoints(curveMz.Mz, "AoA", "Mz", changeHandler)),
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * TestMz::getResult()
{
	return createValue(result_, ñhangeHandler);
}

ValueTable * createValue(ParametersMz & parametersMz, ChangeHandler * changeHandler)
{
	const ItemTable itemTable[] = {
		ItemTable("elevator",	createValue(parametersMz.elevator,		changeHandler, &limitsPercent)),
		ItemTable("AoA step",	createValue(parametersMz.AoAStep,		changeHandler))
	};
	return new ValueTable(itemTable, changeHandler);
}

Value * TestMz::getSpecificParameters_()
{
	return createValue(parameters_, ñhangeHandler);
}

//-=MathModelSFM=-

void MathModelSFM::TestBase::initTests()
{
	tests_[0] = new TestVyMax(modelParameters_, this);
	tests_[1] = new TestVminVmax(modelParameters_, this);
	tests_[2] = new TestTurnRate(modelParameters_, this);
	tests_[3] = new TestAoA(modelParameters_, this);
	tests_[4] = new TestMxMy(modelParameters_, this);
	tests_[5] = new TestOmegaX(modelParameters_, this);
	tests_[6] = new TestMz(modelParameters_, this);
}
