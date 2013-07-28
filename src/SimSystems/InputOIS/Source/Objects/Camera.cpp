#include "stdafx.h"

#include "Objects/Camera.hpp"
#include "Implementation/Commands.hpp"

#include "Events.hpp"

using namespace Sim;

InputOIS::Camera::Camera(const ObjectName & objectName, Scene * scene) : Object(objectName, scene),
	Interfaces::Camera()
{
}

const ObjectClass & InputOIS::Camera::getClass() const
{
	static const ObjectClass objectClass("Camera");
	return objectClass;
}

Event InputOIS::Camera::getPotentialEvents() const
{
	return Interfaces::EVENT_CAMERA_CHANGED;
}

Event InputOIS::Camera::getEventsOfInterest() const
{
	return Interfaces::EVENT_CAMERA_CHANGED;
}

void InputOIS::Camera::onEvent(ISubject* subject, Event event)
{
	if(event & EVENT_CAMERA_CHANGED)
	{
		ICamera * camera = dynamic_cast<ICamera*>(subject);
		ICamera::Data data;
		camera->getCamera(data);
		setData(data);
	}
}

void InputOIS::Camera::onCommand(Command command)
{
	switch(command)
	{
		case COMMAND_CAMERA_WORLD_POINT:
			setType(Camera::TYPE_TRACK_WORLD_POINT);
			break;
		case COMMAND_CAMERA_OBJECT_POSITION:
			setType(Camera::TYPE_TRACK_OBJECT);
			break;
		case COMMAND_CAMERA_OBJECT_TRANSFORM:
			setType(Camera::TYPE_TRACK_OBJECT_LOCAL);
			break;
	}
}

void InputOIS::Camera::setType(Type typeIn)
{
	Interfaces::Camera::setType(typeIn);
	notifyEvent(Interfaces::EVENT_CAMERA_CHANGED);
}