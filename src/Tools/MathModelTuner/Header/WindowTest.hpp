#ifndef _WindowTest_hpp_
#define _WindowTest_hpp_

#include "WidgetData/WidgetData.hpp"

extern "C"
{
#include "ModelTestAPI.h"
}

#include <QVector.h>
#include <QMainWindow.h>

class QLabel;
class QCheckBox;
class QSpinBox;
class QAction;

struct TestData;

class WindowTest : public QMainWindow
{
	Q_OBJECT
public:
	WindowTest(	QWidget * parent,
				const DataModelAPI & dataModelAPI,
				const Test & test,
				TestHandle testHandle,
				const WidgetsDataCompCreate & widgetParamDataCreates,
				const WidgetsDataCompCreate & widgetResultDataCreates,
				TestData & testData);
	~WindowTest();
	void rebuild();
	inline TestHandle getHandle() const { return testHandle_; }
	static void onResultChange(void * data, size_t * path, size_t pathSize);
private slots:
	void toggleReference(int state);
	void slaveToReference(int state);
	void changeReference(int index);
	void addReference();

private:
	WidgetDataComp * createWidgetData_(	const QString & captionPrefix, WidgetDataCompCreate * widgetDataCompCreate,
										dmValueHandle dataHandle, size_t index, WidgetsDataComp & widgetsDataComp);
	QDockWidget * createDockedWidgetData_(	const QString & captionPrefix, WidgetDataCompCreate * widgetDataCompCreate,
											dmValueHandle dataHandle, size_t index, WidgetsDataComp & widgetsDataComp);
	void updateParameters_();
	void updateResult_();
	void update_();

	virtual void closeEvent(QCloseEvent *event);
	const DataModelAPI & dataModelAPI_;
	const Test & test_;
	TestHandle testHandle_;
	
	QWidget * windowMain_;
	const WidgetsDataCompCreate & widgetParamDataCompCreates_;
	WidgetsDataComp widgetsParameters_;
	const WidgetsDataCompCreate & widgetResultDataCompCreates_;
	WidgetsDataComp widgetsResult_;

	TestData & testData_;
	
	QCheckBox * checkBoxToggleReference_;
	QCheckBox * checkBoxSlaveToReference_;
	QSpinBox * spinBoxReferenceIndex_;
	QLabel * labelReferenceCount_;
	QAction * actionNewReference_;
};

#endif _WindowTest_hpp_