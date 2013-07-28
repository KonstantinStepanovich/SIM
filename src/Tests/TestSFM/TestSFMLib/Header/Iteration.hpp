#ifndef _Iteration_hpp_
#define _Iteration_hpp_

#include <vector>
#include <list>

#include "Math/LinearInterpolation.hpp"
#include "Math/BilinearInterpolation.hpp"

template<typename T>
struct IterationRange
{
	T start;
	T end;
	T step;
};

template<typename T>
void scale(IterationRange<typename T> & iteration, T coeff)
{
	iteration.start *= coeff;
	iteration.end *= coeff;
	iteration.step *= coeff;
}

template<typename A, typename T, class S>
inline void read(const Sim::Math::LinearInterpolator<A, T, S> & linterp, IterationRange<A> & iteration)
{
	iteration.start = linterp.getElement(0).arg;
	iteration.end = linterp.getElement(linterp.size() - 1).arg;
	iteration.step = (iteration.end - iteration.start) / (linterp.size() - 1);
}

template<typename A, typename T, class S>
inline void readRow(const Sim::Math::BilinearInterpolator<A, T, S> & bilinterp, IterationRange<A> & iteration)
{
	iteration.start = bilinterp.getRowArg(0);
	iteration.end = bilinterp.getRowArg(bilinterp.size().row - 1);
	iteration.step = (iteration.end - iteration.start) / (bilinterp.size().row - 1);
}

template<typename A, typename T, class S>
inline void readCol(const Sim::Math::BilinearInterpolator<A, T, S> & bilinterp, IterationRange<A> & iteration)
{
	iteration.start = bilinterp.getColArg(0);
	iteration.end = bilinterp.getColArg(bilinterp.size().col - 1);
	iteration.step = (iteration.end - iteration.start) / (bilinterp.size().col - 1);
}

template<typename T, class Proc>
inline void iterate(T start, T end, T step, Proc & proc)
{
	T val = start;
	while(val < end)
	{
		proc(val);
		val += step;
	}
	proc(end);
}

template<typename T, class Proc>
inline void iterate(const IterationRange<T> & iteration, Proc & proc)
{
	iterate(iteration.start, iteration.end, iteration.step, proc);
}

template<typename T, class Proc>
inline void iterate(std::vector<T> & v, Proc & proc)
{
	std::for_each(v.begin(), v.end(), proc);
}

template<typename T, class Proc>
inline void iterate(std::list<T> & l, Proc & proc)
{
	std::for_each(l.begin(), l.end(), proc);
}

template<typename It, class Proc>
inline void iterate(It start, It end, Proc & proc)
{
	std::for_each(start, end, proc);
}

template<typename T, class Proc>
inline void iterate(const T * ptr, size_t size, Proc & proc)
{
	iterate(ptr, ptr + size, proc);
}

template<typename T, size_t size, class Proc>
inline void iterate(const T (&array)[size],Proc & proc)
{
	iterate(array, array + size, proc);
}

template<typename T, class Proc>
inline void iterate(const std::vector<T> & cont, Proc & proc)
{
	iterate(cont.begin(), cont.end(), proc);
}

template<typename T, class Proc>
inline void iterate(const std::list<T> & cont, Proc & proc)
{
	iterate(cont.begin(), cont.end(), proc);
}

#endif _Iteration_hpp_