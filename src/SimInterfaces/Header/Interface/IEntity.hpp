#ifndef _IEntity_hpp_
#define _IEntity_hpp_

#include "Database/Entity.hpp"

namespace Sim
{

namespace Interfaces
{

class IEntity
{
public:
	virtual ~IEntity() {;}
	virtual const TypeId & getTypeId() const = 0;
};

}

}

#endif _IEntity_hpp_