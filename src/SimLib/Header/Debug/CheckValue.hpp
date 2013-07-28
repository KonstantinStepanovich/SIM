#ifndef _CheckValue_hpp_
#define _CheckValue_hpp_

#include <limits>
#include <algorithm>
#include <assert.h>

namespace Sim
{

template<typename T>
void validateScalar(T t)
{
	assert(t == t);
	assert(t != std::numeric_limits<T>::quiet_NaN());
	assert(t != std::numeric_limits<T>::signaling_NaN());
	assert(t != std::numeric_limits<T>::infinity());
	assert(t != -std::numeric_limits<T>::infinity());
}

template<typename T>
inline void validate(const T t)
{
	validateScalar(t);
}

template<typename T>
inline void validate(const T * t, size_t size)
{
	std::for_each(t, t + size, &validateScalar<T>);
}

template<typename T, size_t size>
inline void validate(const T (&t)[size])
{
	validate(&t[0], size);
}

template<typename T, size_t size1, size_t size2>
inline void validate(const T (&t)[size1][size2])
{
	validate(&t[0], size1 * size2);
}

}

#endif _CheckValue_hpp_