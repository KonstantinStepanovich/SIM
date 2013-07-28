#ifndef _Airplane_Object_hpp_
#define _Airplane_Object_hpp_

#include "Object.hpp"
#include "Implementation/Airplane.hpp"
#include "Implementation/KeyAxis.hpp"

namespace Sim
{

using namespace Framework;

namespace InputOIS
{

class Scene;

class Airplane :public Object,
				public Interfaces::Airplane
{
public:
	class Activity;
	Airplane(const ObjectName & objectName, Scene * scene);
	~Airplane();
	virtual ISystemScene * getSystemScene() const;
	virtual Event getEventsOfInterest() const;
	virtual Event getPotentialEvents() const;
	virtual const ObjectClass & getClass() const;
	virtual void onEvent(ISubject* subject, Event event);
	virtual void onCommand(Command command);
	virtual void onAxisCommand(const AxisCommand & axisCommand);
	void update();
protected:
	Scene * scene_;
	std::auto_ptr<Activity> activity_;
	KeyAxis aileronKeyAxis_;
	KeyAxis elevatorKeyAxis_;
	KeyAxis rudderKeyAxis_;
	KeyAxis throttleKeyAxis_;
};

}

}

#endif _Airplane_Object_hpp_