#ifndef _ShapeGen_Task_hpp_
#define _ShapeGen_Task_hpp_

#include "ISystem.hpp"

namespace Sim
{

using namespace Framework;

namespace ShapeGen
{

class Scene;

class Task : public ISystemTask
{
public:
	Task(Scene * scene);
	virtual ISystemScene * getSystemScene() const;
	virtual void operator()(Time time, ModelTime modelTime);
private:
	Scene * scene_;	
	

};

}

}

#endif _ShapeGen_Task_hpp_