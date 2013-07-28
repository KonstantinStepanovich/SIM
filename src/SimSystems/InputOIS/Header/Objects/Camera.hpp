#ifndef _InputOIS_Camera_hpp_
#define _InputOIS_Camera_hpp_

#include "Object.hpp"

#include "Implementation/Camera.hpp"

namespace Sim
{

using namespace Framework;
using namespace Interfaces;

namespace InputOIS
{

class Scene;

class Camera :	public Object,
				public Interfaces::Camera
{
public:
	Camera(const ObjectName & objectName, Scene * scene);
	virtual ~Camera() {;}
	virtual const ObjectClass & getClass() const;
	virtual Event getPotentialEvents() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
	virtual void onCommand(Command command);
protected:
	void setType(Type typeIn);
};

}

}

#endif _InputOIS_Camera_hpp_