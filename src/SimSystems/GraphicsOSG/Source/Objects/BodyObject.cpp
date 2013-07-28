#include "stdafx.h"

#include "Objects/BodyObject.hpp"
#include "Utils/MatrixTransformExtrapolatingCallback.hpp"
#include "Utils/Arrow.hpp"

#include "Events.hpp"
#include "Interface/IBody.hpp"
#include "Interface/IDynamicBody.hpp"
#include "Interface/IAnimated.hpp"
#include "Interface/IDebugObject.h"

#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/ShapeDrawable>

#include <osg/AnimationPath>

using namespace Sim::GraphicsOSG;
using namespace Sim::Framework;
using namespace Sim::Interfaces;

BodyObject::BodyObject(const ObjectName & objectName, Scene * scene) :	Object(objectName, scene),
	_matrixTransformTrans(new osg::MatrixTransform()),
	_matrixTransformTransExtrapolatingCallback(new MatrixTransformExtrapolatingCallback(MatrixTransformExtrapolatingCallback::FLAG_POSITION)),
	_matrixTransformRot(new osg::MatrixTransform()),
	_matrixTransformRotExtrapolatingCallback(new MatrixTransformExtrapolatingCallback(MatrixTransformExtrapolatingCallback::FLAG_ROTATION)),
	accelerationFOF_(0.5, osg::Vec3(0.0f, 0.0f, 0.0f)),
	_prevTime(0.0)
{
	_matrixTransformTransExtrapolatingCallback->setName(objectName);
	_matrixTransformRotExtrapolatingCallback->setName(objectName);
	_matrixTransformTrans->setUpdateCallback(_matrixTransformTransExtrapolatingCallback);
	_matrixTransformRot->setUpdateCallback(_matrixTransformRotExtrapolatingCallback);
	_matrixTransformTrans->addChild(_matrixTransformRot);
}

BodyObject::~BodyObject()
{
}

const ObjectClass & BodyObject::getClass() const
{
	static std::string className("Body");
	return className;
}

Event BodyObject::getEventsOfInterest() const
{
	return EVENT_BODY_CHANGED | EVENT_DEBUG_OBJECT_CHANGED;
}

void BodyObject::onEvent(ISubject * subject, Event event)
{
	if(event & EVENT_POSITION_CHANGED)
	{
		::onEvent(_matrixTransformTransExtrapolatingCallback, subject, EVENT_POSITION_CHANGED);
		onPositionChanged(dynamic_cast<IPosition*>(subject));
	}
	if(event & EVENT_ROTATION_CHANGED)
	{
		::onEvent(_matrixTransformRotExtrapolatingCallback, subject, EVENT_ROTATION_CHANGED);
	}
	if(event & EVENT_TYPE_ID_CHANGED)
	{
		_matrixTransformRot->removeChild(_geometry.getNode());
		_geometry.onEntityChanged(dynamic_cast<IEntity*>(subject));
		_matrixTransformRot->addChild(_geometry.getNode());
	}
	if(event & EVENT_ANIMATION_CHANGED)
	{
		_geometry.onAnimationChanged(dynamic_cast<IAnimated*>(subject));
	}
	if(event & EVENT_DEBUG_OBJECT_CHANGED)
	{
		onDebugObjectChanged(dynamic_cast<IDebugObject*>(subject));
	}
}

osg::Matrix BodyObject::getMatrix() const
{
	return _matrixTransformRot->getMatrix() * _matrixTransformTrans->getMatrix();
}

osg::Node* BodyObject::getNode()
{
	return _matrixTransformTrans;
}

void BodyObject::onPositionChanged(IPosition * position)
{
	IPosition::Data data;
	position->getPosition(data);
	if(_velocityArrow != NULL)
	{
		const float velocityCoeff =  30.0f / 600.0f;
		_velocityArrow->set(osg::Vec3(data.velocity.x(), data.velocity.y(), data.velocity.z()) * velocityCoeff);
	}
	if(_accelerationArrow != NULL)
	{
		osg::Vec3 acceleration(data.acceleration.x(), data.acceleration.y(), data.acceleration.z());
		const float accelerationCoeff = 30.0f / 30.0f;
		_accelerationArrow->set(accelerationFOF_.simulate(acceleration * accelerationCoeff, data.time - _prevTime));
		_prevTime = data.time;
	}
}

void BodyObject::onDebugObjectChanged(IDebugObject * debugObject)
{
	_debugState = debugObject->getString();
}

void BodyObject::createAxes(const osg::BoundingBox & obb)
{
	_obb = obb;

	const float basisAxisArrowLength = 3.0f;
	const float basisAxisArrowRadius = 0.1f;
	_basisArrow[0] = new Arrow(osg::Vec3(obb._max.x() + basisAxisArrowLength, 0.0f, 0.0f), 0.0f, basisAxisArrowRadius, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	_basisArrow[1] = new Arrow(osg::Vec3(0.0f, obb._max.y() + basisAxisArrowLength, 0.0f), 0.0f, basisAxisArrowRadius, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	_basisArrow[2] = new Arrow(osg::Vec3(0.0f, 0.0f, obb._max.z() + basisAxisArrowLength), 0.0f, basisAxisArrowRadius, osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
	for(int i = 0; i < 3; i++)
		_matrixTransformRot->addChild(_basisArrow[i]->getNode());

	const float forceArrowRadius = 0.2f;
	const float arrowLength = basisAxisArrowLength + 1.0f;
	_accelerationArrow = new Arrow(osg::Vec3(20.0f, 10.0f, 0.0f), obb.radius() + arrowLength, forceArrowRadius, osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	_matrixTransformRot->addChild(_accelerationArrow->getNode());

	const float velocityArrowRadius = 0.2f;
	_velocityArrow = new Arrow(osg::Vec3(20.0f, 10.0f, 0.0f), obb.radius() + arrowLength, velocityArrowRadius, osg::Vec4(0.4f, 0.5f, 0.5f, 1.0f));
	_matrixTransformTrans->addChild(_velocityArrow->getNode());
}