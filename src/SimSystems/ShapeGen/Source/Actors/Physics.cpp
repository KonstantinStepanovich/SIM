#include "stdafx.h"

#include "Actors/Physics.hpp"

#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

REGISER_ACTOR(Physics);

Physics::Physics() : gravity(Math::Vec3(0.0f, -9.81f, 0.0f))
{
}

Physics & Physics::instance()
{
	static Physics physics;
	return physics;
}

Actor * Physics::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	Physics::instance().load(variant);
	return &Physics::instance();
}

void Physics::load(const Variant & variant)
{
	::load(variant.key("gravity"), gravity);
}

void Physics::release()
{
}