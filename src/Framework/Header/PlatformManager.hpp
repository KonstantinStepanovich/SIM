#ifndef _PlatformManager_hpp_
#define _PlatformManager_hpp_

#include "Time.hpp"

#include <map>
#include <Windows.h>

namespace Sim
{

namespace Framework
{

class ISystem;
class ISystemLinker;

class PlatformManager
{
public:
	~PlatformManager();
	ISystem * loadSystemLibrary(const char * name, const char * libName);
	void unloadSystemLibrary(ISystem * system);
	void processMessages();
	Time getSystemTime();
	static PlatformManager & instance();
private:
	PlatformManager();
	typedef HMODULE Module;
	struct Library
	{
		const char * name;
		HMODULE module;
	};
	typedef std::map<ISystem*, Library> SystemLibs;
	SystemLibs systemLibs_;
};

}

}

#endif _PlatformManager_hpp_