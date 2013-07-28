#ifndef _SystemNameToReplace_Object_hpp_
#define _SystemNameToReplace_Object_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace SystemNameToReplace
{

class Scene;

class Object : public ISystemObject
{
public:
	Object(const ObjectName & objectName, Scene * scene);
	virtual ~Object() {;}
	virtual ISystemScene * getSystemScene() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
protected:
	Scene * scene_;
};

}

}

#endif _SystemNameToReplace_Object_hpp_