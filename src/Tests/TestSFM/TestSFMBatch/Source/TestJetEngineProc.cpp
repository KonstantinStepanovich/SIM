#include "stdafx.h"

#include "TestJetEngine.hpp"
#include "ParamIteration.hpp"

#include <stdio.h>
#include <iostream>

#include "Algorithm/Exception.hpp"
#include "Database/DatabaseUtil.hpp"
#include "Database/ParserLua.hpp"

#include "Modules/SFM/Model/JetEngine.hpp"

using namespace Sim;
using namespace Sim::SFM;

static void read(int argc, const char ** argv, TestJetEngine & test)
{
	if(argc < 4)
		THROW_EXCEPTION(Exception, "parameters missed");
	
	const char * typeId = argv[0];
	loadDescFile(typeId, "../../SFM/JetEngines/", "lua", &loadLua, "", test.desc);

	readRow(test.desc.table, test.alt);
	sscanf(argv[1], "%f", &test.alt.step);

	readCol(test.desc.table, test.M);
	sscanf(argv[2], "%f", &test.M.step);

	sscanf(argv[3], "%f", &test.env.dT);
	test.state.afterburner = argc > 4 && std::string(argv[4])== "on";
}

static void performTestP(const TestP & test)
{
	FuncPmax funcPmax(test.desc, test.state);
	iteratePrint2(test.alt, test.M, FuncAltM<FuncPmax>(test.env, funcPmax));
}

void runTestP(int argc, const char ** argv)
{
	TestP pTest;
	read(argc, argv, pTest);
	performTestP(pTest);
}

static void performTestCe(const TestCe & test)
{
	FuncCe funcCe(test.desc, test.state);
	iteratePrint2(test.alt, test.M, FuncAltM<FuncCe>(test.env, funcCe));
}

void runTestCe(int argc, const char ** argv)
{
	TestCe ceTest;
	read(argc, argv, ceTest);
	performTestCe(ceTest);
}