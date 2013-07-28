#include "Data/ValueDataPTree.hpp"

#include "Json/Value.h"

static Json::ValueType convertValueType(ValueType valueType)
{
	switch(valueType)
	{
		case VALUE_TYPE_NULL:
			return Json::nullValue;
		case VALUE_TYPE_INTEGER:
			return Json::intValue;
		case VALUE_TYPE_REAL:
			return Json::realValue;
		case VALUE_TYPE_ARRAY:
		case VALUE_TYPE_TABLE:
			return Json::arrayValue;
		default:
			return Json::nullValue;
	}
}

static ValueType convertValueType(Json::ValueType valueType, Json::Value & value)
{
	switch(valueType)
	{
		case Json::nullValue:
			return VALUE_TYPE_NULL;
		case Json::intValue:
			return VALUE_TYPE_INTEGER;
		case Json::realValue:
			return VALUE_TYPE_REAL;
		case Json::arrayValue:
			if(!value.empty())
			{
				Json::Value & indexValue = value[0u];
				if(	indexValue.type() == Json::objectValue &&
					indexValue.isMember("arg") &&
					indexValue.isMember("val"))
					return VALUE_TYPE_TABLE;
				else
					return VALUE_TYPE_ARRAY;
			}
			else
				return VALUE_TYPE_ARRAY;
		case Json::objectValue:
			return VALUE_TYPE_TABLE;
		default:
			return VALUE_TYPE_NULL;
	}
}

ValueDataPTree::ValueDataPTree(Json::Value * value) : value_(value)
{
}

ValueDataPTree::Type ValueDataPTree::getType() const
{
	return convertValueType(value_->type(), *value_);
}

void ValueDataPTree::setType(Type type)
{
	Json::ValueType valueType = convertValueType(type);
	if(valueType != Json::arrayValue)
		value_->clear();
	*value_ = Json::Value(valueType);
}

vInteger ValueDataPTree::getInteger() const
{
	return value_->asInt();
}

const dmLimitsInt *	ValueDataPTree::getLimitsInt() const
{
	return NULL;
}

void ValueDataPTree::setInteger(vInteger val)
{
	*value_ = Json::Value(val);
}

vReal ValueDataPTree::getReal() const
{
	return value_->asDouble();
}

const dmLimitsReal * ValueDataPTree::getLimitsReal() const
{
	return NULL;
}

void ValueDataPTree::setReal(vReal val)
{
	*value_ = Json::Value(val);
}

vSize ValueDataPTree::getSize() const
{
	return value_->size();
}

ValuePtr ValueDataPTree::index(const Path & path)
{
	Json::Value * result = value_;
	for(Path::const_iterator itP = path.begin();
		itP != path.end();
		++itP)
		result = &index_(result, *itP);
	return ValuePtr(new ValueDataPTree(result));
}

ValuePtrC ValueDataPTree::index(const Path & path) const
{
	Json::Value * result = value_;
	for(Path::const_iterator itP = path.begin();
		itP != path.end();
		++itP)
		result = &index_(result, *itP);
	return ValuePtrC(new ValueDataPTree(result));
}

ValuePtr ValueDataPTree::index(Index index)
{
	return ValuePtr(new ValueDataPTree(&index_(index)));
}

ValuePtrC ValueDataPTree::index(Index index) const
{
	return ValuePtrC(new ValueDataPTree(&index_(index)));
}

ValuePtr ValueDataPTree::indexArray(Index index)
{
	assert(value_->type() == Json::arrayValue);
	return ValuePtr(new ValueDataPTree(&index_(index)));
}

ValuePtrC ValueDataPTree::indexArray(Index index) const
{
	assert(value_->type() == Json::arrayValue);
	return ValuePtrC(new ValueDataPTree(&index_(index)));	
}

ValuePtr ValueDataPTree::addItem(Index index)
{
	assert(value_->type() == Json::arrayValue);
	return ValuePtr(new ValueDataPTree(&addIndex_(index)));	
}

void ValueDataPTree::removeItem(Index index)
{
	removeIndex_(index);
}

ItemTable ValueDataPTree::indexTable(Index index)
{
	assert(	value_->type() == Json::arrayValue &&
			!value_->empty());
	Json::Value & value = (*value_)[index];
	assert(	value.isMember("arg") &&
			value.isMember("val"));
	return ItemTable(	value["arg"].asCString(),
						new ValueDataPTree(&(value["val"])));
}

ItemTableC ValueDataPTree::indexTable(Index index) const
{
	assert(	value_->type() == Json::arrayValue &&
			!value_->empty());
	Json::Value & value = (*value_)[index];
	assert(	value.isMember("arg") &&
			value.isMember("val"));
	return ItemTableC(	value["arg"].asCString(),
						new ValueDataPTree(&(value["val"])));
}

ItemTable ValueDataPTree::addField(Index index, const char * field)
{
	Json::Value & value = addIndex_(index);
	value["arg"] = field;
	value["val"] = Json::Value();
	return ItemTable(field, new ValueDataPTree(&(value["val"])));
}

void ValueDataPTree::removeField(Index index)
{
	removeIndex_(index);
}

void ValueDataPTree::notifyChange()
{
}

Json::Value & ValueDataPTree::index_(Json::Value * value, Index index) const
{
	assert(value->type() == Json::arrayValue);
	Json::Value & valueItem = (*value)[index];		
	if(	!valueItem.empty() &&
		valueItem.type() == Json::objectValue &&
		valueItem.isMember("arg"))
	{
		const char * field = valueItem["arg"].asCString();
		return valueItem["val"];
	}
	else
		return valueItem;
}

Json::Value & ValueDataPTree::index_(Index index) const
{
	return index_(value_, index);
}

Json::Value & ValueDataPTree::addIndex_(Index index)
{
	value_->append(Json::Value());
	if(index == value_->size() - 1)
		return (*value_)[value_->size() - 1];
	Json::Value::iterator itS = value_->end();
	--itS;
	if(itS == value_->begin())
		return *itS;
	Json::Value::iterator itD = itS;
	--itS;
	Index indexS = value_->size() - 2;
	while(true)
	{
		*itD = *itS;
		if(indexS == index)
			return *itS;
		--itD;
		--itS;
		--indexS;
	}
	return *(value_->begin());
}

void ValueDataPTree::removeIndex_(Index index)
{
	assert(index < value_->size());
	Json::Value::iterator itD = value_->begin();
	for(Index i = 0; i < index; ++i)
		++itD;
	if(itD == value_->end())
	{
		value_->resize(value_->size() - 1);
		return;
	}
	Json::Value::iterator itS = itD;
	++itS;
	while(itS != value_->end())
	{
		*itD = *itS;
		++itD;
		++itS;		
	}
	value_->resize(value_->size() - 1);	
}