#include "stdafx.h"

#include "Implementation/Entity.hpp"

using Sim::TypeId;
using namespace Sim::Interfaces;

Entity::Entity()
{
}

Entity::Entity(const TypeId & typeId) : typeId_(typeId)
{
}

const TypeId & Entity::getTypeId() const
{
	return typeId_;
}