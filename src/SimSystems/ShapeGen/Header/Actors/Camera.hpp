#ifndef _ShapeGen_Camera_hpp_
#define _ShapeGen_Camera_hpp_

#include "Actors/Actor.hpp"

namespace Sim
{

class Variant;

namespace ShapeGen
{

class CameraObject;

class Camera : public Actor
{
	Camera();
public:
	~Camera();
	static Camera & instance();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void load(const Variant & variant);
	virtual void release();
	CameraObject * cameraObject_;
};

}

}

#endif _ShapeGen_Camera_hpp_