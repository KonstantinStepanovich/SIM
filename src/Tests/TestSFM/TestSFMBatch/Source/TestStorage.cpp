#include "stdafx.h"

#include "TestStorage.hpp"

#include <assert.h>
#include "String/StringStream.hpp"

PlotStorage::PlotStorage()
{
}

void PlotStorage::reg(const char * testName, Test testIn)
{
	Test & test = plots_[testName];
	assert(test == NULL);
	test = testIn;
}

void PlotStorage::performTest(int argc, const char ** argv)
{
	if(argc < 1)
		THROW_EXCEPTION(Sim::Exception, "Test name missed!");
	const char * testName = argv[0];
	Plots::const_iterator itT = plots_.find(testName);
	if(itT != plots_.end())
		itT->second(argc - 1, argv + 1);
	else
		THROW_EXCEPTION(Sim::Exception, OSS("Test " << testName << " not found!"));
}

PlotStorage & PlotStorage::instance()
{
	static PlotStorage testStorage;
	return testStorage;
}
