#ifndef _Interpolation_hpp_
#define _Interpolation_hpp_

#include <algorithm>
#include <assert.h>

namespace Sim
{

namespace Math
{

template<typename RanIt, typename T, class Pred>
inline RanIt findArg(RanIt begin, RanIt end, T arg, Pred pred)
{
	return std::min(std::upper_bound(begin, end, arg, pred) - 1, end - 2);
}

template<typename RanIt, typename T, class Pred>
inline RanIt findArg(RanIt begin, RanIt end, T arg, RanIt cached, Pred pred)
{
	assert(cached < end);
	if(pred(arg, *cached))
		return findArg(begin, cached + 1, arg, pred);
	else if(!pred(arg, *(cached + 1)))
		return findArg(cached + 1, end, arg, pred);
	else
		return cached;
}

typedef size_t TableIndex;

template<typename Data, typename T>
struct CompareArg
{
	inline bool operator()(T left, const Data & right) const
	{
		return left < right.arg;
	}
	inline bool operator()(const Data & left, T right) const
	{
		return left.arg < right;
	}
	inline bool operator()(const Data & left, const Data & right) const
	{
		return left.arg < right.arg;
	}
};

template<typename T>
struct ScalarAccessSelf
{
	typedef T Type;
	inline T & operator()(T & t) const { return t; }
	inline const T & operator()(const T & t) const { return t; }
};

template<typename T, typename M>
struct ScalarAccessStructMember
{
	typedef M Type;
	ScalarAccessStructMember(M T::*ptr) : ptr_(ptr) {;}
	inline M & operator()(T & t) const { return t.*ptr_; }
	inline const M & operator()(const T & t) const { return t.*ptr_; }
private:
	M T::*ptr_;
};

template<typename T, typename M>
inline ScalarAccessStructMember<T, M> makeStructMember(M T::*ptr)
{
	return ScalarAccessStructMember<T, M>(ptr);
}

template<typename T>
struct ScalarAccessArrayElement
{
	typedef T Type;
	ScalarAccessArrayElement(size_t index) : index_(index) {;}
	inline T & operator()(T & t) const { return t[index_]; }
	inline const T & operator()(const T & t) const { return t[index_]; }
private:
	size_t index_;
};

template<typename T>
struct ScalarsAccessSelf
{
	enum { SIZE = 1 };
	typedef T Type;
	T & operator()(T & t, size_t index) const { return t; };
	const T & operator()(const T & t, size_t index) const { return t; };
};

}

}

#endif _Interpolation_hpp_