#include "WindowTest.hpp"
#include "Data/ValueDataAPI.hpp"
#include "Data/ValueDataPTree.hpp"
#include "Data/ValueUtil.hpp"
#include "TestData.hpp"

#include <algorithm>

#include <QEvent.h>
#include <QString.h>
#include <QDockWidget.h>
#include <QMenuBar.h>
#include <QToolBar.h>
#include <QBoxLayout.h>
#include <QLabel.h>
#include <QCheckBox.h>
#include <QSpinBox.h>

#include "String/StringStream.hpp"

WindowTest::WindowTest(	QWidget * parent,
						const DataModelAPI & dataModelAPI,
						const Test & test,
						TestHandle testHandle,
						const WidgetsDataCompCreate & widgetParamDataCompCreates,
						const WidgetsDataCompCreate & widgetResultDataCompCreates,
						TestData & testData) :
	QMainWindow(parent),
	dataModelAPI_(dataModelAPI), test_(test), testHandle_(testHandle),
	widgetParamDataCompCreates_(widgetParamDataCompCreates), widgetResultDataCompCreates_(widgetResultDataCompCreates),
	windowMain_(parent), testData_(testData)
{
	QMenu * menu = menuBar()->addMenu(tr("&View"));
	if(!widgetParamDataCompCreates.empty())
	{
		QMenu * menuParameters = menu->addMenu(tr("&Parameters"));
		widgetsParameters_.resize(widgetParamDataCompCreates.size());
		dmValueHandle parametersDataHandle = test.getParameters(testHandle_);
		for(WidgetsDataCompCreate::const_iterator itWPDC = widgetParamDataCompCreates.begin();
			itWPDC != widgetParamDataCompCreates.end();
			++itWPDC)
		{
			const size_t index = itWPDC - widgetParamDataCompCreates.begin();
			WidgetDataCompCreate * widgetDataCompCreate = *itWPDC;
			if(widgetDataCompCreate != NULL)
			{				
				dmValueHandle parametersHandle = test_.getParameters(testHandle_);
				if(testData_.testParametersRoot->isMember(testData_.testName))
				{
					Variant & testParameters = (*testData_.testParametersRoot)[testData_.testName];
					ValueDataAPI valueDataAPI(&dataModelAPI_.value, parametersHandle);
					copy(&valueDataAPI, &ValueDataPTree(&testParameters));
					valueDataAPI.notifyChange();
				}
				if(itWPDC == widgetParamDataCompCreates.begin())
				{
					WidgetDataComp * widgetDataComp = createWidgetData_(tr("Parameters - "),
																		widgetDataCompCreate,
																		parametersHandle,
																		index,
																		widgetsParameters_);
					widgetDataComp->setData(ValuePtr(new ValueDataAPI(&dataModelAPI_.value, parametersHandle)));
					setCentralWidget(widgetDataComp->widget());
				}
				else
				{
					QDockWidget * dock = createDockedWidgetData_(	tr("Parameters - "),
																	widgetDataCompCreate,
																	parametersHandle,
																	index,
																	widgetsParameters_);
					menuParameters->addAction(dock->toggleViewAction());
				}
			}
		}
	}

	if(!widgetResultDataCompCreates.empty())
	{
		QMenu * menuResult = menu->addMenu(tr("&Result"));
		widgetsResult_.resize(widgetResultDataCompCreates.size());
		dmValueHandle resultDataHandle = test.getResult(testHandle_);
		for(WidgetsDataCompCreate::const_iterator itWRDC = widgetResultDataCompCreates.begin();
			itWRDC != widgetResultDataCompCreates.end();
			++itWRDC)
		{
			const size_t index = itWRDC - widgetResultDataCompCreates.begin();
			WidgetDataCompCreate * widgetDataCompCreate = *itWRDC;
			if(widgetDataCompCreate != NULL)
			{
				QDockWidget * dock = createDockedWidgetData_(	tr("Result - "),
																widgetDataCompCreate,
																test_.getResult(testHandle_),
																index,
																widgetsResult_);
				menuResult->addAction(dock->toggleViewAction());
			}
		}
	}

	QToolBar * toolBar = addToolBar(tr("Reference"));

	toolBar->addWidget(new QLabel(tr("Reference: ")));
	toolBar->addSeparator();

	size_t referenceCount = (*testData.referenceRoot)[testData.testName].size();

	checkBoxToggleReference_ = new QCheckBox(tr("Show"), this);
	checkBoxToggleReference_->setEnabled(referenceCount > 0);
	checkBoxToggleReference_->setChecked(testData.showReference);
	toolBar->addWidget(checkBoxToggleReference_);
	connect(checkBoxToggleReference_, SIGNAL(stateChanged(int)), SLOT(toggleReference(int)) );
	toolBar->addSeparator();

	checkBoxSlaveToReference_ = new QCheckBox(tr("Slave (N/A)"), this);
	checkBoxSlaveToReference_->setEnabled(referenceCount > 0);
	checkBoxSlaveToReference_->setChecked(testData.slaveToReference);
	toolBar->addWidget(checkBoxSlaveToReference_);
	connect(checkBoxSlaveToReference_, SIGNAL(stateChanged(int)), SLOT(slaveToReference(int)) );
	toolBar->addSeparator();	

	toolBar->addWidget(new QLabel(tr("Number: ")));
	spinBoxReferenceIndex_ = new QSpinBox(this);
	spinBoxReferenceIndex_->setEnabled(referenceCount > 0);
	spinBoxReferenceIndex_->setMinimum(1);
	spinBoxReferenceIndex_->setMaximum(referenceCount);
	spinBoxReferenceIndex_->setValue(testData.referenceIndex + 1);
	toolBar->addWidget(spinBoxReferenceIndex_);
	connect(spinBoxReferenceIndex_, SIGNAL(valueChanged(int)), SLOT(changeReference(int)) );

	toolBar->addWidget(new QLabel(tr(" of ")));
	labelReferenceCount_ = new QLabel(OSS(referenceCount));
	toolBar->addWidget(labelReferenceCount_);
	toolBar->addSeparator();

	actionNewReference_ = new QAction(tr("Create New"), this);
	actionNewReference_->setEnabled(true);
	connect(actionNewReference_, SIGNAL(triggered()), SLOT(addReference()) );
	toolBar->addAction(actionNewReference_);

	if(testData.showReference)
		update_();

	setDockOptions(AnimatedDocks | AllowNestedDocks | AllowTabbedDocks);
}

