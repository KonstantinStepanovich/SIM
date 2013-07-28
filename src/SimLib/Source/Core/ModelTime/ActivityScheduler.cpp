#include "Core/ModelTime/ActivityScheduler.hpp"
#include "Core/ModelTime/Activity.hpp"
#include <assert.h>

using namespace Sim;

void ActivityScheduler::PriorityQueue::remove(Activity * activity)
{
	container_type::iterator itSQC;

	class FindActivity
	{
	public:
		FindActivity(Activity * activity) : activity_(activity) {;}
		inline bool operator()(const value_type & schedule) const
		{
			return schedule.activity == activity_;
		}
	private:
		Activity * activity_;
	};

	while((itSQC = std::find_if(c.begin(), c.end(), FindActivity(activity))) != c.end())
		c.erase(itSQC);

	make_heap(c.begin(), c.end(), comp);
}

ActivityScheduler::ActivityScheduler()
{
}

ActivityScheduler::~ActivityScheduler()
{
	assert(priorityQueue_.empty());
}

void ActivityScheduler::run(ModelTime modelTime)
{
	Schedule schedule;
	while(	!priorityQueue_.empty() &&
			(schedule = priorityQueue_.top()).time < modelTime)
	{
		ModelTime nextTime = (*schedule.activity)(schedule.time);
		priorityQueue_.pop();
		schedule.activity->setScheduled_(false);
		if(nextTime > schedule.time)
		{
			schedule.time = nextTime;
			priorityQueue_.push(schedule);
			schedule.activity->setScheduled_(true);
		}
	}
}

void ActivityScheduler::schedule(ModelTime time, Activity * activity)
{
	Schedule schedule;
	schedule.time = time;
	schedule.activity = activity;
	priorityQueue_.push(schedule);
	activity->setScheduled_(true);
}

void ActivityScheduler::remove(Activity * activity)
{
	priorityQueue_.remove(activity);
	activity->setScheduled_(false);
}