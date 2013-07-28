#include "stdafx.h"
#include <iostream>
#include <fstream>

#include "TestStorage.hpp"
#include "TestJetEngineProc.hpp"
#include "TestAirframeProc.hpp"
#include "TestAirplaneProc.hpp"

#include "Algorithm/Exception.hpp"
#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"
#include "Utils/Units.hpp"

int _tmain(int argc, const char * argv[])
{
	if(argc < 2)
		THROW_EXCEPTION(Sim::Exception, "Arguments missed!");

	{
		Sim::VariantRoot variant;
		Sim::loadLua("Config/Config.lua", "Units", variant);
		Sim::UnitManager::instance().load(variant);
	}

	PlotStorage::instance().reg("P",	&runTestP);
	PlotStorage::instance().reg("Ce",	&runTestCe);

	PlotStorage::instance().reg("LD",	&runTestLD);
	PlotStorage::instance().reg("AO",	&runTestAoAOptimal);
	PlotStorage::instance().reg("LDM",	&runTestMaxLD);
	PlotStorage::instance().reg("GM",	&runTestGlideM);
	PlotStorage::instance().reg("MxMy",	&runTestMxMy);
	PlotStorage::instance().reg("Mz",	&runTestMz);
	PlotStorage::instance().reg("MxOmegaX",	&runTestMXOmegaX);

	PlotStorage::instance().reg("TA",	&runTestTurnRate);
	PlotStorage::instance().reg("Vy",	&runTestMaxVy);
	PlotStorage::instance().reg("Ny",	&runTestMaxNy);
	PlotStorage::instance().reg("Nx",	&runTestMaxNx);
	PlotStorage::instance().reg("Pr",	&runTestThrustReq);
	PlotStorage::instance().reg("Cer",	&runTestConsumption);
	PlotStorage::instance().reg("R",	&runTestRange);
	PlotStorage::instance().reg("Vmin",	&runTestMinTAS);
	PlotStorage::instance().reg("Vmax",	&runTestMaxTAS);
	PlotStorage::instance().reg("Ve",	&runTestEnduranceTAS);
	PlotStorage::instance().reg("Vc",	&runTestCruiseTAS);
	PlotStorage::instance().reg("VyTAS",&runTestMaxVyTAS);

	try
	{
		PlotStorage::instance().performTest(argc - 1, argv + 1);
	}
	catch (const std::exception & e)
	{
		std::ofstream errorStream("error.log", std::ios_base::out | std::ios_base::app);
		std::cout << "Error: " << e.what();
		if(!errorStream.bad())
		{
			errorStream << "Error: " << e.what() << std::endl;
			errorStream.close();
		}
	}		
	return 0;
}
