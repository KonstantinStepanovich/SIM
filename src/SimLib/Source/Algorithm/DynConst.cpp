#include "stdafx.h"
#include "Algorithm/DynConst.cpp"
#include "Core/Exception.hpp"
#include <string>
#include <map>

using namespace Sim;

class DynConstEnumerator
{
public:
	DynConstEnumerator & instance()
	{
		static DynConstEnumerator dynConstEnumerator;
		return dynConstEnumerator;
	}
	DynConstType acquire(const char * str)
	{
		DynConstType & number = numbersMap_[str];
		if(number == 0)
			number = ++counter_;
		return number;
	}
private:
	DynConstEnumerator() : number_(0) {;}
	DynConstType counter_;
	typedef std::map<std::string, DynConstType> NumbersMap;
	NumbersMap numbersMap_;
};

DynConstType DynConst::operator DynConstType() const
{
	if(number_ == 0)
		number_ = DynConstEnumerator::instance().acquire(val_);
	return number_;
}

DynConstType DynConstFlag::operator DynConstType() const
{
	if(flag_ == 0)
	{
		unsigned int bit = dynConst_;
		if(bit >= DynConstTypeData<DynConstType>::getBitSize())
			throw Exception(OSS("Bit overflow in DynConstFlag"));
		flag_ = DynConstTypeData<DynConstType>::setBit(flag_, bit);
	}
	return flag_;
}