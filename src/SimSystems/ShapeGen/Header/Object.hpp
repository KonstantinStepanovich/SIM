#ifndef _ShapeGen_Object_hpp_
#define _ShapeGen_Object_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace ShapeGen
{

class Scene;

class Object : public ISystemObject
{
public:
	Object(const ObjectName & objectName, ModelTime birthTime);
	virtual ~Object();
	virtual ISystemScene * getSystemScene() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject* subject, Event event);
protected:
	ModelTime birthTime_;
};

}

}

#endif _ShapeGen_Object_hpp_