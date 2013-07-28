#include "stdafx.h"

#include "Objects/CameraObject.hpp"

#include "Events.hpp"

#include "Database/Variant.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

CameraObject::CameraObject(const ObjectName & objectName) : Object(objectName, 0.0)
{
	setType(TYPE_TRACK_WORLD_POINT);
	setDistance(50.0f);
	setCenter(Math::Vec3(0.0f, 0.0f, 0.0f));
	setRotation(Math::Quat::getIdentity());
}

void CameraObject::load(const Variant & variant)
{
	if(const Variant type = variant.key("type"))
	{
		std::string typeStr = type;
		if(typeStr == "TYPE_TRACK_OBJECT_LOCAL")
			setType(ICamera::TYPE_TRACK_OBJECT_LOCAL);
		else if(typeStr == "TYPE_TRACK_OBJECT")
			setType(ICamera::TYPE_TRACK_OBJECT);
	}
	switch(data.type)
	{
		case ICamera::TYPE_TRACK_WORLD_POINT:
			{
				Math::Vec3 pointToTrack;
				if(tryLoadFromTable(variant, "point", pointToTrack))
					setPointToTrack(pointToTrack);
			}
			break;
		case ICamera::TYPE_TRACK_OBJECT_LOCAL:
		case ICamera::TYPE_TRACK_OBJECT:
			if(const Variant objectToTrack = variant.tryKey("objectToTrack"))
				setObjectToTrackName(objectToTrack);
			break;
	};
	if(const Variant distance = variant.key("distance"))
		setDistance(distance);
	Math::Vec3 center;
	tryLoadFromTable(variant, "center", center);
	setCenter(center);
	Math::Quat quat;	
	tryLoadQuat(variant, quat);	
	setRotation(quat);
}

const ObjectClass & CameraObject::getClass() const
{
	static const ObjectClass objectClass("Camera");
	return objectClass;
}

Event CameraObject::getPotentialEvents() const
{
	return Interfaces::EVENT_CAMERA_CHANGED;
}