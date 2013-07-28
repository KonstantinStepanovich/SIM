#include "stdafx.h"

#include "Core/ModelTime/Activity.hpp"
#include "Core/ModelTime/ActivityScheduler.hpp"
#include "ModelTimer.hpp"

using namespace Sim;

class SimpleActivity : public Sim::Activity
{
public:
	SimpleActivity() : num_(0) {;}
private:
	virtual ModelTime operator()(ModelTime time)
	{
		printf("step %3.0f %d\n", time, num_++);
		return 0.0;
	}
	unsigned int num_;
};

int _tmain(int argc, _TCHAR* argv[])
{
	ActivityScheduler scheduler;
	SimpleActivity sa;
	scheduler.schedule(8.0, &sa);
	scheduler.schedule(2.0, &sa);
	scheduler.schedule(3.0, &sa);
	while(!scheduler.empty())
	{
		scheduler.run(Framework::ModelTimer::instance().execute(0));
	}
	return 0;
}