#include "stdafx.h"

#include "ParamIteration.hpp"
#include "TestAirplane.hpp"

#include <stdio.h>
#include <iostream>

#include "Algorithm/Exception.hpp"
#include "Math/Scalar.hpp"
#include "Database/DatabaseUtil.hpp"
#include "Database/ParserLua.hpp"
#include "Physic/Gas.hpp"

#include "Modules/SFM/Model/Airplane.hpp"
#include "Modules/SFM/Model/JetEngine.hpp"
#include "Modules/Satm1976/Satm1976.hpp"

using namespace Sim;
using namespace Sim::SFM;

static void read(int argc, const char ** argv, const Airplane::Desc & desc, Airplane::State & state)
{
	float fuelRatio;
	sscanf(argv[0], "%f", &fuelRatio);	
	state.fuelSystem.internalFuel = fuelRatio * desc.fuelSystem.internalFuelMax;
	state.fuelSystem.externalFuel = 0.0f;
	sscanf(argv[1], "%f", &state.payloadMass);
}

static void read(int argc, const char ** argv, TestAirplane & test)
{
	if(argc < 5)
		THROW_EXCEPTION(Exception, "parameters missed");

	const char * airplaneName = argv[0];
	loadDescFile(airplaneName, "../../SFM/Airplanes/", "lua", &loadLua, "", test.desc);
	Airframe::Desc & airframeDesc = Airframe::Database::instance().regDesc(test.desc.airframeId);
	loadDescFile(test.desc.airframeId, "../../SFM/Airframes/", "lua", &loadLua, "", airframeDesc);
	JetEngine::Desc & jetEngineDesc = JetEngine::Database::instance().regDesc(test.desc.jetEngineId);
	loadDescFile(test.desc.jetEngineId, "../../SFM/JetEngines/", "lua", &loadLua, "", jetEngineDesc);

	readRow(jetEngineDesc.table, test.altitude);
	sscanf(argv[1], "%f", &test.altitude.step);

	read(airframeDesc.force.table, test.TAS);
	sscanf(argv[2], "%f", &test.TAS.step);

	read(argc - 3, argv + 3, test.desc, test.state);
	test.state.jetEngine.afterburner = argc > 5 && std::string(argv[5])== "on";
}

template<typename T>
struct PrintValid
{
	inline void operator()(const T & param) const
	{
		if(param != Math::invalid<T>())
			PrintValue<T>()(param);
		else
			std::cout << "N/A    , ";
	}
};

static void performTestTurnRate(const TestTurnRate & test)
{
	FuncInstTurnRateMax funcTurnRate(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncInstTurnRateMax>(funcTurnRate), PrintValid<float>());
}

void runTestTurnRate(int argc, const char ** argv)
{
	TestTurnRate test;
	read(argc, argv, test);
	performTestTurnRate(test);
}

template<typename T>
struct PrintPositive
{
	inline void operator()(T val) const
	{
		if(val > 0.0f)
			PrintValue<T>()(val);
		else
			std::cout << "N/A    , ";
	}
};

static void performTestMaxVy(const TestVyMax & test)
{
	FuncVy funcVy(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncVy>(funcVy), PrintPositive<float>());
}

void runTestMaxVy(int argc, const char ** argv)
{
	TestTurnRate test;
	read(argc, argv, test);
	performTestMaxVy(test);
}

static void performTestMaxNy(const TestMaxNy & test)
{
	FuncNyMax funcMaxNy(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncNyMax>(funcMaxNy), PrintPositive<float>());
}

void runTestMaxNy(int argc, const char ** argv)
{
	TestMaxNy test;
	read(argc, argv, test);
	performTestMaxNy(test);
}

static void performTestThrustReq(const TestMaxNy & test)
{
	struct PrintThrust
	{
		inline void operator()(const Airplane::Thrust & thrust) const
		{
			if(!Math::isInvalid(thrust.abs))
				PrintValue<float>()(thrust.abs);
			else
				std::cout << "N/A    , ";
		}
	};
	FuncThrust funcThrust(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncThrust>(funcThrust), PrintThrust());
}

