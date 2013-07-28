#include "WindowMain.hpp"
#include "WindowData.hpp"
#include "WindowTest.hpp"
#include "WidgetData/WidgetDataTree.hpp"

#include <fstream>

#include <QtGui>
#include <QMenuBar.h>
#include <QDialog>

#include <json/reader.h>
#include <json/writer.h>

#include "String/StringStream.hpp"
#include "Utils/Units.hpp"

WindowMain::WindowMain() :	module_(NULL), modelHandle_(NULL),
							windowModelParameters_(NULL)
{
    mdiArea_ = new QMdiArea();
    mdiArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea_);

	testMapper_ = new QSignalMapper(this);
	connect(testMapper_, SIGNAL(mapped(int)),
			this, SLOT(toggleTest(int)));

    createActions_();

    setWindowTitle(tr("Math Model Tuner"));
    setUnifiedTitleAndToolBarOnMac(true);
}

WindowMain::~WindowMain()
{
	if(module_ != NULL)
		unloadModule_();
}

bool WindowMain::eventFilter(QObject * sender, QEvent * event)
{
	if(event->type() == QEvent::Close)
	{
		if(	windowModelParameters_ != NULL &&
			sender == windowModelParameters_->widget())
		{
			windowModelParameters_ = NULL;
			actionModelParameters_->setChecked(false);
		}
		for(Tests::iterator itWT = tests_.begin();
			itWT != tests_.end();
			++itWT)
		{
			QMdiSubWindow * & windowTestSubWindow = itWT->window;
			if(windowTestSubWindow != NULL)
			{
				if(windowTestSubWindow->widget() == sender)
				{
					WindowTest * windowTest = static_cast<WindowTest*>(windowTestSubWindow->widget());
					modelTestAPI_.test.setResultChangeCallback(windowTest->getHandle(), NULL, NULL);
					windowTestSubWindow = NULL;
					size_t index = itWT - tests_.begin();
					QAction * action = (QAction*)testMapper_->mapping(index);
					action->setChecked(false);
					break;
				}
			}
		}
		return true;
	}
	return false;
}

void WindowMain::closeEvent(QCloseEvent *event)
{
    mdiArea_->closeAllSubWindows();
    if (mdiArea_->currentSubWindow())
	{
        event->ignore();
    }
	else
	{
        event->accept();
    }
}

void WindowMain::loadModule()
{
	QFileDialog::Options options;
	//if (!native->isChecked())
	//	options |= QFileDialog::DontUseNativeDialog;
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
													tr("Load module"),
													"../../../bin/MSVC90/",
													tr("Math model dll (MathModel*.dll);;All Files (*.*)"),
													&selectedFilter,
													options);
	if(!fileName.isEmpty())
	{
		module_ = Sim::loadModule(fileName.toLatin1());
		if(module_ != NULL)
		{
			ModelTestAPIInitialize modelTestAPIInitialize = Sim::getFunction<ModelTestAPIInitialize>(module_, "modelTestAPIInitialize");
			if(modelTestAPIInitialize != NULL)
			{
				actionLoadModel_->setEnabled(true);
				actionUnloadModule_->setEnabled(true);
				(*modelTestAPIInitialize)(&modelTestAPI_);
				loadModel_();
			}
			else
			{
				QMessageBox::information(	this,
											tr(OSS("Math model not found in " << (const char*)fileName.toLatin1())),
											tr(OSS("Math model not found in " << (const char*)fileName.toLatin1())));
				unloadModule_();
			}
		}
		else
		{
			actionLoadModel_->setEnabled(false);
			actionUnloadModule_->setEnabled(false);
			QMessageBox::information(	this,
										tr(OSS("Can't load module " << (const char*)fileName.toLatin1())),
										tr(OSS("Can't load module " << (const char*)fileName.toLatin1())));
		}
	}
}

void WindowMain::unloadModule()
{
	unloadModule_();
	delete windowModelParameters_;
	windowModelParameters_ = NULL;
}

