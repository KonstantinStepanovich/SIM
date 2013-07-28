#ifndef _ACTIVITY_SCHEDULER_HPP_
#define _ACTIVITY_SCHEDULER_HPP_

#include "Core/ModelTime/ModelTime.hpp"

#include <queue>

namespace Sim
{

class Activity;

class ActivityScheduler
{
public:
	ActivityScheduler();
	~ActivityScheduler();
	void reset();
	void run(ModelTime modelTime);
	void schedule(ModelTime time, Activity * activity);
	inline void scheduleNow(Activity * activity)
	{
		schedule(0.0, activity);
	}
	void remove(Activity * activity);
	inline bool empty() const { return priorityQueue_.empty(); }
private:
	struct Schedule
	{	
		ModelTime	time;
		Activity *	activity;
	};
	struct TimeLess
	{
		inline bool operator()(const Schedule & left, const Schedule & right)
		{
			return left.time > right.time;
		}
	};
	typedef std::vector<Schedule> ScheduleQueueCont;
	class PriorityQueue : public std::priority_queue<	Schedule,
														ScheduleQueueCont,
														TimeLess>
	{
	public:
		void remove(Activity * activity);
	};
	PriorityQueue priorityQueue_;
};

}

#endif _ACTIVITY_SCHEDULER_HPP_