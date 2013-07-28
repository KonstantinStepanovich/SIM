//#include "stdafx.hpp"

#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"
#include "Database/ParserJson.hpp"

#include <stdio.h>
#include <iostream>

namespace Sim
{

enum Result
{
	RESULT_OK = 0,
	RESULT_LUA_SCRIPT_NAME_MISSED = 1,
	RESULT_LUA_TABLE_NAME_MISSED = 2,
	RESULT_JSON_FILE_NAME_MISSED = 3
};

void lua2Json(const char * luaScriptName, const char * luaTableName, const char * jsonFileName)
{
	VariantRoot root;
	loadLua(luaScriptName, luaTableName, root);
	saveJson(jsonFileName, "", root);
};

}

using namespace Sim;

int main(int argc, const char * argv[])
{
	bool luaScriptFound = false;
	bool luaTableFound = false;
	bool jsonFileFound = false;
	const size_t strMaxLen = 512;
	char luaScriptName[strMaxLen];
	char luaTableName[strMaxLen];
	char jsonFileName[strMaxLen];
	for(int i = 0; i < argc; ++i)
	{
		if(_snscanf(argv[i], std::min(strlen(argv[i]), strMaxLen), "/lua=%s", luaScriptName) == 1)
			luaScriptFound = true;
		if(_snscanf(argv[i], std::min(strlen(argv[i]), strMaxLen), "/tbl=%s", luaTableName) == 1)
			luaTableFound = true;
		if(_snscanf(argv[i], std::min(strlen(argv[i]), strMaxLen), "/json=%s", jsonFileName) == 1)
			jsonFileFound = true;
		if(luaScriptFound && luaTableFound && jsonFileFound)
			break;
	}
	if(!luaScriptFound)
	{
		std::cout << "Lua script name missed";
		return RESULT_LUA_SCRIPT_NAME_MISSED;
	}
	if(!luaTableFound)
	{
		std::cout << "Lua table name missed";
		return RESULT_LUA_TABLE_NAME_MISSED;
	}
	if(!jsonFileFound)
	{
		std::cout << "Json file name missed";
		return RESULT_JSON_FILE_NAME_MISSED;
	}
	try
	{
		lua2Json(luaScriptName, luaTableName, jsonFileName);
	}
	catch(Exception & e)
	{
		std::cout << e.what();
	}

	std::cout << "\"" << luaScriptName << "\" successfully converted to \"" << jsonFileName << "\"";

	return RESULT_OK;
}