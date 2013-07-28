#include "Data/ValueUtil.hpp"

ValuePtr nagivate(IValue * src, const Path & path)
{
	ValuePtr result(src);
	for(Path::const_iterator itP = path.begin();
		itP != path.end();
		++itP)
		result = result->index(*itP);
	return result;
}

ValuePtrC nagivate(const IValue * src, const Path & path)
{
	ValuePtrC result(src);
	for(Path::const_iterator itP = path.begin();
		itP != path.end();
		++itP)
		result = result->index(*itP);
	return result;
}

void copy(IValue * dst, const IValue * src)
{
	const ValueType valueType = src->getType();
	if(dst->getType() != valueType)
		dst->setType(valueType);
	switch(valueType)
	{
		case VALUE_TYPE_INTEGER:
			dst->setInteger(src->getInteger());
			break;
		case VALUE_TYPE_REAL:
			dst->setReal(src->getReal());
			break;
		//case NAV_DATA_TYPE_BOOL:
		//	dst->setBool(src->getBool());
		//	break;
		case VALUE_TYPE_ARRAY:
			{
				const vIndex size = src->getSize();
				for(vIndex index = 0; index < size; ++index)
				{
					ValuePtrC srcEl = src->indexArray(index);
					ValuePtr dstEl = (dst->getSize() <= index) ? dst->addItem(index) : dst->indexArray(index);					
					copy(dstEl.get(), srcEl.get());
				}
			}
			break;
		case VALUE_TYPE_TABLE:
			{
				const vIndex size = src->getSize();
				for(vIndex index = 0; index < size; ++index)
				{
					ItemTableC srcEl = src->indexTable(index);
					ItemTable dstEl = (dst->getSize() <= index) ? dst->addField(index, srcEl.field): dst->indexTable(index);
					copy(dstEl.value.get(), srcEl.value.get());
				}
			}
			break;
	}
}