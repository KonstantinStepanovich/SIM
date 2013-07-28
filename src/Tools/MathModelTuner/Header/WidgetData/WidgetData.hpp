#ifndef _WidgetData_hpp_
#define _WidgetData_hpp_

#include "Data/IValue.hpp"
#include "Variant.hpp"

#include "DataModelAPI.h"

#include <QVector.h>
#include <QString.h>

class QWidget;

class WidgetData;

class WidgetDataCallback
{
public:
	virtual ~WidgetDataCallback() {;}
	virtual void operator()(WidgetData * widgetData, const Path & path) = 0;
};

//WidgetData

class WidgetData
{
public:
	typedef WidgetDataCallback Callback;
	WidgetData(bool editableIn, Callback * callbackIn = NULL);
	~WidgetData();
	virtual QWidget * widget() = 0;
	virtual void setData(const ValuePtr & value) = 0;
	virtual void rebuild() = 0;
	virtual void onChange(const Path & path) = 0;
	inline void notifyChange(const Path & path)
	{
		if(callback != NULL)
			(*callback)(this, path);
	}
	inline bool isEditable() const { return editable; }
protected:
	bool editable;
	Callback * callback;
};

typedef QVector<WidgetData*> WidgetsData;

class WidgetDataCreate
{
public:
	WidgetDataCreate(bool editableIn = false);
	virtual ~WidgetDataCreate();
	virtual void load(const Variant & data);
	virtual WidgetData * operator()(QWidget * widget, WidgetDataCallback * callback) const = 0;
	virtual QString getName() const = 0;
	bool editable;
	Path conditionPath;
};

typedef QVector<WidgetDataCreate*> WidgetsDataCreate;

//WidgetDataComp

class WidgetDataComp : public WidgetData
{
public:
	typedef WidgetDataCallback Callback;
	WidgetDataComp(bool editableIn, bool refEditableIn, Callback * callbackIn = NULL);
	virtual void setDataComp(const ValuePtr & valueResult, const ValuePtr & valueRef) = 0;
	virtual void resetReferenceData() = 0;
	inline bool isRefEditable() const { return refEditable; }
protected:
	bool refEditable;
};

typedef QVector<WidgetDataComp*> WidgetsDataComp;

class WidgetDataCompCreate
{
public:
	WidgetDataCompCreate(bool editableIn = false, bool refEditableIn = false);
	virtual ~WidgetDataCompCreate();
	virtual void load(const Variant & data);
	virtual WidgetDataComp * operator()(QWidget * widget, WidgetDataCallback * callback) const = 0;
	virtual QString getName() const = 0;
	bool editable;
	bool refEditable;
	Path conditionPath;
};

typedef QVector<WidgetDataCompCreate*> WidgetsDataCompCreate;

#endif _WidgetData_hpp_