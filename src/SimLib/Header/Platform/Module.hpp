#ifndef _Module_hpp_
#define _Module_hpp_

#ifdef WIN32

#include "Windows.h"

#endif

namespace Sim
{

#ifdef WIN32

typedef ::HMODULE Module;

typedef ::FARPROC Function;

#endif

Module loadModule(const char * name);

void freeModule(Module module);

Function getFunction(Module module, const char * name);

template<typename Func>
Func getFunction(Module module, const char * name)
{
	return (Func)getFunction(module, name);
}

}

#endif _Module_hpp_