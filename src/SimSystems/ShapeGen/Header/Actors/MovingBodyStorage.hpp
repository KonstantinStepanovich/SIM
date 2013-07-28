#ifndef _ShapeGen_MovingBodyStorage_hpp_
#define _ShapeGen_MovingBodyStorage_hpp_

#include "Actors/Actor.hpp"

#include "Math/Box3.hpp"
#include "Core/ModelTime/Activity.hpp"

#include <list>
#include <algorithm>

namespace Sim
{

class Variant;

namespace ShapeGen
{

class BodyObject;

class MovingBodyStorage : public Actor
{
	MovingBodyStorage();
	virtual void release();
public:
	static MovingBodyStorage & instance();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	void load(const Variant & variant);
	inline void add(BodyObject * bodyObject)
	{
		movingBodies_.push_back(Body(bodyObject));
	}
	void remove(BodyObject * bodyObject);
	void updateBodies(ModelTime modelTime);
	template<class Func>
	inline void iterate(Func & func) const
	{
		for(MovingBodies::const_iterator itMB = movingBodies_.begin();
			itMB != movingBodies_.end();
			++itMB)
			func(itMB->getBodyObject());
	}	
private:
	struct Parameters
	{
		Math::Box3	worldSize;
		Math::Real	minVelocity, minAngularVelocity;
		ModelTime	idlePeriod;
	} parameters_;
	class Activity : public PeriodicActivity
	{
	public:
		Activity(MovingBodyStorage * movingBodyStorage, ModelTime dtime);
		virtual ModelTime operator()(ModelTime time);
	private:
		MovingBodyStorage * movingBodyStorage_;		
	} activity_;
	class Body
	{
	public:
		Body(BodyObject * bodyObject);
		bool updateStatus(const Parameters & parameters);
		inline BodyObject * getBodyObject() const { return bodyObject_; }
	private:
		BodyObject *	bodyObject_;
		bool			idle_;
		ModelTime		idleStartTime_;
	};
	typedef std::list<Body> MovingBodies;
	MovingBodies movingBodies_;
};

}

}

#endif _ShapeGen_MovingBodyStorage_hpp_