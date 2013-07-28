#ifndef _SFM_Payload_hpp_
#define _SFM_Payload_hpp_

#include "Algorithm/Tree.hpp"
#include "Database/Entity.hpp"

#include <vector>

namespace Sim
{

namespace SFM
{

struct Payload
{
	typedef unsigned int Index;
	enum {
		INDEX_NULL = -1
	};
	Index	index;
	TypeId	typeId;
	union {
		unsigned int	count;
		double			mass;
	} ammount;
};

typedef tree<Payload, std::vector> PayloadTree;

}

}

#endif _SFM_Payload_hpp_