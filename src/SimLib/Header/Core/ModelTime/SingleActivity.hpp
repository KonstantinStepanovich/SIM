#ifndef _SingleActivity_hpp_
#define _SingleActivity_hpp_

#include "Core/ModelTime/ModelTime.hpp"

#include <queue>

namespace Sim
{

class Activity;

class SingleActivity 
{
public:
	SingleActivity();
	void schedule(Activity * activity, ModelTime callTime);
	void run(ModelTime modelTime);
	bool empty() const { return activity_ == NULL; }
private:
	Activity * activity_;
	ModelTime callTime_;
};

}

#endif _SingleActivity_hpp_