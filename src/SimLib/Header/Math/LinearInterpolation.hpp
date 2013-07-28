#ifndef __LinearInterpolation_hpp_
#define __LinearInterpolation_hpp_

#include <vector>
#include <assert.h>

#include "Math/Interpolation.hpp"

namespace Sim
{

namespace Math
{

template<typename T>
inline T interpolateLinear(T arg1, T val1, T arg2, T val2, T arg)
{
	return arg1 == arg2 ? val1 : val1 + (val2 - val1) / (arg2 - arg1) * (arg - arg1);
}

template<typename A, typename T, class Scalars = ScalarsAccessSelf<T>>
class LinearInterpolator
{
public:
	typedef A Argument;
	typedef T Value;
	struct Element
	{
		typedef A Argument;
		typedef T Value;
		A arg;
		T val;
	};
	typedef TableIndex Index;
	typedef Index Size;
	LinearInterpolator(const Scalars & scalars = Scalars())
	{
	}
	template<typename RanIt>
	LinearInterpolator(RanIt elBegin, RanIt elEnd)
	{
		table_.resize(elEnd - elBegin);
		std::copy(elBegin, elEnd, table_.begin());
	}
	inline void resize(Size size)
	{
		table_.resize(size);
	}
	void set(Index index, A arg, const T & val)
	{
		Element & element = table_[index];
		element.arg = arg;
		element.val = val;
	}
	void insert(A arg)
	{
		Index index = findIndex(arg, index);
		Element & element = *table_.insert(table_.begin() + index, Element());
		element.arg = arg;
		element.val = eval(arg, index);
	}
	void insert(A arg, const T & val)
	{
		Index index = findIndex(arg);
		insert(index, arg, val);
	}
	void insert(Index index, A arg, const T & val)
	{
		assert(index == 0 || table_.begin()->arg != arg);
		assert(index == table_.size() || (table_.begin() + index)->arg != arg);
		Element & element = *table_.insert(table_.begin() + index, Element());
		element.arg = arg;
		element.val = val;
	}
	void remove(Index index)
	{
		table_.erase(table_.begin() + index);
	}
	inline Size size() const { return table_.size(); }
	const Element & getElement(Index index) const { return table_[index]; }
	Element & getElement(Index index) { return table_[index]; }
	inline Index findIndex(A arg) const
	{
		return findArg(table_.begin(), table_.end(), arg, CompareArg<Element, A>()) - table_.begin();
	}
	inline Index findIndex(A arg, Index index) const
	{
		return findArg(table_.begin(), table_.end(), arg, table_.begin() + index, CompareArg<Element, A>()) - table_.begin();
	}
	template<class Scalar>
	inline typename Scalar::Type eval(A arg, size_t index1, size_t index2, const Scalar & scalar) const
	{
		return interpolateLinear(table_[index1].arg, scalar(table_[index1].val), table_[index2].arg, scalar(table_[index2].val), arg);
	}
	template<class Scalar>
	inline typename Scalar::Type eval(A arg, size_t index, const Scalar & scalar) const
	{
		return eval(arg, index, index + 1, scalar);
	}
	inline Value eval(A arg, size_t index1, size_t index2) const
	{
		Value value;
		for(size_t i = 0; i < Scalars::SIZE; ++i)
			//if(index == 0)
			//	scalars_(value, i) = scalars_(table_[0].val, i);
			//else if(index == table_.size() - 1)
			//	scalars_(value, i) = scalars_(table_[size() - 1].val, i);
			//else
				scalars_(value, i) = interpolateLinear(table_[index1].arg, scalars_(table_[index1].val, i), table_[index2].arg, scalars_(table_[index2].val, i), arg);
		return value;
	}
	inline Value eval(A arg, size_t index) const
	{
		return eval(arg, index, index + 1);
	}
private:
	typedef std::vector<Element> Table;
	Table table_;
	Scalars scalars_;
};

}

}

#endif __LinearInterpolation_hpp_