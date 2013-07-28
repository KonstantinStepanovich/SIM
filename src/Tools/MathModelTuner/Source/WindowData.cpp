#include "WindowData.hpp"
#include "WidgetData/WidgetData.hpp"
#include "WidgetData/WidgetDataTree.hpp"
#include "Data/ValueDataAPI.hpp"

#include <QEvent.h>
#include <QDockWidget.h>
#include <QMenuBar.h>

WindowData::WindowData(QWidget * parent, const WidgetsDataCreate & widgetsDataCreate,
					   const DataModelAPI & dataModelAPI, dmValueHandle dataHandle) :
	QMainWindow(parent), windowMain_(parent), dataModelAPI_(dataModelAPI), callback(new Callback(this))
{
	if(!widgetsDataCreate.empty())
	{
		QMenu * menuView = menuBar()->addMenu(tr("&View"));
		const unsigned int tabCountMax = 4;
		QDockWidget * prevDock = NULL;
		unsigned int tabCount = 0;
		for(WidgetsDataCreate::const_iterator itWDC = widgetsDataCreate.begin();
			itWDC != widgetsDataCreate.end();
			++itWDC)
		{
			WidgetDataCreate * widgetDataCreate = *itWDC;			
			if(widgetDataCreate != NULL)
			{
				ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, dataHandle));
				if(!widgetDataCreate->conditionPath.empty())
					if(valueDataAPI->index(widgetDataCreate->conditionPath)->getInteger() == 0)
						continue;
				QString caption = widgetDataCreate->getName();
				WidgetData * widgetData = (*widgetDataCreate)(this, callback.get());
				widgetsData_.push_back(widgetData);
				if(itWDC == widgetsDataCreate.begin())
				{				
					setCentralWidget(widgetData->widget());
				}
				else
				{
					QDockWidget * dock = new QDockWidget(caption, this);
					dock->setWidget(widgetData->widget());

					if(prevDock != NULL)
					{
						tabifyDockWidget(prevDock, dock);
						++tabCount;
					}
					else
						addDockWidget(Qt::RightDockWidgetArea, dock);	

					if(tabCount == tabCountMax)
					{
						tabCount = 0;
						prevDock = NULL;
					}
					else
						prevDock = dock;
					
					menuView->addAction(dock->toggleViewAction());
				}
			}
		}
	}
	setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks);
}

WindowData::~WindowData()
{
	
}

void WindowData::setData(dmValueHandle dataHandle)
{
	ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, dataHandle));
	for(WidgetsData::const_iterator itWD = widgetsData_.begin();
		itWD != widgetsData_.end();
		++itWD)
	{
		(*itWD)->setData(valueDataAPI);
	}
}

void WindowData::rebuild()
{
	for(WidgetsData::const_iterator itWD = widgetsData_.begin();
		itWD != widgetsData_.end();
		++itWD)
	{
		WidgetData * widgetData = *itWD;
		widgetData->rebuild();
	}
}

void WindowData::closeEvent(QCloseEvent *event)
{
	QWidget::closeEvent(event);
	windowMain_->eventFilter(this, &QCloseEvent());
}

void WindowData::onDataChange_(WidgetData * widgetDataSource, const Path & path)
{
	for(WidgetsData::const_iterator itWD = widgetsData_.begin();
		itWD != widgetsData_.end();
		++itWD)
	{
		WidgetData * widgetData = *itWD;
		if(widgetData != widgetDataSource)
			widgetData->onChange(path);
	}
}

WindowData::Callback::Callback(WindowData * windowData) : windowData_(windowData)
{
}

void WindowData::Callback::operator()(WidgetData * widgetData, const Path & path)
{
	windowData_->onDataChange_(widgetData, path);
}