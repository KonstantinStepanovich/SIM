#ifndef _Value_hpp_
#define _Value_hpp_

#include <vector>
#include <list>

extern "C"
{
#include "DataModelAPI.h"
}

#include "Algorithm/Exception.hpp"

namespace DataModel
{

DECLARE_EXCEPTION(ValueException, Sim::Exception)

class Value;

struct ItemTable
{
	ItemTable() : field(NULL), value(NULL) {;}
	ItemTable(const char * fieldIn, Value * valueIn) : field(fieldIn), value(valueIn) {;}
	const char * field;
	Value * value;
};

struct ItemTableC
{
	ItemTableC() : field(NULL), value(NULL) {;}
	ItemTableC(const char * fieldIn, const Value * valueIn) : field(fieldIn), value(valueIn) {;}
	ItemTableC(const ItemTable & itemTable) : field(itemTable.field), value(itemTable.value) {;}
	const char * field;
	const Value * value;
};

class ChangeHandler
{
public:
	virtual ~ChangeHandler();
	virtual void operator()(Value * value) = 0;
};

class Value
{
public:
	typedef ValueException Exception;
	enum Type { VALUE_TYPE_NULL, VALUE_TYPE_INTEGER, VALUE_TYPE_REAL,
				VALUE_TYPE_ARRAY, VALUE_TYPE_TABLE, VALUE_TYPE_MAX };
	Value(ChangeHandler * changeHandlerIn);
	virtual ~Value();
	virtual Type				getType		() const;
	virtual dmInteger 			getInteger	() const;
	virtual const dmLimitsInt *	getLimitsInt() const;
	virtual void				setInteger	(dmInteger val);
	virtual dmReal				getReal		() const;
	virtual const dmLimitsReal *	getLimitsReal() const;
	virtual void				setReal		(dmReal val);	
	virtual dmIndex				getSize		() const;
	virtual Value *				index		(dmIndex index);
	virtual const Value *		index		(dmIndex index) const;
	virtual Value *				indexArray	(dmIndex index);
	virtual const Value *		indexArray	(dmIndex index) const;
	virtual Value*				addItem		(dmIndex index);
	virtual void				removeItem	(dmIndex index);
	virtual ItemTable			indexTable	(dmIndex index);
	virtual ItemTableC			indexTable	(dmIndex index) const;
	virtual void				notifyChange();
public:
	static unsigned int			getType		(const dmValueHandle handle);
	static dmInteger 			getInteger	(const dmValueHandle handle);
	static const dmLimitsInt *	getLimitsInt(const dmValueHandle handle);
	static void					setInteger	(dmValueHandle handle, dmInteger val);
	static dmReal				getReal		(const dmValueHandle handle);
	static const dmLimitsReal *	getLimitsReal(const dmValueHandle handle);
	static void					setReal		(dmValueHandle handle, dmReal val);	
	static dmIndex				getSize		(const dmValueHandle handle);
	static dmValueHandle		index		(dmValueHandle handle, dmIndex index);
	static const dmValueHandle	indexC		(const dmValueHandle handle, dmIndex index);
	static dmValueHandle		indexArray	(dmValueHandle handle, dmIndex index);
	static const dmValueHandle	indexArrayC	(const dmValueHandle handle, dmIndex index);
	static dmValueHandle		addItem		(dmValueHandle handle, dmIndex index);
	static void					removeItem	(dmValueHandle handle, dmIndex index);
	static dmItemTable			indexTable	(dmValueHandle handle, dmIndex index);
	static dmItemTableC			indexTableC	(const dmValueHandle handle, dmIndex index);
	static void					notifyChange(dmValueHandle handle);
protected:
	ChangeHandler * changeHandler;
};

template<typename T>
class ValueInteger : public Value
{
public:
	ValueInteger(T * val, ChangeHandler * changeHandlerIn, const dmLimitsInt * limitsInt = NULL) : Value(changeHandlerIn), val_(val), limitsInt_(limitsInt) {;}
	virtual Type		getType		() const { return VALUE_TYPE_INTEGER; }
	virtual dmInteger 	getInteger	() const { return (dmInteger)*val_; };
	virtual const dmLimitsInt * getLimitsInt() const { return limitsInt_; }
	virtual void		setInteger	(dmInteger val)
	{
		*val_ = (T)val;
		(*changeHandler)(this);
	}
private:
	T * val_;
	const dmLimitsInt * limitsInt_;
};

template<typename T>
inline ValueInteger<T> * createValueInteger(T & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL)
{
	return new ValueInteger<T>(&val, changeHandler, limitsInt);
}

inline ValueInteger<bool> * 			createValue(bool & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL){ return createValueInteger(val, changeHandler, limitsInt); }
inline ValueInteger<short> * 			createValue(short & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }
inline ValueInteger<unsigned short> * 	createValue(unsigned short & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }
inline ValueInteger<int> * 				createValue(int & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }
inline ValueInteger<unsigned int> * 	createValue(unsigned int & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }
inline ValueInteger<long> * 			createValue(long & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }
inline ValueInteger<unsigned long> * 	createValue(unsigned long & val, ChangeHandler * changeHandler, const dmLimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }
//inline ValueInteger<size_t> * 			createValue(size_t * val, ChangeHandler * changeHandler, const LimitsInt * limitsInt = NULL) { return createValueInteger(val, changeHandler, limitsInt); }

template<typename T>
class ValueReal : public Value
{
public:
	ValueReal(T * val, ChangeHandler * changeHandlerIn, const dmLimitsReal * limitsReal = NULL) : 
	  Value(changeHandlerIn), val_(val), limitsReal_(limitsReal) {;}
	virtual Type		getType		() const { return VALUE_TYPE_REAL; }
	virtual dmReal 		getReal	() const { return (dmReal)*val_; };
	virtual const dmLimitsReal * getLimitsReal() const { return limitsReal_; }
	virtual void		setReal	(dmReal val)
	{
		*val_ = (T)val;
		(*changeHandler)(this);
	}
private:
	T * val_;
	const dmLimitsReal * limitsReal_;
};

template<typename T>
inline ValueReal<T> * createValueReal(T & val, ChangeHandler * changeHandler, const dmLimitsReal * limitsReal = NULL) 
{
	return new ValueReal<T>(&val, changeHandler);
}

inline ValueReal<float> * 	createValue(float & val, ChangeHandler * changeHandler, const dmLimitsReal * limitsReal = NULL)
{
	return createValueReal(val, changeHandler, limitsReal);
}

inline ValueReal<double> * 	createValue(double & val, ChangeHandler * changeHandler, const dmLimitsReal * limitsReal = NULL)
{
	return createValueReal(val, changeHandler, limitsReal);
}

struct RanIter {};
static RanIter ranIter;

class ValueArray : public Value
{
public:
	template<size_t size>
	ValueArray(Value * (&elements)[size], ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		elements_.resize(size);
		std::copy(elements, elements + size, elements_.begin());
	}
	template<typename RanIt>
	ValueArray(RanIt begin, RanIt end, RanIter, ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		elements_.resize(end - begin);
		std::copy(begin, end, elements_.begin());
	}
	template<typename BidIt>
	ValueArray(BidIt begin, BidIt end, ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		for(BidIt it = begin;
			it != end;
			++it)
			elements_.push_back(*it);
	}	
	~ValueArray();
	virtual Type				getType		() const;
	virtual dmIndex				getSize		() const;
	virtual Value *				index		(dmIndex index);
	virtual const Value *		index		(dmIndex index) const;
	virtual Value *				indexArray	(dmIndex index);
	virtual const Value*		indexArray	(dmIndex index) const;		
private:
	typedef std::vector<Value*> Elements;
	Elements elements_;
};

template<typename BidIt>
inline ValueArray * createValueArrayBidIt(BidIt begin, BidIt end, ChangeHandler * changeHandlerIn)
{
	std::list<Value*> values;
	for(BidIt it = begin;
		it != end;
		++it)
	{
		values.push_back(createValue(*it));
	}
	return new ValueArray(values.begin(), values.end(), changeHandlerIn);
}

template<typename RanIt>
inline ValueArray * createValueArrayRanIt(RanIt begin, RanIt end, ChangeHandler * changeHandlerIn)
{
	std::vector<Value*> values;
	values.resize(end - begin);
	for(RanIt it = begin;
		it != end;
		++it)
	{
		values[it - begin] = createValue(*it, changeHandlerIn);
	}
	return new ValueArray(values.begin(), values.end(), ranIter, changeHandlerIn);
}

template<typename T, size_t size>
ValueArray * createValue(T (&array)[size], ChangeHandler * changeHandlerIn)
{
	return createValueArrayRanIt(array, array + size, changeHandlerIn);
}

template<typename T>
ValueArray * createValue(T * array, size_t size, ChangeHandler * changeHandlerIn)
{
	return createValueArrayRanIt(array, array + size, changeHandlerIn);
}

template<typename T>
ValueArray * createValue(std::list<T> & vec, ChangeHandler * changeHandlerIn)
{
	return createValueArrayBidIt(vec.begin(), vec.end(), changeHandlerIn);
}

template<typename T>
ValueArray * createValue(std::vector<T> & vec, ChangeHandler * changeHandlerIn)
{
	return createValueArrayRanIt(vec.begin(), vec.end(), changeHandlerIn);
}

class ValueTable : public Value
{
public:
	ValueTable(const ItemTable & itemTable, ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		elements_.resize(1);
		elements_[0] = itemTable;
	}
	template<size_t size>
	ValueTable(const ItemTable (&elements)[size], ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		elements_.resize(size);
		std::copy(elements, elements + size, elements_.begin());
	}
	template<typename RanIt>
	ValueTable(RanIt begin, RanIt end, RanIter, ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		elements_.resize(size);
		std::copy(begin, end, elements_.begin());
	}
	template<typename BidIt>
	ValueTable(BidIt begin, BidIt end, ChangeHandler * changeHandlerIn) : Value(changeHandlerIn)
	{
		for(BidIt it = begin;
			it != end;
			++it)
			elements_.push_back(*it);
	}
	~ValueTable();
	virtual Type				getType		() const;
	virtual dmIndex				getSize		() const;
	virtual Value *				index		(dmIndex index);
	virtual const Value *		index		(dmIndex index) const;
	virtual ItemTable			indexTable	(dmIndex index);
	virtual ItemTableC			indexTable	(dmIndex index) const;
private:
	typedef std::vector<ItemTable> Elements;
	Elements elements_;
};

}

extern "C"
{
void __declspec(dllexport) initialize(DataModelAPI * dataModelAPI);
}

#endif _Value_hpp_