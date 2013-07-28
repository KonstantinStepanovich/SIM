#ifndef _ShapeGen_BodyEmmiter_hpp_
#define _ShapeGen_BodyEmmiter_hpp_

#include "Actors/Actor.hpp"

#include "ISystem.hpp"
#include "Interface/IEntity.hpp"
#include "Interface/IMass.hpp"

#include "Math/Scalar.hpp"
#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"
#include "Core/ModelTime/ModelTime.hpp"
#include "Core/ModelTime/Activity.hpp"

namespace Sim
{

class Variant;

using namespace Sim::Framework;

namespace ShapeGen
{

class BodyObject;

class BodyEmmiter : public Actor
{
public:
	BodyEmmiter(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	~BodyEmmiter();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void update(ModelTime modelTime, ModelTime dtime);
private:
	BodyObject * createPlane_(ModelTime modelTime);
	BodyObject * createBody_(ModelTime modelTime);

	BodyObject * plane_;
	
	class Activity : public PeriodicActivity
	{
	public:
		Activity(BodyEmmiter * bodyEmmiter, ModelTime dtime);
		virtual ModelTime operator()(ModelTime time);
	private:
		BodyEmmiter * bodyEmmiter_;
	} activity_;

	Math::Vec3 pos_;
	Math::Quat rot_;
	Math::Vec3 size_;
	Math::Real minVel_;
	Math::Real maxVel_;
	Math::Real maxAngle_;
	Math::Real minBodySize_;
	Math::Real maxBodySize_;
	Math::Real bodyDensity_;
	Math::Real bodyRate_;
	float planeColor_[4];
	float bodyColorMin_[4], bodyColorMax_[4];

	unsigned int bodyCount_;
};

}

}

#endif _ShapeGen_BodyEmmiter_hpp_