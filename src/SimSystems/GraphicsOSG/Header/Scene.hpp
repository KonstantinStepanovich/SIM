#ifndef _GraphicsOSG_Scene_hpp_
#define _GraphicsOSG_Scene_hpp_

#include "ISystem.hpp"
#include "Interface/IWindow.hpp"

#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/Quat>

#include <map>

namespace osgViewer
{
class Viewer;
}

namespace osg
{
class Group;
}

namespace osgGA
{
class MatrixManipulator;
}

namespace osgText
{
class Text;
}

namespace Sim
{

using namespace Framework;

namespace GraphicsOSG
{

class System;
class Task;
class Object;
class BodyObject;
class CameraObject;

class Scene :	public ISystemScene,
				public Interfaces::IWindow
{
public:
	Scene(osgViewer::Viewer * viewer, System * system);
	~Scene();
	virtual void load(const char * scene);
	
	virtual ISystem * getSystem();
	virtual ISystemTask * getSystemTask();
	
	virtual ISystemObject * createObject(const ObjectName & objectName, const ObjectClass & objectClass);
	virtual void destroyObject(ISystemObject * systemObject);

	virtual void onEvent(ISubject* subject, Event event);
	virtual Event getEventsOfInterest() const;
	virtual Event getPotentialEvents() const;

	virtual HWND getWindowHandle() const;

	void setManipulator(osgGA::MatrixManipulator * matrixManipulator);
	BodyObject * getBody(const ObjectName & objectName);

private:
	ISystemObject * addObject_(Object * object);
	void removeObject_(Object * object);

	ISystemObject * addBody_(BodyObject * body);
	
	System*							_system;
	std::auto_ptr<Task>				_task;

	osg::ref_ptr<osgViewer::Viewer>	_viewer;
	osg::ref_ptr<osg::Group>		_root;
	
	CameraObject *					_camera;
	typedef std::map<ObjectName, BodyObject*> Bodies;
	Bodies							_bodies;
};

}

}

#endif _GraphicsOSG_Scene_hpp_