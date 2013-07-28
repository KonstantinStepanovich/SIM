#ifndef _ParserJson_hpp_
#define _ParserJson_hpp_

namespace Sim
{

class Variant;

void loadJson(const char * path, const char * table, Variant & variant);

void saveJson(const char * path, const char * table, const Variant & variant);

}

#endif _ParserJson_hpp_