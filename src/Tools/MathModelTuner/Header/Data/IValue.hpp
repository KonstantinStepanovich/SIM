#ifndef _IValue_hpp_
#define _IValue_hpp_

#include "boost/smart_ptr/shared_ptr.hpp"

#include <deque>

class IValue;

typedef boost::shared_ptr<IValue> ValuePtr;
typedef boost::shared_ptr<const IValue> ValuePtrC;

struct ItemTable
{
	ItemTable(const char * fieldIn, IValue * valueIn) : field(fieldIn), value(valueIn) {;}
	const char * field;
	ValuePtr value;
};

struct ItemTableC
{
	ItemTableC(const char * fieldIn, const IValue * valueIn) : field(fieldIn), value(valueIn) {;}
	const char * field;
	ValuePtrC value;
};

typedef int vInteger;
typedef double vReal;
typedef unsigned int vIndex;
typedef vIndex vSize;

enum ValueType {	VALUE_TYPE_NULL, VALUE_TYPE_INTEGER, VALUE_TYPE_REAL,
					VALUE_TYPE_ARRAY, VALUE_TYPE_TABLE, VALUE_TYPE_MAX };

struct dmLimitsInt;
struct dmLimitsReal;

typedef std::deque<vIndex> Path;

class IValue
{
public:
	typedef ValueType Type;
	typedef vInteger Integer;
	typedef vReal Real;
	typedef vIndex Index;
	typedef Index Size;
	virtual ~IValue() {;}
	virtual Type				getType		() const = 0;
	virtual void				setType		(Type type) = 0;
	virtual Integer 			getInteger	() const = 0;
	virtual const dmLimitsInt *	getLimitsInt() const = 0;
	virtual void				setInteger	(Integer val) = 0;
	virtual Real				getReal		() const = 0;
	virtual const dmLimitsReal *getLimitsReal() const = 0;
	virtual void				setReal		(Real val) = 0;	
	virtual Size				getSize		() const = 0;
	virtual ValuePtr 			index		(const Path & path) = 0;
	virtual ValuePtrC 			index		(const Path & path) const = 0;
	virtual ValuePtr 			index		(Index index) = 0;
	virtual ValuePtrC 			index		(Index index) const = 0;
	virtual ValuePtr 			indexArray	(Index index) = 0;
	virtual ValuePtrC 			indexArray	(Index index) const = 0;
	virtual ValuePtr			addItem		(Index index) = 0;
	virtual void				removeItem	(Index index) = 0;
	virtual ItemTable			indexTable	(Index index) = 0;
	virtual ItemTableC			indexTable	(Index index) const = 0;
	virtual ItemTable			addField	(Index index, const char * field) = 0;
	virtual void				removeField	(Index index) = 0;
	virtual void 				notifyChange() = 0;	
};

#endif _IValue_hpp_