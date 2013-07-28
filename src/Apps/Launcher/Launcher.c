#include <stdio.h>
#include <tchar.h>

#include "FrameworkAPI.hpp"

enum ErrorCode
{
	ERROR_CODE_OK = 0,
	ERROR_CODE_NO_SYSTEMS = 1,
	ERROR_CODE_NO_PARAMETERS = 2,
	ERROR_CODE_MAX
};

int min(int a, int b)
{
	return a < b ? a : b;
}

int _tmain(int argc, const char * argv[])
{
	int argn = 1;

	const size_t maxSceneryLength = 1024;
	char scenery[1024];
	
	const size_t maxSystemCount = 128;
	struct SystemData systemData[128];
	size_t systemCount = 0;

	const size_t systemsSize = 1024;
	char systemsBuff[1024];
	char * system = systemsBuff;

	if(argc < 2)
	{
		printf("Error: no parameters!\n");
		return ERROR_CODE_NO_PARAMETERS;
	}

	scenery[0] = '\0';
	if(_snscanf(argv[argn], min(strlen(argv[argn]), maxSceneryLength), "/scenery=%s", scenery) == 1)
		argn++;

	while(argn < argc)
	{
		if(_snscanf(argv[argn], min(strlen(argv[argn]), systemsSize - (system - systemsBuff)), "/system=%s", system) == 1)
		{
			if(systemCount < maxSystemCount)
			{
				systemData[systemCount].name = system;
				systemData[systemCount].libName = system;
				systemCount++;
				system += strlen(system) + 1;
			}
			else
			{
				printf("Warning: too many systems! System \"%s\" ignored! Max system quantity is %d\n", maxSystemCount);
			}
		}
		else
		{
			printf("Warning: unknown parameter \"%s\"\n", argv[argn]);
		}
		argn++;
	}

	if(systemCount > 0)
	{
		executeScenery(systemData, systemCount, scenery);
		return ERROR_CODE_OK;
	}
	else
	{
		printf("Error: no systems assigned!\n");
		return ERROR_CODE_NO_SYSTEMS;
	}
}