#ifndef _KeyAxis_hpp_
#define _KeyAxis_hpp_

#include "Math/Control.hpp"

namespace Sim
{

namespace InputOIS
{

class KeyAxis
{
public:
	KeyAxis(float maxRate, ModelTime tau, float min, float max) : maxRate_(maxRate), axisValueRate_(tau),
		axisValue_(Math::Constraint<float>(min, max)), keyIncrease_(false), keyDecrease_(false) {;}
	inline void setKeyIncrease(bool key) { keyIncrease_ = key; }
	inline void setKeyDecrease(bool key) { keyDecrease_ = key; }
	inline float simulate(ModelTime dt)
	{
		if(isMoving())
			return constraint_(axisValue_.simulate(axisValueRate_.simulate(getMoveDirection() * maxRate_, dt), dt));
		else
			return axisValue_.get();
	}
	inline bool isMoving() const
	{
		return getMoveDirection() != 0;
	}
	inline int getMoveDirection() const
	{
		return (int)keyIncrease_ - (int)keyDecrease_;
	}
	inline float get() const
	{
		return axisValue_.get();
	}
private:
	const float maxRate_;
	bool keyIncrease_;
	bool keyDecrease_;
	Math::FirstOrderFilter<float> axisValueRate_;
	Math::Integral<float> axisValue_;
	Math::Constraint<float> constraint_;
};


}

}

#endif _KeyAxis_hpp_