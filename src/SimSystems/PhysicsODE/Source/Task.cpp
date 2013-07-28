#include "stdafx.h"

#include "Task.hpp"
#include "Scene.hpp"
#include "Utils/Body.hpp"

#include "Core/ModelTime/Activity.hpp"

#include <ode/objects.h>
#include <ode/collision.h>

#include <assert.h>

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::PhysicsODE;

static dSurfaceParameters surfaceParameters;

namespace Sim
{

namespace PhysicsODE
{

const size_t maxContacts = 100;

class ContactGeomBuff
{
public:
	inline const dContactGeom & operator[](size_t index) const { return ptr_[index]; }
	inline dContactGeom * getPtr() const { return ptr_ + size_; }
	inline int getSize() const { return size_; }
	inline int getMaxSize() const { return maxSize_ - size_; }
	inline void advance(int step) { size_ += step; }
protected:
	ContactGeomBuff(dContactGeom * ptr, int maxSize) : ptr_(ptr), maxSize_(maxSize), size_(0) {;}
private:
	dContactGeom * const ptr_;
	const int maxSize_;
	int size_;
};

template<int size>
class SizedContactGeomBuff : public ContactGeomBuff
{
public:
	SizedContactGeomBuff() : ContactGeomBuff(&buff[0], size) {}
private:
	dContactGeom buff[size];
};

void collideAll(void *data, dGeomID o1, dGeomID o2)
{
	ContactGeomBuff * contactGeomBuff = (ContactGeomBuff*)data;

	if (dGeomIsSpace(o1) || dGeomIsSpace(o2))
	{

		// colliding a space with something :
		dSpaceCollide2(o1, o2, data, &collideAll); 

		// collide all geoms internal to the space(s)
		if (dGeomIsSpace(o1) && dGeomGetData(o1) != NULL)
			dSpaceCollide((dSpaceID)o1, data, &collideAll);
		if (dGeomIsSpace(o2) && dGeomGetData(o2) != NULL)
			dSpaceCollide((dSpaceID)o2, data, &collideAll);

	}
	else
	{
		// colliding two non-space geoms, so generate contact
		// points between o1 and o2
		//assert(contactGeomBuff->getMaxSize() > 0);
		if(contactGeomBuff->getMaxSize() > 0)
		{
			int num_contact = dCollide (o1, o2, contactGeomBuff->getMaxSize(), contactGeomBuff->getPtr(), sizeof(dContactGeom));
			contactGeomBuff->advance(num_contact);
		}
		// add these contact points to the simulation ...
	}
}

void collideDynamicObjects(void *data, dGeomID o1, dGeomID o2)
{
	ContactGeomBuff * contactGeomBuff = (ContactGeomBuff*)data;

	if (dGeomIsSpace (o1) || dGeomIsSpace (o2))
	{
		// colliding a space with something :
		dSpaceCollide2 (o1, o2, data, &collideAll);
	}
	else
	{
		// colliding two non-space geoms, so generate contact
		// points between o1 and o2
		//assert(contactGeomBuff->getMaxSize() > 0);
		if(contactGeomBuff->getMaxSize() > 0)
		{
			int num_contact = dCollide (o1, o2, contactGeomBuff->getMaxSize(), contactGeomBuff->getPtr(), sizeof(dContactGeom));
			contactGeomBuff->advance(num_contact);
		}
		// add these contact points to the simulation ...
	}
}

class Task::Activity : public Sim::PeriodicActivity
{
public:
	Activity(Scene * scene, dJointGroupID contactJointGroupID, ModelTime dtime) : scene_(scene), contactJointGroupID_(contactJointGroupID), PeriodicActivity(dtime)
	{
	}
	virtual ModelTime operator()(ModelTime time)
	{
		SizedContactGeomBuff<maxContacts> sizedContactGeomBuff;
		dSpaceCollide (scene_->getSpaceID(), &sizedContactGeomBuff, &collideAll);
		//dSpaceCollide (scene_->getSpaceID(false), &sizedContactGeomBuff, &collideDynamicObjects);
		//dSpaceCollide2 ((dGeomID)scene_->getSpaceID(true),
		//				(dGeomID)scene_->getSpaceID(false),
		//				&sizedContactGeomBuff, &collideAll);
		for(int i = 0; i < sizedContactGeomBuff.getSize(); i++)
		{
			const dContactGeom & ñontactGeom = sizedContactGeomBuff[i];
#ifdef _DEBUG
			Body * body1 = (Body*)dBodyGetData(dGeomGetBody(ñontactGeom.g1));
			Body * body2 = (Body*)dBodyGetData(dGeomGetBody(ñontactGeom.g2));
#endif
			dContact contact;
			contact.geom = ñontactGeom;
			contact.surface = surfaceParameters;
			dJointID jointContactID = dJointCreateContact(scene_->getWorldID(), contactJointGroupID_, &contact);
			dJointAttach(jointContactID, dGeomGetBody(ñontactGeom.g1), dGeomGetBody(ñontactGeom.g2));
		}
		dWorldStep(scene_->getWorldID(), getDTime());
		dJointGroupEmpty(contactJointGroupID_);

		scene_->update(time);
		return PeriodicActivity::operator()(time);
	}
private:
	Scene * scene_;
	dJointGroupID contactJointGroupID_;
};

}

}

Task::Task(Scene * scene) : scene_(scene)
{
	contactJointGroupID_ = dJointGroupCreate(maxContacts);
	surfaceParameters.mode = dContactBounce | dContactApprox1_1;
	surfaceParameters.mu = 0.03;
	surfaceParameters.bounce = 0.1;
	surfaceParameters.bounce_vel = 0.001;
	surfaceParameters.soft_cfm = 0.0001;
	const ModelTime stepSize = 0.01;
	activity_.reset(new Activity(scene_, contactJointGroupID_, stepSize));
	scheduler_.scheduleNow(activity_.get());
}

Task::~Task()
{
	scheduler_.remove(activity_.get());
	dJointGroupEmpty(contactJointGroupID_);
	dJointGroupDestroy(contactJointGroupID_);
}

ISystemScene * Task::getSystemScene() const
{
	return scene_;
}

void Task::operator()(Time time, ModelTime modelTime)
{
	scheduler_.run(modelTime);
}