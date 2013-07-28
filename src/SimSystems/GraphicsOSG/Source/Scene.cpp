#include "stdafx.h"

#include <assert.h>

#include "Scene.hpp"
#include "System.hpp"
#include "Task.hpp"
#include "Object.hpp"
#include "Objects/BodyObject.hpp"
#include "Objects/CameraObject.hpp"

#include "ISystem.hpp"

#include "Events.hpp"
#include "Interface/ICamera.hpp"

#include "Database/ParserLua.hpp"
#include "Database/ParserJson.hpp"

#include <osg/Geode>

#include <osgViewer/Viewer>
#include <osgViewer/api/Win32/GraphicsWindowWin32>

using namespace Sim::Framework;
using namespace Sim::GraphicsOSG;

Scene::Scene(osgViewer::Viewer * viewer, System * system) : _system(system),
															_viewer(viewer),
															_root(new osg::Group()),
															_camera(NULL)
{
	_viewer->setSceneData(_root);
	_task.reset(new Task(_viewer, this));
	_viewer->getCamera()->setClearColor(osg::Vec4(0.0f, 1.0f, 1.0f, 1.0f));
	{
		osg::Light * light = new osg::Light();
		light->setPosition(osg::Vec4(0.0f, 10000000.0f, 0.0f, 0.0f));
		light->setDirection(osg::Vec3(0.0f, -1.0f, 0.0f));
		light->setThreadSafeRefUnref(true);
		light->setLightNum(0);
		light->setAmbient(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
		light->setDiffuse(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
		light->setSpecular(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
		_viewer->setLight(light);
	}
}

Scene::~Scene()
{
	assert(_camera == NULL);
	assert(_bodies.empty());
	_viewer->setSceneData(NULL);
}

//#include <osg/Geode>
//#include <osg/MatrixTransform>
//#include <osg/ShapeDrawable>

void Scene::load(const char * scene)
{
	_viewer->setSceneData(_root);
	notifyEvent(Interfaces::EVENT_WINDOW_CHANGED);

	//osg::ShapeDrawable * s = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), 5.0));
	//osg::Matrix m;
	//m.setTrans(osg::Vec3(5.0, 5.0, 5.0));
	//osg::MatrixTransform * t1 = new osg::MatrixTransform(m);
	//osg::Geode * g1 = new osg::Geode();
	//g1->addDrawable(s);
	//t1->addChild(g1);
	//m.setTrans(osg::Vec3(-5.0, 5.0, -5.0));
	//osg::MatrixTransform * t2 = new osg::MatrixTransform(m);
	//osg::Geode * g2 = new osg::Geode();
	//g2->addDrawable(s);
	//t2->addChild(g2);
	//_root->addChild(t1);
	//_root->addChild(t2);
}

ISystem * Scene::getSystem()
{
	return _system;
}

ISystemTask * Scene::getSystemTask()
{
	return _task.get();
}

ISystemObject * Scene::createObject(const ObjectName & objectName, const ObjectClass & objectClass)
{
	if(objectClass == "Camera")
	{
		CameraObject * camera = new CameraObject("camera", this);
		addObject_(camera);
		_camera = camera;
		return camera;
	}
	else if(objectClass == "Body" ||
			objectClass == "Airplane")
	{
		return addBody_(new BodyObject(objectName, this));
	}
	else
		return NULL;
}

void Scene::destroyObject(ISystemObject * systemObject)
{
	if(systemObject->getClass() == "Camera")
		_camera = NULL;
	else
	{
		Object * object = static_cast<Object*>(systemObject);
		removeObject_(object);
		delete object;
	}
}

void Scene::onEvent(ISubject* subject, Event event)
{
}

Event Scene::getEventsOfInterest() const
{
	return NO_EVENT;
}

Event Scene::getPotentialEvents() const
{
	return Interfaces::EVENT_WINDOW_CHANGED;
}

HWND Scene::getWindowHandle() const
{
	osgViewer::Viewer::Windows windows;
	_viewer->getWindows(windows);
	assert(!windows.empty());
	const osgViewer::GraphicsWindowWin32 * graphicsWindowWin32 = static_cast<const osgViewer::GraphicsWindowWin32*>(windows[0]);		
	return graphicsWindowWin32->getHWND();
}

void Scene::setManipulator(osgGA::MatrixManipulator * matrixManipulator)
{
	_viewer->setCameraManipulator(matrixManipulator);
}

BodyObject * Scene::getBody(const ObjectName & objectName)
{
	Bodies::iterator itB = _bodies.find(objectName);
	return itB != _bodies.end() ? itB->second : NULL;
}

ISystemObject * Scene::addObject_(Object * object)
{
	_root->addChild(object->getNode());
	return object;
}

void Scene::removeObject_(Object * object)
{
	_bodies.erase(object->getName());
	Interfaces::ICamera::Data data;
	if(_camera != NULL)
	{
		_camera->getCamera(data);
		if(data.objectToTrackName == object->getName())
			_camera->setObjectToTrackName("");
	}
	_root->removeChild(object->getNode());
}

ISystemObject * Scene::addBody_(BodyObject * body)
{
	addObject_(body);
	_bodies[body->getName()] = body;
	return body;
}
