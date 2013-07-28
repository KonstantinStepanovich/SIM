#ifndef _GraphicsOSG_Object_hpp_
#define _GraphicsOSG_Object_hpp_

#include "ISystem.hpp"

namespace osg
{
class Node;
};

namespace Sim
{

using namespace Framework;

namespace GraphicsOSG
{

class Scene;

class Object : public ISystemObject
{
public:
	Object(const ObjectName & objectName, Scene * scene);
	virtual ~Object() {;}
	virtual ISystemScene * getSystemScene() const;
	virtual Event getPotentialEvents() const;
	virtual osg::Node* getNode() = 0;
protected:
	Scene * _scene;
};

}

}

#endif _GraphicsOSG_Object_hpp_