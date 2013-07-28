#ifndef _Parser_hpp_
#define _Parser_hpp_

namespace Sim
{

class Variant;

struct Parser
{
typedef void (*Load)(const char * path, const char * table, Variant & variant);
typedef void (*Save)(const char * path, const char * table, const Variant & variant);
};

}

#endif _Parser_hpp_