void WindowMain::updateWindows_()
{
	if(windowModelParameters_ != NULL)
	{
		WindowData * windowData = static_cast<WindowData*>(windowModelParameters_->widget());
		windowData->rebuild();
	}

	for(Tests::iterator itWT = tests_.begin();
		itWT != tests_.end();
		++itWT)
	{
		if(itWT->window != NULL)
		{
			WindowTest * windowTest = static_cast<WindowTest*>(itWT->window->widget());
			windowTest->rebuild();
		}
	}
}

void WindowMain::toggleTest(int index)
{
	Test & test = tests_[index];
	QMdiSubWindow * & windowTestSub = test.window;
	if(windowTestSub == NULL)
	{
		TestBaseHandle testBaseHandle = modelTestAPI_.model.getTestBase(modelHandle_);
		TestHandle testHandle = modelTestAPI_.testBase.getTest(testBaseHandle, index);
		modelTestAPI_.test.enable(testHandle);
		const char * testName = modelTestAPI_.test.getName(testHandle);

		const TestRepresentation & testRepresentation = modelRepresentation_.test(testName);
		WindowTest * windowTest = new WindowTest(	this,
													modelTestAPI_.dataModelAPI,
													modelTestAPI_.test,
													testHandle,
													testRepresentation.parameters,
													testRepresentation.result,
													test.data);
		modelTestAPI_.test.setResultChangeCallback(testHandle, WindowTest::onResultChange, windowTest);
		windowTestSub = mdiArea_->addSubWindow(windowTest);
		windowTest->setWindowTitle((QString(testName) + " - Test").toLatin1());
		windowTest->show();
	}
	else
	{
		delete windowTestSub;
		windowTestSub = NULL;
	}
}

void WindowMain::toggleWindowModelParameters()
{
	if(windowModelParameters_ == NULL)
		createWindowModelParameters_();
	else
	{
		delete windowModelParameters_;
		windowModelParameters_ = NULL;
	}
}

void WindowMain::setMetricUnitSystem()
{
	Sim::UnitManager::instance().setUnitSystem(Sim::UNIT_SYSTEM_METRIC);
	unitsImperialAction_->setChecked(false);
	updateWindows_();
}

void WindowMain::setImperialUnitSystem()
{
	Sim::UnitManager::instance().setUnitSystem(Sim::UNIT_SYSTEM_IMPERIAL);
	unitsMetricAction_->setChecked(false);
	updateWindows_();
}

void WindowMain::loadModel()
{
	loadModel_();
}

void WindowMain::unloadModel()
{
	unloadModel_();
}

void WindowMain::saveModel()
{
	modelTestAPI_.model.save(modelHandle_);
}

void WindowMain::unloadModule_()
{
	unloadModel_();
	actionUnloadModule_->setEnabled(false);
	actionLoadModel_->setEnabled(false);
	
	Sim::freeModule(module_);
	module_ = NULL;
}

