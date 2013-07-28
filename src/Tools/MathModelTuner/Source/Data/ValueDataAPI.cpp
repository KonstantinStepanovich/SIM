#include "Data/ValueDataAPI.hpp"

ValueDataAPI::ValueDataAPI(const dmValue * value, dmValueHandle valueHandle) : value_(value), valueHandle_(valueHandle)
{
}

ValueDataAPI::Type ValueDataAPI::getType() const
{
	return (ValueDataAPI::Type)value_->getType(valueHandle_);
}

void ValueDataAPI::setType(Type type)
{
	assert(!"Not implemented");
}

vInteger ValueDataAPI::getInteger() const
{
	return value_->getInteger(valueHandle_);
}

const dmLimitsInt *	ValueDataAPI::getLimitsInt() const
{
	return value_->getLimitsInt(valueHandle_);
}

void ValueDataAPI::setInteger(vInteger val)
{
	return value_->setInteger(valueHandle_, val);
}

vReal ValueDataAPI::getReal() const
{
	return value_->getReal(valueHandle_);
}

const dmLimitsReal * ValueDataAPI::getLimitsReal() const
{
	return value_->getLimitsReal(valueHandle_);
}

void ValueDataAPI::setReal(vReal val)
{
	return value_->setReal(valueHandle_, val);
}

vSize ValueDataAPI::getSize() const
{
	return value_->getSize(valueHandle_);
}

ValuePtr ValueDataAPI::index(const Path & path)
{
	dmValueHandle result = valueHandle_;
	for(Path::const_iterator itP = path.begin();
		itP != path.end();
		++itP)
		result =  value_->index(result, *itP);
	return ValuePtr(new ValueDataAPI(value_, result));
}

ValuePtrC ValueDataAPI::index(const Path & path) const
{
	dmValueHandle result = valueHandle_;
	for(Path::const_iterator itP = path.begin();
		itP != path.end();
		++itP)
		result =  value_->index(result, *itP);
	return ValuePtrC(new ValueDataAPI(value_, result));
}

ValuePtr ValueDataAPI::index(Index index)
{
	Type type = getType();
	assert(type == VALUE_TYPE_ARRAY || type == VALUE_TYPE_TABLE);
	if(type == VALUE_TYPE_ARRAY)
		return indexArray(index);
	else if(type == VALUE_TYPE_TABLE)
		return indexTable(index).value;
	else
		return ValuePtr();
}

ValuePtrC ValueDataAPI::index(Index index) const
{
	Type type = getType();
	assert(type == VALUE_TYPE_ARRAY || type == VALUE_TYPE_TABLE);
	if(type == VALUE_TYPE_ARRAY)
		return indexArray(index);
	else if(type == VALUE_TYPE_TABLE)
		return indexTable(index).value;
	else
		return ValuePtr();
}

ValuePtr ValueDataAPI::indexArray(Index index)
{
	return ValuePtr(new ValueDataAPI(value_, value_->indexArray(valueHandle_, index)));
}

ValuePtrC ValueDataAPI::indexArray(Index index) const
{
	return ValuePtrC(new ValueDataAPI(value_, value_->indexArray(valueHandle_, index)));
}

ValuePtr ValueDataAPI::addItem(Index index)
{
	return ValuePtr(new ValueDataAPI(value_, value_->addItem(valueHandle_, index)));
}

void ValueDataAPI::removeItem(Index index)
{
	value_->removeItem(valueHandle_, index);
}

ItemTable ValueDataAPI::indexTable(Index index)
{
	dmItemTable itemTable = value_->indexTable(valueHandle_, index);
	return ItemTable(itemTable.field, new ValueDataAPI(value_, itemTable.handle));
}

ItemTableC ValueDataAPI::indexTable(Index index) const
{
	dmItemTableC itemTable = value_->indexTableC(valueHandle_, index);
	return ItemTableC(itemTable.field, new ValueDataAPI(value_, const_cast<dmValueHandle>(itemTable.handle)));
}

ItemTable ValueDataAPI::addField(Index index, const char * field)
{
	assert(!"Not implemented");
	return ItemTable(field, (ValueDataAPI*)NULL);
}

void ValueDataAPI::removeField(Index index)
{
	assert(!"Not implemented");
}

void ValueDataAPI::notifyChange()
{
	value_->notifyChange(valueHandle_);
}