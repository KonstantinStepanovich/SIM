#ifndef _Scalar_hpp_
#define _Scalar_hpp_

#include "bullet/LinearMath/btScalar.h"
#include <limits>

namespace Sim
{

typedef ::btScalar Real;

namespace Math
{

typedef ::btScalar Real;

template<typename T>
inline T invalid()
{
	return std::numeric_limits<T>::quiet_NaN();
}

template<typename T>
inline bool isInvalid(T t)
{
	return t == invalid<T>() || t != t;
}

template<typename T>
inline T min_max(const T & min, const T & value, const T & max)
{
	return std::min(max, std::max(min, value));
}

}

}

#endif _Scalar_hpp_