void WindowMain::loadModel_()
{
	QStringList items;
	
	size_t modelCount = modelTestAPI_.modelBase.getSize();
	for(size_t i = 0; i < modelCount; ++i)
	{
		ModelHandle modelHandle = modelTestAPI_.modelBase.getModel(i);
		const char * name = modelTestAPI_.model.getName(modelHandle);
		items << tr(name);
	}

	bool ok;
	QString item = QInputDialog::getItem(	this, tr("Choose model"),
											tr("Models:"), items, 0, false, &ok);
	if(ok)
	{
		size_t modelIndex = 0;
		for(QStringList::iterator itI = items.begin(); itI != items.end(); ++itI, ++modelIndex)
			if(*itI == item)
				break;

		modelHandle_ = modelTestAPI_.modelBase.getModel(modelIndex);
		const char * modelName = modelTestAPI_.model.getName(modelHandle_);
		setWindowTitle(tr((QString(modelName) + " - Math Model Tuner").toLatin1()));		
		if(!modelTestAPI_.model.load(modelHandle_))
		{
			QMessageBox::information(	this,
										tr(OSS("Can't load model " << modelName)),
										tr(OSS("Can't load model " << modelName)));
			return;
		}

		const char * modelType = modelTestAPI_.model.getType(modelHandle_);
		{
			std::string path = "Tools\\MathModelTuner\\Representations\\";
			path += modelType;
			path += ".js";

			Variant representation;
			Json::Reader reader;
			std::ifstream ifs;
			ifs.open(path.c_str());
				reader.parse(ifs, representation, true);
			ifs.close();
			modelRepresentation_.load(representation);			
		}

		createWindowModelParameters_();

		actionUnloadModel_->setEnabled(true);
		actionSaveModel_->setEnabled(true);

		menuWindows_->setEnabled(true);

		TestBaseHandle testBaseHandle = modelTestAPI_.model.getTestBase(modelHandle_);
		{
			menuTests_->clear();
			size_t testCount = modelTestAPI_.testBase.getSize(testBaseHandle);
			menuTests_->setEnabled(testCount > 0);
			for(size_t i = 0; i < testCount; ++i)
			{
				TestHandle testHandle = modelTestAPI_.testBase.getTest(testBaseHandle, i);
				const char * name = modelTestAPI_.test.getName(testHandle);
				QAction * action = menuTests_->addAction(name);
				action->setCheckable(true);
				action ->setChecked(false);
				connect(action, SIGNAL(triggered()), testMapper_, SLOT(map()));
				testMapper_->setMapping(action, i);
			}
		}

		{
			for(Tests::iterator itWT = tests_.begin();
				itWT != tests_.end();
				++itWT)
			{
				QMdiSubWindow * & windowTest = itWT->window;
				if(windowTest != NULL)
				{
					delete windowTest;
					windowTest = NULL;
				}
			}
			size_t testCount = modelTestAPI_.testBase.getSize(testBaseHandle);
			tests_.resize(testCount);
			referenceRoot_.clear();
			for(Tests::iterator itWT = tests_.begin();
				itWT != tests_.end();
				++itWT)
			{
				const size_t index = itWT - tests_.begin();
				TestHandle testHandle = modelTestAPI_.testBase.getTest(testBaseHandle, index);
				TestData & testData = itWT->data;
				testData.testName = modelTestAPI_.test.getName(testHandle);
				testData.testParametersRoot = &testPrarameterRoot_;
				testData.referenceRoot = &referenceRoot_;
				testData.referenceIndex = 0;
				testData.showReference = false;
				testData.slaveToReference = false;
			}
		}

		{
			std::string path = "Tools\\MathModelTuner\\Reference\\";
			path += modelName;
			path += ".js";

			Json::Reader reader;
			std::ifstream ifs;
			ifs.open(path.c_str());
				reader.parse(ifs, referenceRoot_, true);
			ifs.close();
		}
		{
			std::string path = "Tools\\MathModelTuner\\Tests\\";
			path += modelName;
			path += ".js";

			Json::Reader reader;
			std::ifstream ifs;
			ifs.open(path.c_str());
				reader.parse(ifs, testPrarameterRoot_, true);
			ifs.close();
		}
	}
}

void WindowMain::unloadModel_()
{
	menuWindows_->setEnabled(false);
	menuTests_->clear();
	menuTests_->setEnabled(false);
	actionUnloadModel_->setEnabled(false);
	actionSaveModel_->setEnabled(false);
	setWindowTitle(tr("Math Model Tuner"));

	delete windowModelParameters_;
	windowModelParameters_ = NULL;
	for(Tests::iterator itWT = tests_.begin();
		itWT != tests_.end();
		++itWT)
	{
		QMdiSubWindow * & windowTest = itWT->window;
		if(windowTest != NULL)
		{
			delete windowTest;
			windowTest = NULL;
		}
	}

	{
		const char * modelName = modelTestAPI_.model.getName(modelHandle_);
		std::string path = "Tools\\MathModelTuner\\Tests\\";
		path += modelName;
		path += ".js";

		Json::StyledStreamWriter writer;
		std::ofstream ofs;
		ofs.open(path.c_str());
		if(ofs.is_open())
			writer.write(ofs, testPrarameterRoot_);
		ofs.close();
	}
	{
		const char * modelName = modelTestAPI_.model.getName(modelHandle_);
		std::string path = "Tools\\MathModelTuner\\Reference\\";
		path += modelName;
		path += ".js";

		Json::StyledStreamWriter writer;
		std::ofstream ofs;
		ofs.open(path.c_str());
		if(ofs.is_open())
			writer.write(ofs, referenceRoot_);
		ofs.close();
	}
}

