#include "stdafx.h"

#include "Core/ModelTime/SingleActivity.hpp"
#include "Core/ModelTime/Activity.hpp"

using namespace Sim;

SingleActivity::SingleActivity() : activity_(NULL), callTime_(0.0)
{
}

void SingleActivity::schedule(Activity * activity, ModelTime callTime)
{
	activity_ = activity;
	callTime_ = callTime;
}

void SingleActivity::run(ModelTime modelTime)
{
	if(activity_ != NULL)
	{
		while(callTime_ < modelTime)
		{
			callTime_ = (*activity_)(callTime_);
		}
	}
}