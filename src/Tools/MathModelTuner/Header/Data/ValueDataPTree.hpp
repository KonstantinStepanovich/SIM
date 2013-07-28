#ifndef _ValueDataPTree_hpp_
#define _ValueDataPTree_hpp_

#include "Data/IValue.hpp"

namespace Json
{
class Value;
}

class ValueDataPTree : public IValue
{
public:
	ValueDataPTree(Json::Value * value);
	virtual Type			getType		() const;
	virtual void			setType		(Type type);
	virtual Integer 		getInteger	() const;
	virtual const dmLimitsInt *	getLimitsInt() const;
	virtual void			setInteger	(Integer val);
	virtual Real			getReal		() const;
	virtual const dmLimitsReal *getLimitsReal() const;
	virtual void			setReal		(Real val);	
	virtual Size			getSize		() const;
	virtual ValuePtr 		index		(const Path & path);
	virtual ValuePtrC 		index		(const Path & path) const;
	virtual ValuePtr 		index		(Index index);
	virtual ValuePtrC 		index		(Index index) const;
	virtual ValuePtr		indexArray	(Index index);
	virtual ValuePtrC		indexArray	(Index index) const;
	virtual ValuePtr		addItem		(Index index);
	virtual void			removeItem	(Index index);
	virtual ItemTable		indexTable	(Index index);
	virtual ItemTableC		indexTable	(Index index) const;
	virtual ItemTable		addField	(Index index, const char * field);
	virtual void			removeField	(Index index);
	virtual void 			notifyChange();	
private:
	Json::Value &			index_(Json::Value * value, Index index) const;
	Json::Value &			index_(Index index) const;
	Json::Value &			addIndex_(Index index);
	void					removeIndex_(Index index);
	Json::Value * value_;
};

#endif _ValueDataPTree_hpp_