void WindowMain::createActions_()
{
	QMenu * menuMain = menuBar()->addMenu(tr("&Main"));
	{
		QAction * actionLoadModule = new QAction(tr("&Load Module..."), this);
		menuMain->addAction(actionLoadModule);
		connect(actionLoadModule, SIGNAL(triggered()), this, SLOT(loadModule()));

		actionUnloadModule_ = new QAction(tr("&Unload Module"), this);
		actionUnloadModule_->setEnabled(false);
		menuMain->addAction(actionUnloadModule_);
		connect(actionUnloadModule_, SIGNAL(triggered()), this, SLOT(unloadModule()));

		actionLoadModel_ = new QAction(tr("&Load Model..."), this);
		actionLoadModel_->setEnabled(false);
		menuMain->addAction(actionLoadModel_);
		connect(actionLoadModel_, SIGNAL(triggered()), this, SLOT(loadModel()));

		actionUnloadModel_ = new QAction(tr("&Unload Model"), this);
		actionUnloadModel_->setEnabled(false);
		menuMain->addAction(actionUnloadModel_);
		connect(actionUnloadModel_, SIGNAL(triggered()), this, SLOT(unloadModel()));

		actionSaveModel_ = new QAction(tr("&Save Model"), this);
		actionSaveModel_->setEnabled(false);
		menuMain->addAction(actionSaveModel_);
		connect(actionSaveModel_, SIGNAL(triggered()), this, SLOT(saveModel()));
	}
	menuWindows_ = menuBar()->addMenu(tr("&Windows"));
	menuWindows_->setEnabled(false);
	{
		actionModelParameters_ = new QAction(tr("&Model Parameters"), this);
		actionModelParameters_->setCheckable(true);
		actionModelParameters_->setChecked(false);
		menuWindows_->addAction(actionModelParameters_);
		connect(actionModelParameters_, SIGNAL(triggered()), this, SLOT(toggleWindowModelParameters()));
	}
	menuTests_ = menuBar()->addMenu(tr("&Tests"));
	QMenu * menuSettings = menuBar()->addMenu(tr("&Settings"));
	{
		QMenu * unitsMenu = menuSettings->addMenu(tr("&Units"));

		unitsMetricAction_ = new QAction(tr("&Metric"), this);
		unitsMetricAction_->setCheckable(true);
		unitsMetricAction_->setChecked(true);
		unitsMenu->addAction(unitsMetricAction_);
		connect(unitsMetricAction_, SIGNAL(triggered()), this, SLOT(setMetricUnitSystem()));

		unitsImperialAction_ = new QAction(tr("&Imperial"), this);
		unitsImperialAction_->setCheckable(true);
		unitsImperialAction_->setChecked(false);
		unitsMenu->addAction(unitsImperialAction_);
		connect(unitsImperialAction_, SIGNAL(triggered()), this, SLOT(setImperialUnitSystem()));
	}
	menuTests_->setEnabled(false);
}

void WindowMain::createWindowModelParameters_()
{
	const char * modelName = modelTestAPI_.model.getName(modelHandle_);

	dmValueHandle dataHandle = modelTestAPI_.model.getParameters(modelHandle_);
	if(windowModelParameters_ == NULL)
		windowModelParameters_ = mdiArea_->addSubWindow(new WindowData(this, modelRepresentation_.parameters(), modelTestAPI_.dataModelAPI, dataHandle));

	WindowData * windowData = static_cast<WindowData*>(windowModelParameters_->widget());
	windowData->setWindowTitle((QString(modelName) + " - Model parameters").toLatin1());
	windowData->setData(dataHandle);
	windowData->show();

	actionModelParameters_->setChecked(true);
}