#ifndef _ValueDataAPI_hpp_
#define _ValueDataAPI_hpp_

#include "Data/IValue.hpp"

#include "DataModelAPI.h"

class ValueDataAPI : public IValue
{
public:
	ValueDataAPI(const dmValue * value, dmValueHandle valueHandle);
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
	const dmValue * value_;
	dmValueHandle valueHandle_;	
};

#endif _ValueDataAPI_hpp_