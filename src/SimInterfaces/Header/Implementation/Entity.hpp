#ifndef _Interfaces_Entity_hpp_
#define _Interfaces_Entity_hpp_

#include "Interface/IEntity.hpp"

namespace Sim
{

namespace Interfaces
{

class Entity : public IEntity
{
public:
	Entity();
	Entity(const TypeId & typeId);
	virtual const TypeId & getTypeId() const;
protected:
	TypeId typeId_;
};

}

}

#endif _Interfaces_Entity_hpp_