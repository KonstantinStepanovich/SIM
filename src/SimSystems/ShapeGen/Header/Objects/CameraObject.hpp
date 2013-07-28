#ifndef _ShapeGen_CameraObject_hpp_
#define _ShapeGen_CameraObject_hpp_

#include "Object.hpp"

#include "Implementation/Camera.hpp"

namespace Sim
{

class Variant;

namespace ShapeGen
{

class Scene;

class CameraObject :	public Object,
						public Interfaces::Camera
{
public:
	CameraObject(const ObjectName & objectName);
	virtual ~CameraObject() {;}
	virtual void load(const Variant & variant);
	virtual const ObjectClass & getClass() const;
	virtual Event getPotentialEvents() const;
};

}

}

#endif _ShapeGen_CameraObject_hpp_