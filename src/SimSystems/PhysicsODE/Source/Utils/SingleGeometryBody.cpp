#include "stdafx.h"
/*
#include "Utils/SingleGeometryBody.hpp"

#include "Interface/IBody.hpp"
#include "Interface/IMass.hpp"
#include "Interface/IShape.hpp"

#include <ode/objects.h>
#include <ode/collision.h>

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::PhysicsODE;

SingleGeometryBody::SingleGeometryBody() : Body(), geomID_(NULL)
{

}

SingleGeometryBody::~SingleGeometryBody()
{
	if(geomID_ != NULL)
	{
		dGeomSetBody(geomID_, 0);
		dGeomDestroy(geomID_);
		geomID_ = NULL;
	}
}

void SingleGeometryBody::onShapeChanged(IShape * shape)
{
	IShape::Type type = shape->getType();
	if(geomID_ != NULL)
	{
		if(dGeomGetClass(geomID_) != type)
		{
			dGeomSetBody(geomID_, 0);
			dGeomDestroy(geomID_);
			geomID_ = NULL;
		}
	}
	
	dSpaceID spaceID = getSpaceID(scene_, dBodyIsKinematic(id_) != 0);

	switch(type)
	{
		case IShape::SHAPE_TYPE_SPHERE:
			{
				ISphere * sphere = static_cast<ISphere*>(shape);
				if(geomID_ != NULL)
					dGeomSphereSetRadius(geomID_, sphere->getRadius());
				else
					geomID_ = dCreateSphere(spaceID, sphere->getRadius());
			}
			break;
		case IShape::SHAPE_TYPE_BOX:
			{
				IBox * box = static_cast<IBox*>(shape);
				const Math::Vec3 & size = box->getSize();
				if(geomID_ != NULL)
					dGeomBoxSetLengths(geomID_, size.x(), size.y(), size.z());
				else
					geomID_ = dCreateBox(spaceID, size.x(), size.y(), size.z());
			}
			break;
		case IShape::SHAPE_TYPE_CYLINDER:
			{
				ICylinder * cylinder = static_cast<ICylinder*>(shape);
				if(geomID_ != NULL)
					dGeomCylinderSetParams(geomID_, cylinder->getRadius(), cylinder->getHeight());
				else
					geomID_ = dCreateCylinder(spaceID, cylinder->getRadius(), cylinder->getHeight());
			}
			break;
		default:
			return;
	}
	dGeomSetBody(geomID_, id_);
}

void SingleGeometryBody::onMassChanged(IMass * mass)
{
	bool wasKinematic = dBodyIsKinematic(id_) != 0;
	Object::onMassChanged(mass);
	bool isKinematic = dBodyIsKinematic(id_) != 0;
	if(geomID_ != NULL)
	{
		if(wasKinematic != isKinematic)
		{
			dSpaceRemove(getSpaceID(scene_, wasKinematic), geomID_);
			dSpaceAdd(getSpaceID(scene_, isKinematic), geomID_);
		}
	}
}
*/