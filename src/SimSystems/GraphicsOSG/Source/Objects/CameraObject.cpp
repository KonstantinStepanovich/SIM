#include "stdafx.h"

#include "Objects/CameraObject.hpp"
#include "Objects/BodyObject.hpp"
#include "Scene.hpp"

#include "IEnvironmentManager.hpp"

#include "Interface/IBody.hpp"
#include "Interface/ICamera.hpp"
#include "Events.hpp"

#include "Utils/TrackingTrackballManipulator.hpp"

#include <sstream>

#include <osg/Matrix>
#include <osg/Group>
#include <osg/Geode>

#include <osgGA/MatrixManipulator>

#include <osgText/Font>
#include <osgText/Text>

using namespace Sim;
using namespace Sim::GraphicsOSG;

namespace Sim
{

namespace GraphicsOSG
{

extern Managers managers;

class ObjectStatusBarCallback : public osg::Drawable::UpdateCallback
{
public:
	ObjectStatusBarCallback() : _body(NULL) {;}
	inline void setBody(const BodyObject * body)
	{
		_body = body;
	}
private:
	virtual void update(osg::NodeVisitor* nv, osg::Drawable* text)
	{
		static_cast<osgText::Text*>(text)->setText(_body != NULL ?_body->getDebugState() : "");
	}
	const BodyObject * _body;
};

}

}

CameraObject::CameraObject(const ObjectName & objectName, Scene * scene) : Object(objectName, scene),
																_trackingTrackballManipulator(new TrackingTrackballManipulator()),
																_objectStatusBarCallback(new ObjectStatusBarCallback())
{
	_scene->setManipulator(_trackingTrackballManipulator);

	_trackingTrackballManipulator->setDistance(100.0f);
	_trackingTrackballManipulator->setCenter(osg::Vec3());
	_trackingTrackballManipulator->setRotation(osg::Quat());

	//Text & hud camera
	{
		osg::Group * group = new osg::Group();
		osg::Geode * geode = new osg::Geode();
		group->addChild(geode);
		osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
		const osg::Vec4 layoutColor(1.0f,1.0f,0.0f,1.0f);
		{
			osgText::Text * text = new osgText::Text;
			text->setFont(font);
			text->setColor(layoutColor);
			text->setFontResolution(40, 40);
			text->setPosition(osg::Vec3(20.0, 1000.0, 0.0));

			text->setCharacterSize(60);
			text->setLineSpacing(-0.5);
			text->setDataVariance(osg::Object::DYNAMIC);

			geode->addDrawable(text);

			class MainStatusBarCallback : public osg::Drawable::UpdateCallback
			{
				virtual void update(osg::NodeVisitor* nv, osg::Drawable* text)
				{
					ModelTime modelTime = nv->getFrameStamp()->getSimulationTime();
					std::ostringstream stream;
					stream << "Simulation is " << (!managers.environmentManager->isPause() ? "running" : "paused") << std::endl;
					stream << "Model time (sec) " << modelTime << std::endl;
					stream << "Model time scale: " << managers.environmentManager->getTimeAccelleration() << " : " << managers.environmentManager->getTimeDecelleration() << std::endl;
					static_cast<osgText::Text*>(text)->setText(stream.str());
				}
			};
			text->setUpdateCallback(new MainStatusBarCallback());
		}
		{
			osgText::Text * text = new osgText::Text;
			text->setFont(font);
			text->setColor(layoutColor);
			text->setFontResolution(40,40);
			text->setPosition(osg::Vec3(20.0, 600.0, 0.0));

			text->setCharacterSize(60);
			text->setLineSpacing(-0.5);
			text->setDataVariance(osg::Object::DYNAMIC);

			geode->addDrawable(text);

			text->setUpdateCallback(_objectStatusBarCallback);
		}

		_camera = new osg::Camera();
		_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		_camera->setProjectionMatrixAsOrtho2D(0,1280,0,1024);
		_camera->setViewMatrix(osg::Matrix::identity());
		_camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		_camera->addChild(group);
		_camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	}
}

CameraObject::~CameraObject()
{

}

const ObjectClass & CameraObject::getClass() const
{
	static std::string className("Camera");
	return className;
}

ISystemScene * CameraObject::getSystemScene() const
{
	return _scene;
}

Event CameraObject::getPotentialEvents() const
{
	return NO_EVENT;
}

Event CameraObject::getEventsOfInterest() const
{
	return Interfaces::EVENT_CAMERA_CHANGED;
}

void CameraObject::onEvent(ISubject* subject, Event event)
{
	if(event & Interfaces::EVENT_CAMERA_CHANGED)
	{
		Interfaces::ICamera * camera = dynamic_cast<Interfaces::ICamera*>(subject);
		Interfaces::ICamera::Data data;
		camera->getCamera(data);
		Interfaces::Camera::setData(data);
		BodyObject * body = _scene->getBody(data.objectToTrackName);
		_objectStatusBarCallback->setBody(body);
		_trackingTrackballManipulator->setBodyToTrack(data.type == Interfaces::ICamera::TYPE_TRACK_WORLD_POINT ? NULL : body);
		_trackingTrackballManipulator->setUseBodyFrame(data.type == Interfaces::ICamera::TYPE_TRACK_OBJECT_LOCAL);
		_trackingTrackballManipulator->setDistance(data.distance);
		_trackingTrackballManipulator->setCenter(osg::Vec3(data.center.x(), data.center.y(), data.center.z()));
		_trackingTrackballManipulator->setRotation(osg::Quat(-osg::PI_2, osg::Vec3(0.0f, 1.0f, 0.0f))*osg::Quat(data.rotation.x(), data.rotation.y(), data.rotation.z(), data.rotation.w()));
		_trackingTrackballManipulator->setNode(body != NULL ? body->getNode() : NULL);
	}
}

osg::Node* CameraObject::getNode()
{
	return _camera;
}