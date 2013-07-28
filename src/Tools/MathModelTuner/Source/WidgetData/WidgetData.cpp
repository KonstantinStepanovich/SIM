#include "WidgetData/WidgetData.hpp"

void load(Path & path, const Variant & data);

WidgetData::WidgetData(bool editableIn, Callback * callbackIn) : editable(editableIn), callback(callbackIn)
{
}

WidgetData::~WidgetData()
{
}

WidgetDataCreate::WidgetDataCreate(bool editableIn) : editable(editableIn)
{
}

WidgetDataCreate::~WidgetDataCreate()
{
}

void WidgetDataCreate::load(const Variant & data)
{
	editable = data["editable"].asBool();
	::load(conditionPath, data["condition"]);
}

WidgetDataComp::WidgetDataComp(bool editableIn, bool refEditableIn, Callback * callbackIn) : WidgetData(editableIn, callbackIn), refEditable(refEditableIn)
{
}

WidgetDataCompCreate::WidgetDataCompCreate(bool editableIn, bool refEditableIn) : editable(editableIn), refEditable(refEditableIn)
{
}

WidgetDataCompCreate::~WidgetDataCompCreate()
{
}

void WidgetDataCompCreate::load(const Variant & data)
{
	editable = data["editable"].asBool();
	refEditable = data["refEditable"].asBool();
	::load(conditionPath, data["condition"]);
}