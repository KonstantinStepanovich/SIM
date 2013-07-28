#ifndef _GraphicsOSG_Task_hpp_
#define _GraphicsOSG_Task_hpp_

#include "ISystem.hpp"

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

class Task : public ISystemTask
{
public:
	Task(osgViewer::Viewer * viewer, Scene * scene);
	virtual ISystemScene * getSystemScene() const;
	virtual void operator()(Time time, ModelTime modelTime);
private:
	Scene * scene_;
	osgViewer::Viewer * _viewer;
};

}

}

#endif _GraphicsOSG_Task_hpp_