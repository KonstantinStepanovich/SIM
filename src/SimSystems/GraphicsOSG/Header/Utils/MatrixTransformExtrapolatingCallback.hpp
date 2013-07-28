#ifndef _GraphicsOSG_MatrixTransformExtrapolatingCallback_hpp_
#define _GraphicsOSG_MatrixTransformExtrapolatingCallback_hpp_

namespace osg
{
class Group;
class MatrixTransform;
};

#include "Synchronization.hpp"

#include "Core/ModelTime/ModelTime.hpp"

#include <osg/Vec3>
#include <osg/Quat>
#include <osg/NodeCallback>

namespace Sim
{

namespace Framework
{
class ISubject;
}

namespace GraphicsOSG
{

class MatrixTransformExtrapolatingCallback : public osg::NodeCallback
{
public:
	enum Flags {
		FLAG_NOTHING = 0,
		FLAG_POSITION = 1 << 0,
		FLAG_ROTATION = 1 << 1,
		FLAG_ALL = FLAG_POSITION | FLAG_ROTATION
	};
	MatrixTransformExtrapolatingCallback(Flags flags);
	void update(ModelTime time,
				const osg::Vec3 & position,
				const osg::Vec3 & velocity,
				const osg::Quat & rotation,
				const osg::Vec3 & omega);
	void update(ModelTime time,
				const osg::Vec3 & position,
				const osg::Vec3 & velocity);
	void update(ModelTime time,
				const osg::Quat & rotation,
				const osg::Vec3 & omega);
private:
	virtual ~MatrixTransformExtrapolatingCallback() {;}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
	const Flags _flags;
	ModelTime _time;
	osg::Vec3 _position;
	osg::Vec3 _velocity;
	osg::Quat _rotation;
	osg::Vec3 _omega;
};

void onEvent(MatrixTransformExtrapolatingCallback * callback, Framework::ISubject * subject, Framework::Event event);

}

}

#endif _GraphicsOSG_MatrixTransformExtrapolatingCallback_hpp_