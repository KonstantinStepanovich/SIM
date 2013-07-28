#ifndef _TestStorage_hpp_
#define _TestStorage_hpp_

#include <map>

#include "Algorithm/Exception.hpp"

typedef void(*Test)(int argc, const char ** argv);

class PlotStorage
{
public:
	void reg(const char * testName, Test test);
	void performTest(int argc, const char ** argv);
	static PlotStorage & instance();
private:
	PlotStorage();
	typedef std::map<std::string, Test> Plots;
	Plots plots_;
};

#endif