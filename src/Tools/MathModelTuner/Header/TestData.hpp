#ifndef _TestData_hpp_
#define _TestData_hpp_

#include "Variant.hpp"

struct TestData
{
	Variant * testParametersRoot;
	Variant * referenceRoot;
	const char * testName;
	int referenceIndex;
	bool showReference;
	bool slaveToReference;
};

#endif _TestData_hpp_