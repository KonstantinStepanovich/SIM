#ifndef _ACTIVITY_HPP_
#define _ACTIVITY_HPP_

#include "Core/ModelTime/ModelTime.hpp"
#include <assert.h>

namespace Sim
{

class Activity
{
public:
	Activity() : scheduled_(false) {;}
	virtual ~Activity()
	{
		assert(!scheduled_);
	}
	virtual ModelTime operator()(ModelTime time) = 0;
private:
	friend class ActivityScheduler;
	inline void setScheduled_(bool scheduled) { scheduled_ = scheduled; }
	bool scheduled_;
};

class PeriodicActivity : public Activity
{
public:
	PeriodicActivity(ModelTime dtime) : dtime_(dtime) {;}
	virtual ModelTime operator()(ModelTime time)
	{
		return time + dtime_;
	}
	inline void setDTime(ModelTime dtime) { dtime_ = dtime; }
	inline ModelTime getDTime() const { return dtime_; }
private:
	ModelTime dtime_;
};

}

#endif _ACTIVITY_HPP_