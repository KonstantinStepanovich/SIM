#include "stdafx.h"

#include "Utils/MultiGeometryBody.hpp"

#include "Interface/IEntity.hpp"

#include "Database/Variant.hpp"

#include <ode/objects.h>
#include <ode/collision.h>

#include <map>

using namespace Sim;
using namespace Sim::Interfaces;
using namespace Sim::PhysicsODE;

MultiGeometryBody::MultiGeometryBody() : spaceID_(NULL)
{
}

MultiGeometryBody::~MultiGeometryBody()
{
	for(Geoms::iterator itG = geoms_.begin();
		itG != geoms_.end();
		itG++)
		dSpaceRemove(spaceID_, itG->getID());

	if(spaceID_ != NULL)
		dSpaceDestroy(spaceID_);
}

void MultiGeometryBody::setParentSpace(dSpaceID parentSpaceID)
{
	if(	spaceID_ == NULL ||
		dGeomGetSpace((dGeomID)spaceID_) != parentSpaceID)
	{
		spaceID_ = dSimpleSpaceCreate(parentSpaceID);
		//dSpaceSetCleanup(spaceID_, 1);
	}
}

void MultiGeometryBody::load(const Sim::Variant & variant)
{
	body_.load(variant);
	if(const Sim::Variant vGeometry = variant.key("geometry"))
	{
		geoms_.resize(vGeometry.size());
		for(Sim::Variant::ConstIterator itG = vGeometry.begin();
			itG != vGeometry.end();
			++itG)
		{
			geoms_[itG.index()].load(*itG, &body_, spaceID_);			
		}
	}
}
