#include "stdafx.h"

#include "Actors/World.hpp"
#include "Actors/Camera.hpp"
#include "Objects/CameraObject.hpp"
#include "Scene.hpp"

#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

REGISER_ACTOR(Camera);

Camera::Camera() : cameraObject_(new CameraObject("Camera"))
{
	World::instance().addActor(this);
	Scene::instance()->addObject(cameraObject_);
}

Camera::~Camera()
{
	assert(cameraObject_ == NULL);
}

Camera & Camera::instance()
{
	static Camera camera;
	return camera;
}

Actor * Camera::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	Camera::instance().load(variant);
	return &Camera::instance();
}

void Camera::load(const Variant & variant)
{
	Interfaces::ICamera::Type type = Interfaces::ICamera::TYPE_VOID;
	if(const Variant vType = variant.key("camType"))
	{
		std::string typeStr = vType;
		if(typeStr == "TYPE_TRACK_OBJECT_LOCAL")
			type = Interfaces::ICamera::TYPE_TRACK_OBJECT_LOCAL;
		else if(typeStr == "TYPE_TRACK_OBJECT")
			type = Interfaces::ICamera::TYPE_TRACK_OBJECT;
	}
	if(type != Interfaces::ICamera::TYPE_VOID)
		cameraObject_->setType(type);
	switch(type)
	{
		case Interfaces::ICamera::TYPE_TRACK_WORLD_POINT:
			{
				Math::Vec3 pointToTrack;
				if(tryLoadFromTable(variant, "point", pointToTrack))
					cameraObject_->setPointToTrack(pointToTrack);
			}
			break;
		case Interfaces::ICamera::TYPE_TRACK_OBJECT_LOCAL:
		case Interfaces::ICamera::TYPE_TRACK_OBJECT:
			if(const Variant objectToTrack = variant.tryKey("objectToTrack"))
				cameraObject_->setObjectToTrackName(objectToTrack);
			break;
	};
	if(const Variant distance = variant.key("distance"))
		cameraObject_->setDistance(distance);
	Math::Vec3 center;
	tryLoadFromTable(variant, "center", center);
	cameraObject_->setCenter(center);
	Math::Quat quat;	
	if(tryLoadQuat(variant, quat))
		cameraObject_->setRotation(quat);
}

void Camera::release()
{
	Scene::instance()->removeObject(cameraObject_);
	delete cameraObject_;
	cameraObject_ = NULL;
}