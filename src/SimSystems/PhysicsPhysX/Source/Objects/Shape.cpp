#include "stdafx.h"

#include "Objects/Shape.hpp"
#include "Scene.hpp"

#include "Interface/IBody.hpp"
#include "Interface/IMass.hpp"
#include "Interface/IShape.hpp"
#include "Events.hpp"

#include "PhysXAPI/PxScene.h"
#include "PhysXAPI/PxRigidDynamic.h"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::PhysicsPhysX;

Shape::Shape(const ObjectName & objectName, Scene * scene) :
	Object(objectName, scene),
	body_(this, scene_->getSDK()), shape_(NULL)
{
	scene->get()->addActor(*body_.get());
}

Shape::~Shape()
{
}

const ObjectClass & Shape::getClass() const
{
	static const ObjectClass shapeClass("Shape");
	return shapeClass;
}

Event Shape::getEventsOfInterest() const
{
	return Object::getEventsOfInterest() | Interfaces::EVENT_SHAPE_CHANGED;
}

void Shape::onEvent(ISubject * subject, Event event)
{
	if(event & EVENT_POSITION_CHANGED)
	{
		body_.onPositionChanged(dynamic_cast<IPosition*>(subject));
	}
	if(event & EVENT_ROTATION_CHANGED)
	{
		body_.onRotationChanged(dynamic_cast<IRotation*>(subject));
	}
	if(event & EVENT_MASS_CHANGED)
	{
		onMassChanged(dynamic_cast<IMass*>(subject));
	}
	if(event & EVENT_SHAPE_CHANGED)
	{
		onShapeChanged(dynamic_cast<IShape*>(subject));
	}
}

void Shape::getPosition(IPosition::Data & data) const
{
	body_.getPosition(scene_->getModelTime(), data);
}

void Shape::getRotation(IRotation::Data & data) const
{
	body_.getRotation(scene_->getModelTime(), data);
}

void Shape::onShapeChanged(IShape * shape)
{
	Geometry(shape_).onShapeChanged(body_.get(), shape);
}

void Shape::onMassChanged(IMass * mass)
{
	body_.onMassChanged(mass);
}

void Shape::update()
{
	body_.update();
}

void Shape::onBodyMove(const Body * body)
{
	if(!body_.isKinematic())
		notifyEvent(EVENT_POSITION_CHANGED | EVENT_ROTATION_CHANGED);	
}

physx::PxActor * Shape::getActor()
{
	return body_.get();
}