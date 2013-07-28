#ifndef _Actuators_hpp_
#define _Actuators_hpp_

#include "Math/Control.hpp"
#include "Core/ModelTime/ModelTime.hpp"

#include <xutility>

namespace Sim {

namespace SFM {

class ConstRateActuator
{
public:
	ConstRateActuator() : rate_(0.0f), value_(0.0f) {;}
	ConstRateActuator(float rate) : rate_(rate), value_(0.0f) {;}
	void reset(float value)
	{
		value_ = value;
	}
	inline void simulate(float reqValue, ModelTime dt)
	{
		float dv = rate_ * (float)dt;
		if(reqValue < value_)
			dv = -dv;
		value_ += dv;
	}
	inline float getValue() const { return value_; }
private:
	const float rate_;
	float value_;
};

class InertRateActuator
{
public:
	InertRateActuator(float maxRate, ModelTime tau) : maxRate_(maxRate), tau_(tau), value_(0.0f), rate_(tau, 0.0f) {;}
	void reset(float value)
	{
		value_ = value;
		rate_.reset(0.0f);
		controller_.reset();
	}
	void simulate(float reqValue, ModelTime dt)
	{
		float delta = reqValue - value_;
		float rateReq = controller_.simulate(delta, dt);
		float rate = rate_.simulate(rateReq, dt);
		value_ += rate * dt;
	}
	inline float getValue() const { return value_; }
private:
	const float maxRate_;
	Math::PID<float> controller_;
	Math::FirstOrderFilter<float, ModelTime> rate_;
	const float tau_;
	float value_;
};

}

}

#endif _Actuators_hpp_