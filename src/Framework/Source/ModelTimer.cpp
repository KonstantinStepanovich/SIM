#include "stdafx.h"
#include "ModelTimer.hpp"

using namespace Sim;
using namespace Sim::Framework;

ModelTimer::ModelTimer() : paused_(false), scale_(1, 1), prevTime_(0), modelTime_(0.0)
{

}

ModelTimer & ModelTimer::instance()
{
	static ModelTimer modelTimer;
	return modelTimer;
}

ModelTime ModelTimer::execute(Time time)
{
	if(!paused_)
	{
#ifdef _DEBUG
		static const Time antiFreeze = (Time)2E9;
		Time dtime = time - prevTime_;
		if(dtime < antiFreeze)
			modelTime_ += (ModelTime)1E-9 * ((time - prevTime_) * scale_.first / scale_.second);
#else
		modelTime_ += (ModelTime)1E-9 * ((time - prevTime_) * scale_.first / scale_.second);
#endif
	}
	prevTime_ = time;
	return modelTime_;
}