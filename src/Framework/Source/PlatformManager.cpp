#include "stdafx.h"
#include "PlatformManager.hpp"
#include "EnvironmentManager.hpp"
#include "ISystem.hpp"

#include "String/StringStream.hpp"
#include "Algorithm/Exception.hpp"

using namespace Sim::Framework;

#ifdef WIN32

#include <windows.h>

static LARGE_INTEGER frequency = {0, 0};

BOOL getSystemTime(Time & time)
{
	LARGE_INTEGER counts;
	time_t sec;
	if(frequency.QuadPart == 0)
	{
		if(QueryPerformanceFrequency(&frequency) == FALSE)
		{
			return FALSE;
		}
	}
	if(QueryPerformanceCounter(&counts) == FALSE)
	{
		return FALSE;
	}
	sec = counts.QuadPart / frequency.QuadPart;
	time = sec * 1000000000 + ((counts.QuadPart - sec * frequency.QuadPart) * 1000000000) / frequency.QuadPart;
	return TRUE;
}


#else

#include <time.h>

BOOL getSystemTime(Time & time)
{
	timespec tsp;
	if(clock_gettime(CLOCK_REALTIME, &tsp) == 0)
	{
		st = tsp.sec * 1000000000 + tsp.nanosec;
		return TRUE;
	}
	return FALSE;
}

#endif

PlatformManager::PlatformManager()
{
}

PlatformManager::~PlatformManager()
{
	while(!systemLibs_.empty())
	{
		unloadSystemLibrary(systemLibs_.begin()->first);
	}
}

ISystem * PlatformManager::loadSystemLibrary(const char * name, const char * libName)
{
	HMODULE module = LoadLibraryA(libName);
	if(module == NULL)
		THROW_EXCEPTION(Exception, OSS("Can't load module \"" << libName << "\""));
	CreateSystem createSystem = (CreateSystem)GetProcAddress(module, "createSystem");
	if(createSystem == NULL)
		THROW_EXCEPTION(Exception, OSS("Can't find \"createSystem\" function in module \"" << libName << "\""));
	Managers managers = { &EnvironmentManager::instance() };
	ISystem * system = createSystem(name, &managers);
	Library library;
	library.name = libName;
	library.module = module;
	systemLibs_[system] = library;
	return system;
}

void PlatformManager::unloadSystemLibrary(ISystem* system)
{
	SystemLibs::iterator itSL = systemLibs_.find(system);
	if(itSL != systemLibs_.end())
	{
		const Library & library = itSL->second;
		DestroySystem destroySystem = (DestroySystem)GetProcAddress(library.module, "destroySystem");
		if(destroySystem == NULL)
			THROW_EXCEPTION(Exception, OSS("Can't find \"destroySystem\" function in module \"" << library.name << "\""));
		destroySystem(system);
		FreeLibrary(library.module);
		itSL = systemLibs_.erase(itSL);
	}
}

void PlatformManager::processMessages()
{
	MSG msg;
	while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

Time PlatformManager::getSystemTime()
{
	Time time;
	::getSystemTime(time);
	return time;
}

PlatformManager & PlatformManager::instance()
{
	static PlatformManager platformManager;
	return platformManager;
}