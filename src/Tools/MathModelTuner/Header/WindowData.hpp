#ifndef _WindowData_hpp_
#define _WindowData_hpp_

#include "WidgetData/WidgetData.hpp"

#include "DataModelAPI.h"

#include <QMainWindow.h>

class WindowData : public QMainWindow
{
public:
	WindowData(	QWidget * parent, const WidgetsDataCreate & widgetsDataCreate,
				const DataModelAPI & dataModelAPI, dmValueHandle dataHandle);
	~WindowData();
	void setData(dmValueHandle dataHandle);
	void rebuild();
private:
	virtual void closeEvent(QCloseEvent *event);
	QWidget * windowMain_;
	WidgetsData widgetsData_;
	const DataModelAPI & dataModelAPI_;
	void onDataChange_(WidgetData * widgetDataSource, const Path & path);
	class Callback : public WidgetDataCallback
	{
	public:
		Callback(WindowData * windowData);
		virtual void operator()(WidgetData * widgetData, const Path & path);
	private:
		WindowData * windowData_;
	};
	std::auto_ptr<Callback> callback;
	friend class Callback;
};

#endif _WindowData_hpp_