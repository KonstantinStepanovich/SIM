#include "Platform/Module.hpp"

using namespace Sim;

#ifdef WIN32

Module Sim::loadModule(const char * name)
{
	return LoadLibraryA(name);
}

void Sim::freeModule(Module module)
{
	FreeLibrary(module);
}

Function Sim::getFunction(Module module, const char * name)
{
	return GetProcAddress(module, name);
}

#endif