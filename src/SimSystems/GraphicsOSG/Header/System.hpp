#ifndef _GraphicsOSG_System_hpp_
#define _GraphicsOSG_System_hpp_

#include "ISystem.hpp"

#include <osg/ref_ptr>

namespace osgViewer
{
	class Viewer;
}

namespace Sim
{

using namespace Framework;

namespace GraphicsOSG
{

class Scene;

class System : public ISystem
{
public:
	System();
	~System();
	virtual const SystemName & getName() const;
	virtual ISystemScene * createScene();
	virtual void destroyScene();
private:
	osg::ref_ptr<osgViewer::Viewer> _viewer;
	Scene * _scene;
};

}

}

#endif _GraphicsOSG_System_hpp_