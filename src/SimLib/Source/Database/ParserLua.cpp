#include "Database/ParserLua.hpp"
#include "Database/Variant.hpp"

#include "Algorithm/Exception.hpp"

#include <assert.h>

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
}

static void loadLuaVal(lua_State * L, Sim::Variant & variant)
{
	int tp = lua_type(L, -1);
	if(lua_istable(L, -1))
	{
		lua_pushnil(L);
		while(lua_next(L, -2) != 0)
		{
			if(lua_isnumber(L, -2))
			{
				unsigned int index = (unsigned int)lua_tonumber(L, -2);
				loadLuaVal(L, variant[index - 1]);
			}
			else if(lua_isstring(L, -2))
			{
				const char * key = lua_tostring(L, -2);
				loadLuaVal(L, variant[key]);
			}
			lua_pop(L, 1);
		}
	}
	else if(lua_isnumber(L, -1))
	{
		variant.setReal(lua_tonumber(L, -1));
	}
	else if(lua_isstring(L, -1))
	{
		variant.setString(lua_tostring(L, -1));
	}
	else if(lua_isboolean(L, -1))
	{
		variant.setBool(lua_toboolean(L, -1) != 0);
	}	
}

void Sim::loadLua(const char * path, const char * table, Variant & variant)
{
	lua_State * L = lua_open();
	luaL_openlibs(L);
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	if(luaL_dofile(L, path) != 0)
	{
		const std::string errorString = lua_tostring(L, -1);
		lua_pop(L, 1);
		lua_close(L);
		THROW_EXCEPTION(Exception, OSS("Unable to open file \"" << path << ": " << errorString.c_str()));
	}
	variant.setFileName(path);
	if(	table != NULL &&
		strlen(table) > 0)
	{
		lua_pushstring(L, table);
		lua_gettable(L, -2);
		if(!lua_istable(L, -1))
		{
			lua_close(L);
			THROW_EXCEPTION(Exception, OSS("Unable to open table \"" << table << "\"" << " in file \"" << path << "\""));
		}
	}
	else
	{
		lua_close(L);
		THROW_EXCEPTION(Exception, OSS("Can't load _G of file \"" << path << "\""));
	}
	loadLuaVal(L, variant);
	lua_close(L);
}

void Sim::saveLua(const char * path, const char * table, const Variant & variant)
{
	assert(!"Not implemented");
}