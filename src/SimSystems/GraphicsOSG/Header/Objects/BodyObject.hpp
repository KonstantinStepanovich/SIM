#ifndef _GraphicsOSG_BodyObject_hpp_
#define _GraphicsOSG_BodyObject_hpp_

#include "Object.hpp"
#include "Utils/Geometry.hpp"

#include "Implementation/Entity.hpp"

#include "Core/ModelTime/ModelTime.hpp"
#include "Math/Control.hpp"

#include <osg/ref_ptr>
#include <osg/Matrix>
#include <osg/BoundingBox>

namespace osg
{
class MatrixTransform;
class Geode;
}

namespace Sim
{

namespace Framework
{
class ISubject;
}

namespace Interfaces
{
class IPosition;
class IDynamicBodyObject;
class IDebugObject;
}

namespace GraphicsOSG
{

class MatrixTransformExtrapolatingCallback;
class Arrow;
class Geometry;

//BodyObject has a position and rotation
class BodyObject :	public Object,
					public Interfaces::Entity
{
public:
	BodyObject(const ObjectName & objectName, Scene * scene);
	~BodyObject();
	virtual const ObjectClass & getClass() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);	

	osg::Matrix getMatrix() const;
	virtual osg::Node* getNode();

	inline const std::string & getDebugState() const { return _debugState; }
protected:
	void onPositionChanged(Interfaces::IPosition * position);
	void onDebugObjectChanged(Interfaces::IDebugObject * debugObject);

	void createAxes(const osg::BoundingBox & obb);
	//inline osg::MatrixTransform * getMatrixTransform() { return _matrixTransformRot; }
private:
	osg::ref_ptr<osg::MatrixTransform> _matrixTransformTrans;
	osg::ref_ptr<MatrixTransformExtrapolatingCallback> _matrixTransformTransExtrapolatingCallback;
	osg::ref_ptr<osg::MatrixTransform> _matrixTransformRot;
	osg::ref_ptr<MatrixTransformExtrapolatingCallback> _matrixTransformRotExtrapolatingCallback;
	osg::ref_ptr<Arrow> _basisArrow[3];
	osg::ref_ptr<Arrow> _accelerationArrow;
	Math::FirstOrderFilter<osg::Vec3, ModelTime> accelerationFOF_;
	ModelTime _prevTime;
	osg::ref_ptr<Arrow> _velocityArrow;
	osg::BoundingBox _obb;
	std::string _debugState;
	Geometry _geometry;
};

}

}

#endif _GraphicsOSG_BodyObject_hpp_