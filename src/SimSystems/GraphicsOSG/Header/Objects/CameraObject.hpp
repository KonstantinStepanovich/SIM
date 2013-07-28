#ifndef _GraphicsOSG_CameraObject_hpp_
#define _GraphicsOSG_CameraObject_hpp_

#include "Object.hpp"
#include "Implementation/Camera.hpp"

#include <osg/ref_ptr>

namespace osg
{
class Camera;
}

namespace Sim
{

using namespace Framework;

namespace GraphicsOSG
{

class Scene;
class TrackingTrackballManipulator;
class ObjectStatusBarCallback;

class CameraObject :	public Object,
						public Interfaces::Camera
{
public:
	CameraObject(const ObjectName & objectName, Scene * scene);
	~CameraObject();
	virtual const ObjectClass & getClass() const;
	virtual ISystemScene * getSystemScene() const;
	virtual Event getPotentialEvents() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
	virtual osg::Node* getNode();
private:
	osg::ref_ptr<TrackingTrackballManipulator>	_trackingTrackballManipulator;
	osg::ref_ptr<ObjectStatusBarCallback>		_objectStatusBarCallback;
	osg::ref_ptr<osg::Camera>					_camera;
};

}

}

#endif _GraphicsOSG_CameraObject_hpp_