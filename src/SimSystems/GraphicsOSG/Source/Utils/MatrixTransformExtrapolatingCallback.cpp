#include "stdafx.h"

#include "Utils/MatrixTransformExtrapolatingCallback.hpp"

#include "Synchronization.hpp"
#include "Interface/IBody.hpp"
#include "Events.hpp"

#include  "Debug/CheckValue.hpp"

#include <osg/MatrixTransform>

class MatrixTransformExtrapolatingCallbackVisitor : public osg::NodeVisitor
{
public:
	MatrixTransformExtrapolatingCallbackVisitor(const osg::Vec3 & position, osg::Quat & rotation) : _position(position), _rotation(rotation)
	{
		Sim::validate(_position.ptr(), 3);
		Sim::validate(_rotation._v, 4);
	}
	virtual void apply(osg::MatrixTransform& mt)
	{
		osg::Matrix matrix;
		matrix.setTrans(_position);
		matrix.setRotate(_rotation);
		Sim::validate(matrix.ptr(), 16);
		mt.setMatrix(matrix);		
	}
private:
	const osg::Vec3 & _position;
	const osg::Quat & _rotation;
};

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::GraphicsOSG;

MatrixTransformExtrapolatingCallback::MatrixTransformExtrapolatingCallback(Flags flags) : _flags(flags), _time(0.0)
{

}

void MatrixTransformExtrapolatingCallback::update(	ModelTime time,
													const osg::Vec3 & position,
													const osg::Vec3 & velocity,
													const osg::Quat & rotation,
													const osg::Vec3 & omega)
{
	update(time, position, velocity);
	update(time, rotation, omega);
}

void MatrixTransformExtrapolatingCallback::update(	ModelTime time,
													const osg::Vec3 & position,
													const osg::Vec3 & velocity)
{
	_time = time;
	_position = position;
	_velocity = velocity;
}

void MatrixTransformExtrapolatingCallback::update(	ModelTime time,
													const osg::Quat & rotation,
													const osg::Vec3 & omega)
{
	_time = time;
	_rotation = rotation;
	_omega = omega;
}

void MatrixTransformExtrapolatingCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType()==osg::NodeVisitor::UPDATE_VISITOR && 
		nv->getFrameStamp())
	{
		double time = nv->getFrameStamp()->getSimulationTime();
		double dt = time - _time;

		osg::Vec3 position;
		if(_flags & FLAG_POSITION)
			position = _position;
			//position = _position + _velocity * dt;

		osg::Quat rotation;
		if(_flags & FLAG_ROTATION)
		{
			rotation = _rotation;
			//osg::Vec3 angle = _omega * dt;
			//rotation = _rotation * 	osg::Quat(angle.y(), osg::Vec3(0.0f, 1.0f, 0.0f)) *
			//						osg::Quat(angle.z(), osg::Vec3(0.0f, 0.0f, 1.0f)) *
			//						osg::Quat(angle.x(), osg::Vec3(1.0f, 0.0f, 0.0f));
		}
		
		MatrixTransformExtrapolatingCallbackVisitor visitor(position, rotation);
		node->accept(visitor);
	}
	NodeCallback::operator()(node, nv);
}

void Sim::GraphicsOSG::onEvent(MatrixTransformExtrapolatingCallback * callback, ISubject * subject, Event event)
{
	if(event & EVENT_POSITION_CHANGED)
	{
		IPosition* position = dynamic_cast<IPosition*>(subject);
		IPosition::Data data;
		position->getPosition(data);
		callback->update(	data.time,
							osg::Vec3(data.position.x(), data.position.y(), data.position.z()),
							osg::Vec3(data.velocity.x(), data.velocity.y(), data.velocity.z()));
	}
	if(event & EVENT_ROTATION_CHANGED)
	{
		IRotation* rotation = dynamic_cast<IRotation*>(subject);
		IRotation::Data data;
		rotation->getRotation(data);
		callback->update(	data.time,
							osg::Quat(data.rotation.x(),		data.rotation.y(),			data.rotation.z(),			data.rotation.w()),
							osg::Vec3(data.angularVelocity.x(), data.angularVelocity.y(),	data.angularVelocity.z()));
	}
}