WindowTest::~WindowTest()
{
	Variant & testParameters = (*testData_.testParametersRoot)[testData_.testName];
	testParameters.clear();
	copy(	&ValueDataPTree(&testParameters),
			&ValueDataAPI(&dataModelAPI_.value, test_.getParameters(testHandle_)));
}

void WindowTest::rebuild()
{
	for(WidgetsDataComp::iterator itWP = widgetsParameters_.begin();
		itWP != widgetsParameters_.end();
		++itWP)
		(*itWP)->rebuild();
	for(WidgetsDataComp::iterator itWR = widgetsResult_.begin();
		itWR != widgetsResult_.end();
		++itWR)
		(*itWR)->rebuild();
}

void WindowTest::onResultChange(void * data, size_t * path, size_t pathSize)
{
	((WindowTest*)(data))->updateResult_();
}

void WindowTest::toggleReference(int state)
{
	testData_.showReference = state != 0;
	checkBoxSlaveToReference_->setEnabled(testData_.showReference);
	update_();
}

void WindowTest::slaveToReference(int state)
{
	testData_.slaveToReference = state != 0;
}

void WindowTest::changeReference(int index)
{
	testData_.referenceIndex = index - 1;
	if(testData_.showReference)
		update_();
}

void WindowTest::addReference()
{
	Variant & testRefereces = (*testData_.referenceRoot)[testData_.testName];
	Variant & reference = testRefereces[testRefereces.size()];

	{
		Variant & parameters = reference["parameters"];
		ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, test_.getParameters(testHandle_)));
		ValuePtr valueDataPTree(new ValueDataPTree(&parameters));
		copy(valueDataPTree, valueDataAPI);
	}

	{
		Variant & result = reference["result"];
		ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, test_.getResult(testHandle_)));
		ValuePtr valueDataPTree(new ValueDataPTree(&result));
		copy(valueDataPTree, valueDataAPI);
	}

	checkBoxToggleReference_->setEnabled(true);
	spinBoxReferenceIndex_->setMinimum(1);
	spinBoxReferenceIndex_->setMaximum(testRefereces.size());
	spinBoxReferenceIndex_->setEnabled(true);
	spinBoxReferenceIndex_->setValue(testRefereces.size());
	labelReferenceCount_->setText(OSS(testRefereces.size()));

	testData_.referenceIndex = testRefereces.size() - 1;
}

void WindowTest::closeEvent(QCloseEvent *event)
{
	QWidget::closeEvent(event);
	windowMain_->eventFilter(this, &QCloseEvent());
}