void runTestThrustReq(int argc, const char ** argv)
{
	TestThrustReq test;
	read(argc, argv, test);
	performTestThrustReq(test);
}

static void performTestConsumption(const TestConsumption & test)
{
	FuncConsumption funcConsumption(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncConsumption>(funcConsumption), PrintValid<float>());
}

void runTestConsumption(int argc, const char ** argv)
{
	TestConsumption test;
	read(argc, argv, test);
	performTestConsumption(test);
}

static void performTestRange(const TestRange & test)
{
	FuncRange funcRange(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncRange>(funcRange), PrintValid<float>());
}

void runTestRange(int argc, const char ** argv)
{
	TestRange test;
	read(argc, argv, test);
	performTestRange(test);
}

static void performTestMaxNx(const TestVyMax & test)
{
	FuncMaxNx funcMaxNx(test.desc, test.state);
	iteratePrint2(test.altitude, test.TAS, FuncAltTAS<FuncMaxNx>(funcMaxNx), PrintPositive<float>());
}

void runTestMaxNx(int argc, const char ** argv)
{
	TestMaxNx test;
	read(argc, argv, test);
	performTestMaxNx(test);
}

static void read(int argc, const char ** argv, TestTAS & test)
{
	if(argc < 4)
		THROW_EXCEPTION(Exception, "parameters missed");

	const char * airplaneName = argv[0];
	loadDescFile(airplaneName, "../../SFM/Airplanes/", "lua", &loadLua, "", test.desc);
	loadDescFile(test.desc.airframeId, "../../SFM/Airframes/", "lua", &loadLua, "",
					Airframe::Database::instance().regDesc(test.desc.airframeId));
	JetEngine::Desc & jetEngineDesc = JetEngine::Database::instance().regDesc(test.desc.jetEngineId);
	loadDescFile(test.desc.jetEngineId, "../../SFM/JetEngines/", "lua", &loadLua, "", jetEngineDesc);
	
	readRow(jetEngineDesc.table, test.alt);
	sscanf(argv[1], "%f", &test.alt.step);

	read(argc - 2, argv + 2, test.desc, test.state);
	test.state.jetEngine.afterburner = argc > 4 && std::string(argv[4])== "on";
}

static void performTestMinTAS(const TestTASMin & test)
{
	iteratePrint1(test.alt, FuncAlt<TasMin>(TasMin(test.desc, test.state)));
}

void runTestMinTAS(int argc, const char ** argv)
{
	TestTASMin test;
	read(argc, argv, test);
	performTestMinTAS(test);
}

static void performTestMaxTAS(const TestTASMax & test)
{
	iteratePrint1(test.alt, FuncAlt<TasMax>(TasMax(test.desc, test.state)));
}

void runTestMaxTAS(int argc, const char ** argv)
{
	TestTASMin test;
	read(argc, argv, test);
	performTestMaxTAS(test);
}

static void performTestEnduranceTAS(const TestTASEndurance & test)
{
	FuncCruiseSpeed funcCruiseSpeed(test.desc, test.state);
	iteratePrint1(test.alt, FuncAlt<FuncCruiseSpeed>(funcCruiseSpeed));
}

void runTestEnduranceTAS(int argc, const char ** argv)
{
	TestTASEndurance test;
	read(argc, argv, test);
	performTestEnduranceTAS(test);
}

static void performTestCruiseTAS(const TestTASCruise & test)
{
	FuncCruiseSpeed funcCruiseSpeed(test.desc, test.state);
	iteratePrint1(test.alt, FuncAlt<FuncCruiseSpeed>(funcCruiseSpeed));
}

void runTestCruiseTAS(int argc, const char ** argv)
{
	TestTASCruise test;
	read(argc, argv, test);
	performTestCruiseTAS(test);
}

static void performTestMaxVyTAS(const TestMaxVyTAS & test)
{
	FuncMaxVyTAS funcMaxVyTAS(test.desc, test.state);
	iteratePrint1(test.alt, FuncAlt<FuncMaxVyTAS>(funcMaxVyTAS));
}

void runTestMaxVyTAS(int argc, const char ** argv)
{
	TestMaxVyTAS test;
	read(argc, argv, test);
	performTestMaxVyTAS(test);
}
