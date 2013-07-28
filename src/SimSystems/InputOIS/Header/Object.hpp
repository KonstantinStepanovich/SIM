#ifndef _InputOIS_Object_hpp_
#define _InputOIS_Object_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace InputOIS
{

class Scene;
enum Command;
struct AxisCommand;

class Object : public ISystemObject
{
public:
	Object(const ObjectName & objectName, Scene * scene);
	virtual ~Object() {;}
	virtual ISystemScene * getSystemScene() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
	virtual void onCommand(Command command);
	virtual void onAxisCommand(const AxisCommand & axisCommand);
protected:
	Scene * scene_;
};

}

}

#endif _InputOIS_Object_hpp_