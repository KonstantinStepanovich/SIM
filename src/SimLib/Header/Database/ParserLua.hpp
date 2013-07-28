#ifndef _ParserLua_hpp_
#define _ParserLua_hpp_

namespace Sim
{

class Variant;

void loadLua(const char * path, const char * table, Variant & variant);

void saveLua(const char * path, const char * table, const Variant & variant);

}

#endif _ParserLua_hpp_