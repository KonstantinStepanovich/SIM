#ifndef _MODEL_TIMER_HPP_
#define _MODEL_TIMER_HPP_

#include "Time.hpp"
#include "Core/ModelTime/ModelTime.hpp"
#include <utility>

namespace Sim
{

namespace Framework
{

//Model timer returns model time value each step
class ModelTimer
{
	ModelTimer();
public:
	static ModelTimer & instance();
	//Time control
	typedef std::pair<long, long> Scale;
	inline void pause(bool on) { paused_ = on; }
	inline void normalize() { scale_ = Scale(1, 1); }
	inline void accelerate(long scale) { scale_ = Scale(scale, 1); }
	inline void deccelerate(long scale) { scale_ = Scale(1, scale); }
	inline bool isPaused() const { return paused_; }
	inline const Scale & getScale() const { return scale_; }
	//Action
	void start(Time time) { prevTime_ = time; modelTime_ = 0.0; }
	ModelTime execute(Time time);
private:
	bool		paused_;
	Scale		scale_;
	Time		prevTime_;
	ModelTime	modelTime_;
};

}

}

#endif _MODEL_TIMER_HPP_