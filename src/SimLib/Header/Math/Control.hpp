#ifndef _Control_hpp_
#define _Control_hpp_

#include <xutility>
#include <limits>

namespace Sim
{

namespace Math
{

template<typename T>
class Constraint
{
public:
	Constraint() : min_(-std::numeric_limits<T>::infinity()), max_(std::numeric_limits<T>::infinity()) {;}
	Constraint(const T & min, const T & max) : min_(min), max_(max) {;}
	T operator()(const T & value) const
	{
		return std::max<T>(min_, std::min<T>(value, max_));	
	}
private:
	T min_;
	T max_;
};

template<typename T, typename TTau = T>
class FirstOrderFilter
{
public:
	FirstOrderFilter(const TTau & tau, const T & value = (T)0) : tau_(tau), value_(value) {;}
	void reset(const T & target)
	{
		value_ = target;
	}
	const T & simulate(const T & target, const TTau & dt)
	{
		return value_ += (target - value_) * dt / tau_;
	}
	inline const T & get() const { return value_; }
private:
	const TTau tau_;
	T value_;
};

template<typename T>
class Derivative
{
public:
	Derivative(const T & value = T(0)) : prevValue_(value), der_(T(0)) {;}
	void reset(const T & value)
	{
		prevValue_ = value;
		der_ = T(0);
	}
	const T & simulate(const T & value, const T & dt)
	{
		der_ = (value - prevValue_) / dt;
		prevValue_ = value;
		return der_;
	}
	inline const T & get() const { return der_; }
private:
	T prevValue_;
	T der_;
};

template<typename T>
class Integral
{
public:
	Integral() : intg_(T(0)) {;}
	Integral(const Constraint<T> & constraint) : intg_(T(0)), constraint_(constraint) {;}
	void reset()
	{
		intg_ = T(0);
	}
	const T & simulate(const T & value, const T & dt)
	{
		return intg_ = constraint_(intg_ += value * dt);
	}
	inline const T & get() const { return intg_; }
private:
	T intg_;
	Constraint<T> constraint_;
};

template<typename T>
class PID
{
public:
	PID()
	{
		std::fill(coeff_, coeff_ + 3, T(0.0));
	}
	PID(const T(&coeff)[3])
	{
		std::copy(coeff_, coeff, coeff + 3);
	}
	void reset()
	{
		integral_.reset();
		derrivative_.reset(0.0f);
	}
	const T & simulate(const T & delta, const T & dt)
	{
		value_ = coeff_[1] * delta + coeff_[2] * integral_.simulate(delta, dt) + coeff_[3] * derrivative_.simulate(delta, dt);
	}
	inline const T & get() const { return value_; }
private:
	T coeff_[3];
	Integral<float> integral_;
	Derivative<float> derrivative_;
	T value_;
};

}

}

#endif _Control_hpp_