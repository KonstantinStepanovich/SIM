#ifndef _WindowMain_hpp_
#define _WindowMain_hpp_

#include "ModelRepresentation.hpp"
#include "TestData.hpp"
#include "Variant.hpp"

struct ModelWidgetData;

extern "C"
{
#include "ModelTestAPI.h"
}

#include <map>

#include <QMainWindow>

class MdiChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QEvent;
QT_END_NAMESPACE

#include "Platform/Module.hpp"

class WindowMain : public QMainWindow
{
    Q_OBJECT
public:
    WindowMain();
	~WindowMain();
	virtual bool eventFilter(QObject * sender, QEvent * event);
protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
	void loadModule();
	void unloadModule();
	void loadModel();
	void unloadModel();
	void saveModel();
	void toggleTest(int index);
	void toggleWindowModelParameters();
	void setMetricUnitSystem();
	void setImperialUnitSystem();

private:	
	void unloadModule_();
	void loadModel_();
	void unloadModel_();
	void updateWindows_();

    void createActions_();
	void createWindowModelParameters_();

    QMdiArea * mdiArea_;

	Sim::Module module_;
	ModelTestAPI modelTestAPI_;
	ModelHandle modelHandle_;
	ModelRepresentation modelRepresentation_;
	Variant testPrarameterRoot_;
	Variant referenceRoot_;

	QAction * actionUnloadModule_;
	QAction * actionLoadModel_;
	QAction * actionUnloadModel_;
	QAction * actionSaveModel_;
	QSignalMapper * testMapper_;
	QMenu * menuWindows_;
		QAction * actionModelParameters_;
	QMenu * menuTests_;
		QAction * unitsMetricAction_;
		QAction * unitsImperialAction_;

	QMdiSubWindow * windowModelParameters_;	
	struct Test
	{
		TestData data;
		QMdiSubWindow * window;
	};
	typedef std::vector<Test> Tests;
	Tests tests_;
};

#endif _WindowMain_hpp_