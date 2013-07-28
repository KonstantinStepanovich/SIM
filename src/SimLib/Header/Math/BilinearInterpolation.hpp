#ifndef _BilinearInterpolation_hpp_
#define _BilinearInterpolation_hpp_

#include "Math/LinearInterpolation.hpp"
#include <functional>
#include <assert.h>

namespace Sim
{

namespace Math
{

template<typename T>
inline T interpolateBilinear(	T rowArg1, T rowArg2,
								T colArg1, T colArg2,
								T val11, T val12,
								T val21, T val22,
								T rowArg, T colArg)
{
	T R1 = interpolateLinear(colArg1, val11, colArg2, val12, colArg);
	T R2 = interpolateLinear(colArg1, val21, colArg2, val22, colArg);
	return interpolateLinear(rowArg1, R1, rowArg2, R2, rowArg);
}

struct Table2DIndex
{
	typedef TableIndex SubIndex;
	Table2DIndex(SubIndex rowIn, SubIndex colIn) : row(rowIn), col(colIn) {;}
	SubIndex row;
	SubIndex col;
};

typedef Table2DIndex Size2D;

template<typename A, typename T, class ScalarsAcess = ScalarsAccessSelf<T>>
class BilinearInterpolator
{
public:
	typedef Table2DIndex Index;
	typedef Size2D Size;
	typedef A Argument;
	typedef T Value;
	BilinearInterpolator(const ScalarsAcess & scalarsAccess = ScalarsAcess()) : scalarsAccess_(scalarsAccess)
	{
	}
	void resize(size_t rowCount, size_t colCount)
	{
		rowArgs_.resize(rowCount);
		colArgs_.resize(colCount);
		values_.resize(rowCount * colCount);
	}
	void setRowArg(size_t index, A arg)
	{
		rowArgs_[index] = arg;
	}
	void insertRowArg(size_t index, A arg)
	{
		rowArgs_.insert(rowArgs_.begin() + index, arg);
		values_.resize(values_.size() + colArgs_.size());
		std::copy_backward(	values_.begin() + index * colArgs_.size(),
							values_.begin() + index * colArgs_.size() + colArgs_.size(), 
							values_.begin() + index * colArgs_.size() + colArgs_.size() + 1);
	}
	void removeRowArg(size_t index)
	{
		std::copy(	values_.begin() + index * colArgs_.size(),
					values_.begin() + index * colArgs_.size() + colArgs_.size(),
					values_.begin() + (index - 1) * colArgs_.size());
		rowArgs_.erase(rowArgs_.begin() + index);
	}
	void setColArg(size_t index, A arg)
	{
		colArgs_[index] = arg;
	}
	void insertColArg(size_t index, A arg)
	{
		values_.resize(values_.size() + rowArgs_.size());
		for(size_t r = rowArgs_.size(); r > 0; --r)
		{
			std::copy_backward(	values_.begin() + (r - 1) * colArgs_.size() + index,
								values_.begin() + (r - 1) * colArgs_.size() + colArgs_.size(),
								values_.begin() + (r - 1) * (colArgs_.size() + 1) + colArgs_.size() + 1);
			std::copy_backward(	values_.begin() + (r - 1) * colArgs_.size(),
								values_.begin() + (r - 1) * colArgs_.size() + index,
								values_.begin() + (r - 1) * (colArgs_.size() + 1) + index);
		}
		colArgs_.insert(colArgs_.begin() + index, arg);
	}
	void removeColArg(size_t index)
	{
		//for(size_t r = rowArgs_.size() - 1; r >= 0; --r)
		//{
		//	std::copy_backward(	values_.begin() + r * colArgs_.size() + index,
		//						values_.begin() + r * colArgs_.size() + colArgs_.size(),
		//						);
		//	std::copy_backward(	values_.begin() + r * colArgs_.size(),
		//						values_.begin() + r * colArgs_.size() + index,
		//						values_.begin() + r * (colArgs_.size() + 1));
		//}
		//values_.resize(values_.size() - rowArgs_.size());
		//colArgs_.erase(colArgs_.begin() + index);
	}
	void setValue(const Index & index, const T & val)
	{
		getValue_(index) = val;
	}
	void setValue(size_t rowIndex, size_t colIndex, const T & val)
	{
		setValue(Index(rowIndex, colIndex), val);
	}
	const T & getValue(const Index & index) const
	{
		return getValue_(index);
	}
	const T & getValue(size_t rowIndex, size_t colIndex) const
	{
		return getValue_(Index(rowIndex, colIndex));
	}
	T & getValue(const Index & index)
	{
		return getValue_(index);
	}
	T & getValue(size_t rowIndex, size_t colIndex)
	{
		return getValue_(Index(rowIndex, colIndex));
	}
	inline Size size() const
	{
		return Size(rowArgs_.size(), colArgs_.size());
	}
	const A & getRowArg(TableIndex rowIndex) const
	{
		return rowArgs_[rowIndex];
	}
	A & getRowArg(TableIndex rowIndex)
	{
		return rowArgs_[rowIndex];
	}
	const A & getColArg(TableIndex colIndex) const
	{
		return colArgs_[colIndex];
	}
	A & getColArg(TableIndex colIndex)
	{
		return colArgs_[colIndex];
	}
	Index findIndex(A rowArg, A colArg) const
	{
		return Index(	findArg(rowArgs_.begin(), rowArgs_.end(), rowArg, std::less<A>()) - rowArgs_.begin(),
						findArg(colArgs_.begin(), colArgs_.end(), colArg, std::less<A>()) - colArgs_.begin());
	}	
	Index findIndex(A rowArg, A colArg, const Index & cached) const
	{
		return Index(	findArg(rowArgs_.begin(), rowArgs_.end(), rowArg, rowArgs_.begin() + cached.row, std::less<A>()) - rowArgs_.begin(),
						findArg(colArgs_.begin(), colArgs_.end(), colArg, colArgs_.begin() + cached.col, std::less<A>()) - colArgs_.begin());
	}
	template<class ScalarAccess>
	typename ScalarAccess::Type eval(A rowArg, A colArg, const Index & index1, const Index & index2, const ScalarAccess & scalarAccess) const
	{
		return interpolateBilinear(	*(rowArgs_.begin() + index1.row), 							*(rowArgs_.begin() + index2.row),
									*(colArgs_.begin() + index1.col), 							*(colArgs_.begin() + index2.col),
									scalarAccess(getValue_(Index(index1.row, index1.col))), 	scalarAccess(getValue_(Index(index1.row, index2.col))),
									scalarAccess(getValue_(Index(index2.row, index1.col))),		scalarAccess(getValue_(Index(index2.row, index2.col))),
									rowArg,														colArg);
	}
	template<class ScalarAccess>
	typename ScalarAccess::Type eval(A rowArg, A colArg, const Index & index, const ScalarAccess & scalarAccess) const
	{
		return eval(rowArg, colArg, index, Index(index.row + 1, index.col + 1), scalarAccess);
	}
	Value eval(A rowArg, A colArg, const Index & index1, const Index & index2) const
	{
		Value value;
		for(size_t i = 0; i < ScalarsAcess::SIZE; ++i)
		{
			scalarsAccess_(value, i) = 
				interpolateBilinear(	*(rowArgs_.begin() + index1.row), 								*(rowArgs_.begin() + index2.row),
										*(colArgs_.begin() + index1.col), 								*(colArgs_.begin() + index2.col),
										scalarsAccess_(getValue_(Index(index1.row, index1.col)), i), 	scalarsAccess_(getValue_(Index(index1.row, index2.col)), i),
										scalarsAccess_(getValue_(Index(index2.row, index1.col)), i),	scalarsAccess_(getValue_(Index(index2.row, index2.col)), i),
										rowArg,															colArg);
		}
		return value;
	}
	typename Value eval(A rowArg, A colArg, const Index & index) const
	{
		return eval(rowArg, colArg, index, Index(index.row + 1, index.col + 1));
	}
private:
	T & getValue_(const Index & index)
	{
		assert(index.row < rowArgs_.size());
		assert(index.col < colArgs_.size());
		return values_[index.row * colArgs_.size() + index.col];
	}
	const T & getValue_(const Index & index) const
	{
		assert(index.row < rowArgs_.size());
		assert(index.col < colArgs_.size());
		return values_[index.row * colArgs_.size() + index.col];
	}
	ScalarsAcess scalarsAccess_;
	typedef std::vector<A> ArgArray;
	ArgArray rowArgs_;
	ArgArray colArgs_;
	typedef std::vector<T> ValArray;
	ValArray values_;
};

}

}

#endif _BilinearInterpolation_hpp_