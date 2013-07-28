#include "stdafx.h"

#include "Utils/Geometry.hpp"
#include "Utils/Body.hpp"
#include "Utils/Utilities.hpp"
#include "Utils/Materials.hpp"

#include "Interface/IShape.hpp"
#include "Events.hpp"

#include <map>
#include <string>
#include <limits>

#include "Debug/CheckValue.hpp"
#include "Database/Variant.hpp"
#include "String/StringStream.hpp"
#include "Algorithm/Exception.hpp"

#include "PhysXAPI/PxRigidDynamic.h"
#include "PhysXAPI/PxRigidActor.h"
#include "PhysXAPI/PxShape.h"
#include "PhysXAPI/geometry/PxSphereGeometry.h"
#include "PhysXAPI/geometry/PxBoxGeometry.h"
#include "PhysXAPI/geometry/PxCapsuleGeometry.h"

using Sim::ModelTime;
using namespace Sim::PhysicsPhysX;
using namespace Sim::Interfaces;

using namespace physx;

Geometry::Geometry(physx::PxShape * & shape) : shape_(shape)
{
}

void Geometry::setSphere(PxRigidActor * rigidActor, PxReal radius)
{
	if(shape_ != NULL)
		shape_->release();
	shape_ = rigidActor->createShape(PxSphereGeometry(radius), *mainMaterial);
}

void Geometry::setBox(PxRigidActor * rigidActor, PxReal dx, PxReal dy, PxReal dz)
{
	if(shape_ != NULL)
		shape_->release();
	shape_ = rigidActor->createShape(PxBoxGeometry(dx / 2, dy / 2, dz / 2), *mainMaterial);
}

void Geometry::setCylinder(PxRigidActor * rigidActor, PxReal radius, PxReal height)
{
	if(shape_ != NULL)
		shape_->release();
	shape_ = rigidActor->createShape(PxCapsuleGeometry(radius, height / 2), *mainMaterial);
}

class GeomLoader
{
public:
	GeomLoader()
	{
		geomLoadFuncs_["Sphere"]	= &loadSphere;
		geomLoadFuncs_["Box"]		= &loadBox;
		geomLoadFuncs_["Cylinder"]	= &loadCylinder;
	}
	static GeomLoader & instance()
	{
		static GeomLoader geomLoader;
		return geomLoader;
	}
	static physx::PxShape * loadSphere(PxRigidActor * rigidActor, const Sim::Variant & variant)
	{
		PxReal radius = variant.key("radius");
		return rigidActor->createShape(PxSphereGeometry(radius), *mainMaterial);
	}
	static physx::PxShape * loadBox(PxRigidActor * rigidActor, const Sim::Variant & variant)
	{
		PxReal dx = variant.key("dx");
		PxReal dy = variant.key("dy");
		PxReal dz = variant.key("dz");
		return rigidActor->createShape(PxBoxGeometry(dx / 2, dy / 2, dz / 2), *mainMaterial);
	}
	static physx::PxShape * loadCylinder(PxRigidActor * rigidActor, const Sim::Variant & variant)
	{
		PxReal radius = variant.key("radius");
		PxReal height = variant.key("height");
		return rigidActor->createShape(PxCapsuleGeometry(radius, height / 2), *mainMaterial);
	}
	physx::PxShape * load(PxRigidActor * rigidActor, const Sim::Variant & variant) const
	{
		std::string name = variant.key("name");
		std::string type = variant.key("type");
		GeomLoadFuncs::const_iterator itGL = geomLoadFuncs_.find(type);
		if(itGL == geomLoadFuncs_.end())
			THROW_EXCEPTION(Sim::Exception, OSS("Unknown geometry \"" << type << "\""));
		return (*itGL->second)(rigidActor, variant);
	}
private:
	typedef physx::PxShape * (*GeomLoadFunc)(PxRigidActor * rigidActor, const Sim::Variant & variant);
	typedef std::map<std::string, GeomLoadFunc> GeomLoadFuncs;
	GeomLoadFuncs geomLoadFuncs_;
};

void Geometry::load(PxRigidActor * rigidActor, const Sim::Variant & variant)
{
	if(shape_ != NULL)
		shape_->release();
	
	shape_ = GeomLoader::instance().load(rigidActor, variant);

	PxVec3 pos;
	tryLoadFromTable(variant, "pos", pos, loadVec);
	PxQuat quat;
	loadQuatEx(variant, quat);
	shape_->setLocalPose(PxTransform(pos, quat));
}

void Geometry::onShapeChanged(physx::PxRigidActor * rigidActor, IShape * shape)
{
	IShape::Type type = shape->getType();
	switch(type)
	{
		case IShape::SHAPE_TYPE_SPHERE:
			{
				ISphere * sphere = static_cast<ISphere*>(shape);
				setSphere(rigidActor, sphere->getRadius());
			}
			break;
		case IShape::SHAPE_TYPE_BOX:
			{
				IBox * box = static_cast<IBox*>(shape);
				const Math::Vec3 & size = box->getSize();
				setBox(rigidActor, (PxReal)size.x(), (PxReal)size.y(), (PxReal)size.z());
			}
			break;
		case IShape::SHAPE_TYPE_CYLINDER:
			{
				ICylinder * cylinder = static_cast<ICylinder*>(shape);
				setCylinder(rigidActor, (PxReal)cylinder->getRadius(), (PxReal)cylinder->getHeight());
			}
			break;
		default:
			return;
	}
}