WidgetDataComp * WindowTest::createWidgetData_(	const QString & captionPrefix,
												WidgetDataCompCreate * widgetDataCompCreate,
												dmValueHandle dataHandle,
												size_t index,
												WidgetsDataComp & widgetsDataComp)
{
	QString caption = captionPrefix + widgetDataCompCreate->getName();
	WidgetDataComp * widgetDataComp = (*widgetDataCompCreate)((QWidget*)this->parent(), NULL);
	widgetDataComp->widget()->setWindowTitle(caption);
	widgetsDataComp[index] = widgetDataComp;
	//widgetDataComp->setData(ValuePtr(new ValueDataAPI (&dataModelAPI_.value, dataHandle)));
	return widgetDataComp;
}

QDockWidget * WindowTest::createDockedWidgetData_(	const QString & captionPrefix,
													WidgetDataCompCreate * widgetDataCompCreate,
													dmValueHandle dataHandle,
													size_t index,
													WidgetsDataComp & widgetsDataComp)
{
	WidgetDataComp * widgetDataComp = createWidgetData_(captionPrefix, widgetDataCompCreate, dataHandle, index, widgetsDataComp);
	//widgetDataComp->setData(ValuePtr(new ValueDataAPI(&dataModelAPI_.value, dataHandle)));

	QDockWidget * dock = new QDockWidget(widgetDataComp->widget()->windowTitle(), this);
	dock->setWidget(widgetDataComp->widget());
	addDockWidget(Qt::RightDockWidgetArea, dock);

	widgetDataComp->setData(ValuePtr(new ValueDataAPI(&dataModelAPI_.value, dataHandle)));

	return dock;
}

void WindowTest::updateParameters_()
{
	if(!testData_.showReference)
	{
		ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, test_.getParameters(testHandle_)));
		for(WidgetsDataComp::Iterator itWP = widgetsParameters_.begin();
			itWP != widgetsParameters_.end();
			++itWP)
		{
			WidgetData * widgetData = *itWP;
			if(widgetData == NULL)
				continue;
			widgetData->setData(valueDataAPI);
			QDockWidget * dock = (QDockWidget*)widgetData->widget()->parent();
			dock->setWindowTitle(widgetData->widget()->windowTitle());
		}
	}
	else
	{
		Variant & testRefereces = (*testData_.referenceRoot)[testData_.testName];
		Variant & reference = testRefereces[testData_.referenceIndex];
		ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, test_.getParameters(testHandle_)));
		ValuePtr valueDataPTree(new ValueDataPTree (&reference["parameters"]));
		for(WidgetsDataComp::Iterator itWP = widgetsParameters_.begin();
			itWP != widgetsParameters_.end();
			++itWP)
		{
			WidgetDataComp * widgetDataComp = *itWP;
			if(widgetDataComp == NULL)
				continue;
			widgetDataComp->setDataComp(valueDataAPI, valueDataPTree);
			QDockWidget * dock = (QDockWidget*)widgetDataComp->widget()->parent();
			dock->setWindowTitle(widgetDataComp->widget()->windowTitle());
		}
	}
}

void WindowTest::updateResult_()
{
	if(!testData_.showReference)
	{
		ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, test_.getResult(testHandle_)));		
		for(WidgetsDataComp::Iterator itWR = widgetsResult_.begin();
			itWR != widgetsResult_.end();
			++itWR)
		{
			WidgetData * widgetData = *itWR;
			if(widgetData == NULL)
				continue;
			widgetData->setData(valueDataAPI);
			QDockWidget * dock = (QDockWidget*)widgetData->widget()->parent();
			dock->setWindowTitle(widgetData->widget()->windowTitle());
		}
	}
	else
	{
		Variant & testRefereces = (*testData_.referenceRoot)[testData_.testName];
		Variant & reference = testRefereces[testData_.referenceIndex];
		ValuePtr valueDataAPI(new ValueDataAPI(&dataModelAPI_.value, test_.getResult(testHandle_)));
		ValuePtr valueDataPTree(new ValueDataPTree(&reference["result"]));
		for(WidgetsDataComp::Iterator itWR = widgetsResult_.begin();
			itWR != widgetsResult_.end();
			++itWR)
		{
			WidgetDataComp * widgetDataComp = *itWR;
			if(widgetDataComp == NULL)
				continue;
			widgetDataComp->setDataComp(valueDataAPI, valueDataPTree);
			QDockWidget * dock = (QDockWidget*)widgetDataComp->widget()->parent();
			dock->setWindowTitle(widgetDataComp->widget()->windowTitle());
		}
	}	
}

void WindowTest::update_()
{
	updateParameters_();
	updateResult_();
}