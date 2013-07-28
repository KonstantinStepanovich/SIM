#ifndef _Polynom_hpp_
#define _Polynom_hpp_

#include <algorithm>
#include <math.h>

namespace Sim
{

namespace Math
{

template<typename T, size_t size>
T evalPolynom(const T (&coefs)[size], const T & arg)
{
	T val = T(0);
	for(size_t i = 0; i < size; i++)
		val += coefs[i] * pow(arg, (int)i);
	return val;
}

template<typename T, size_t order>
class Polynom
{
public:
	enum  { COEFS_COUNT = order + 1 };
	Polynom()
	{
		std::fill(coeffs_,coeffs_ + COEFS_COUNT, T(0));
	}
	Polynom(const T (&coefs)[COEFS_COUNT])
	{
		set(coefs);
	}
	void set(const T (&coefs)[COEFS_COUNT])
	{
		std::copy(coefs, coefs + COEFS_COUNT, coeffs_);
	}
	T eval(const T & arg) const
	{
		return evalPolynom(coeffs_, arg);
	}
	T * getCoeffs()
	{
		return coeffs_;
	}
	const T * getCoeffs() const
	{
		return coeffs_;
	}
private:
	T coeffs_[COEFS_COUNT];
};

}

}

#endif _Polynom_hpp_