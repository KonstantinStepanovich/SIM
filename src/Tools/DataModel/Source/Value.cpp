#include "Value.hpp"

using namespace DataModel;

ChangeHandler::~ChangeHandler()
{
}

Value::Value(ChangeHandler * changeHandlerIn) : changeHandler(changeHandlerIn)
{
}

Value::~Value()
{
}

Value::Type Value::getType() const
{
	return VALUE_TYPE_NULL;
}

dmInteger Value::getInteger() const
{
	THROW_EXCEPTION(Exception, "The value is not an integer");
}

const dmLimitsInt * Value::getLimitsInt() const
{
	THROW_EXCEPTION(Exception, "The value is not an integer");
}

void Value::setInteger(dmInteger val)
{
	THROW_EXCEPTION(Exception, "The value is not an integer");
}

dmReal Value::getReal() const
{
	THROW_EXCEPTION(Exception, "The value is not a real");
}

const dmLimitsReal * Value::getLimitsReal() const
{
	THROW_EXCEPTION(Exception, "The value is not a real");
}

void Value::setReal(dmReal val)
{
	THROW_EXCEPTION(Exception, "The value is not a real");
}

dmIndex Value::getSize() const
{
	THROW_EXCEPTION(Exception, "The value is not an array or a table");
}

Value * Value::index(dmIndex index)
{
	THROW_EXCEPTION(Exception, "The value is not an array or table");
}

const Value * Value::index(dmIndex index) const
{
	THROW_EXCEPTION(Exception, "The value is not an array or table");
}

Value * Value::indexArray(dmIndex index)
{
	THROW_EXCEPTION(Exception, "The value is not an array");
}

const Value * Value::indexArray(dmIndex index) const
{
	THROW_EXCEPTION(Exception, "The value is not an array");
}

Value * Value::addItem(dmIndex index)
{
	THROW_EXCEPTION(Exception, "The value is not an array");
}

void Value::removeItem(dmIndex index)
{
	THROW_EXCEPTION(Exception, "The value is not an array");
}

ItemTable Value::indexTable(dmIndex index)
{
	THROW_EXCEPTION(Exception, "The value is not a table");
}

ItemTableC Value::indexTable(dmIndex index) const
{
	THROW_EXCEPTION(Exception, "The value is not a table");
}

void Value::notifyChange()
{
	(*changeHandler)(this);
}

//API functions

unsigned int Value::getType(const dmValueHandle handle)
{
	return ((const Value*)(handle))->getType();
}

dmInteger Value::getInteger(const dmValueHandle handle)
{
	return ((const Value*)(handle))->getInteger();
}

const dmLimitsInt * Value::getLimitsInt(const dmValueHandle handle)
{
	return ((const Value*)(handle))->getLimitsInt();
}

void Value::setInteger(dmValueHandle handle, dmInteger val)
{
	return ((Value*)(handle))->setInteger(val);
}

dmReal Value::getReal(const dmValueHandle handle)
{
	return ((const Value*)(handle))->getReal();
}

const dmLimitsReal * Value::getLimitsReal(const dmValueHandle handle)
{
	return ((const Value*)(handle))->getLimitsReal();
}

void Value::setReal(dmValueHandle handle, dmReal val)
{
	return ((Value*)(handle))->setReal(val);
}

dmIndex Value::getSize(const dmValueHandle handle)
{
	return ((Value*)(handle))->getSize();
}

dmValueHandle Value::index(dmValueHandle handle, dmIndex index)
{
	return ((Value*)(handle))->index(index);
}

const dmValueHandle	Value::indexC(const dmValueHandle handle, dmIndex index)
{
	return ((const Value*)(handle))->index(index);
}

dmValueHandle Value::indexArray(dmValueHandle handle, dmIndex index)
{
	return ((Value*)(handle))->indexArray(index);
}

const dmValueHandle	Value::indexArrayC(const dmValueHandle handle, dmIndex index)
{
	return ((const Value*)(handle))->indexArray(index);
}

dmValueHandle Value::addItem(dmValueHandle handle, dmIndex index)
{
	return ((Value*)(handle))->addItem(index);
}

void Value::removeItem(dmValueHandle handle, dmIndex index)
{
	return ((Value*)(handle))->removeItem(index);
}

dmItemTable Value::indexTable(dmValueHandle handle, dmIndex index)
{
	ItemTable itemTable = ((Value*)(handle))->indexTable(index);
	dmItemTable res = { itemTable.field, itemTable.value };
	return res;
}

dmItemTableC Value::indexTableC(const dmValueHandle handle, dmIndex index)
{
	const ItemTableC itemTable = ((const Value*)(handle))->indexTable(index);
	dmItemTableC res = { itemTable.field, itemTable.value };
	return res;
}

void Value::notifyChange(dmValueHandle handle)
{
	((Value*)(handle))->notifyChange();
}

//-=ValueArray=-

ValueArray::~ValueArray()
{
	for(Elements::iterator itE = elements_.begin();
		itE != elements_.end();
		++itE)
		delete *itE;
}

ValueArray::Type ValueArray::getType() const
{
	return VALUE_TYPE_ARRAY;
}

dmIndex ValueArray::getSize() const
{
	return elements_.size();
}

Value * ValueArray::index(dmIndex index)
{
	return elements_[index];
}

const Value* ValueArray::index(dmIndex index) const
{
	return elements_[index];
}

Value * ValueArray::indexArray(dmIndex index)
{
	return elements_[index];
}

const Value* ValueArray::indexArray(dmIndex index) const
{
	return elements_[index];
}

//-=ValueTable=-

ValueTable::~ValueTable()
{
	for(Elements::iterator itE = elements_.begin();
		itE != elements_.end();
		++itE)
		delete itE->value;
}

ValueTable::Type	ValueTable::getType		() const
{
	return VALUE_TYPE_TABLE;
}

dmIndex				ValueTable::getSize		() const
{
	return elements_.size();
}

Value * ValueTable::index(dmIndex index)
{
	return elements_[index].value;
}

const Value* ValueTable::index(dmIndex index) const
{
	return elements_[index].value;
}

ItemTable			ValueTable::indexTable	(dmIndex index)
{
	return elements_[index];
}

ItemTableC			ValueTable::indexTable	(dmIndex index) const
{
	return elements_[index];
}

extern "C"
{
void __declspec(dllexport) initialize(DataModelAPI * dataModelAPI)
{
	dataModelAPI->value.getType		= &Value::getType;
	dataModelAPI->value.getInteger	= &Value::getInteger;
	dataModelAPI->value.getLimitsInt= &Value::getLimitsInt;
	dataModelAPI->value.setInteger	= &Value::setInteger;
	dataModelAPI->value.getReal		= &Value::getReal;
	dataModelAPI->value.getLimitsReal= &Value::getLimitsReal;
	dataModelAPI->value.setReal		= &Value::setReal;
	dataModelAPI->value.getSize		= &Value::getSize;
	dataModelAPI->value.index		= &Value::index;
	dataModelAPI->value.indexArray	= &Value::indexArray;
	dataModelAPI->value.indexArrayC = &Value::indexArrayC;
	dataModelAPI->value.addItem		= &Value::addItem;
	dataModelAPI->value.removeItem	= &Value::removeItem;
	dataModelAPI->value.indexTable	= &Value::indexTable;
	dataModelAPI->value.indexTableC	= &Value::indexTableC;
	dataModelAPI->value.notifyChange= &Value::notifyChange;
}
}