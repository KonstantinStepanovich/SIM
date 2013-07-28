#ifndef _FrameworkAPI_hpp_
#define _FrameworkAPI_hpp_

struct SystemData
{
	const char * name;
	const char * libName;
};

extern void executeScenery(	const struct SystemData * systemData,
							size_t systemCount,
							const char * sceneName);

#endif _FrameworkAPI_hpp_