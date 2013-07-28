#include "ModelRepresentation.hpp"

#include "WidgetData/WidgetDataTree.hpp"
#include "WidgetData/WidgetDataPlot.hpp"

#include "Algorithm/Exception.hpp"

//-=WidgetDataCreateBase=-

typedef WidgetDataCreate* (*ConstructWidgetDataCreate)();

static WidgetDataCreate * constructCreateTree()
{
	return new CreateTree();
}

static WidgetDataCreate * constructCreatePlot()
{
	return new CreatePlot();
}

class WidgetDataCreateBase
{
private:
	typedef std::map<QString, ConstructWidgetDataCreate> Storage;
	Storage storage_;
	WidgetDataCreateBase()
	{
		storage_["Tree"] = constructCreateTree;
		storage_["Plot"] = constructCreatePlot;
	}
public:
	static WidgetDataCreateBase & instance()
	{
		static WidgetDataCreateBase widgetDataCreateBase;
		return widgetDataCreateBase;
	}
	WidgetDataCreate * create(const QString & name) const
	{
		Storage::const_iterator itS = storage_.find(name);
		if(itS != storage_.end())
			return (*itS->second)();
		else
			THROW_EXCEPTION(Sim::Exception, OSS("Unknown widget data \"" << name.toStdString().c_str() << "\""));
	}
};

//WidgetDataCompCreateBase

typedef WidgetDataCompCreate* (*ConstructWidgetDataCompCreate)();

static WidgetDataCompCreate * constructCreateTreeComp()
{
	return new CreateTreeComp();
}

static WidgetDataCompCreate * constructCreatePlotComp()
{
	return new CreatePlotComp();
}

class WidgetDataCreateCompBase
{
private:
	typedef std::map<QString, ConstructWidgetDataCompCreate> Storage;
	Storage storage_;
	WidgetDataCreateCompBase()
	{
		storage_["Tree"] = &constructCreateTreeComp;
		storage_["Plot"] = &constructCreatePlotComp;
	}
public:
	static WidgetDataCreateCompBase & instance()
	{
		static WidgetDataCreateCompBase widgetDataCreateCompBase;
		return widgetDataCreateCompBase;
	}
	WidgetDataCompCreate * create(const QString & name) const
	{
		Storage::const_iterator itS = storage_.find(name);
		if(itS != storage_.end())
			return (*itS->second)();
		else
			THROW_EXCEPTION(Sim::Exception, OSS("Unknown widget data \"" << name.toStdString().c_str() << "\""));
	}
};

//-=ModelRepresentation=-

static void load(WidgetsDataCreate & widgetsDataCreate, const Variant & data)
{
	widgetsDataCreate.resize(data.size());
	for(Variant::const_iterator itV = data.begin();
		itV != data.end();
		++itV)
	{
		const Variant & elem = *itV;
		WidgetDataCreate * widgetDataCreate = WidgetDataCreateBase::instance().create(elem["id"].asCString());
		widgetDataCreate->load(elem["parameters"]);
		widgetsDataCreate[itV.index()] = widgetDataCreate;
	}
}

static void load(WidgetsDataCompCreate & widgetsDataCompCreate, const Variant & data)
{
	widgetsDataCompCreate.resize(data.size());
	for(Variant::const_iterator itV = data.begin();
		itV != data.end();
		++itV)
	{
		const Variant & elem = *itV;
		WidgetDataCompCreate * widgetDataCompCreate = WidgetDataCreateCompBase::instance().create(elem["id"].asCString());
		widgetDataCompCreate->load(elem["parameters"]);
		widgetsDataCompCreate[itV.index()] = widgetDataCompCreate;
	}
}

static void load(TestRepresentation & testRepresentation, const Variant & data)
{
	load(testRepresentation.parameters, data["parameters"]);
	load(testRepresentation.result, data["result"]);
}

ModelRepresentation::ModelRepresentation()
{
}

void ModelRepresentation::load(const Variant & data)
{
	::load(parametersRepresentation_, data["parameters"]);
	const Variant & testData = data["tests"];
	//const Variant::Members members = testData.getMemberNames();
	//for(Variant::Members::const_iterator itM = members.begin();
	//	itM != members.end();
	//	++itM)
	//{
	//	const std::string & testName = *itM;
	//	::load(testRepresentations_[testName], testData[testName]);
	//}
	for(Variant::const_iterator itV = testData.begin();
		itV != testData.end();
		++itV)
	{
		const char * testName = itV.memberName();
		const Variant & test = *itV;		
		::load(testRepresentations_[testName], test);
	}
}

const TestRepresentation & ModelRepresentation::test(const std::string & name) const
{
	TestRepresentations::const_iterator itTR = testRepresentations_.find(name);
	if(itTR != testRepresentations_.end())
		return itTR->second;
	else
		THROW_EXCEPTION(Sim::Exception, OSS("Test \"" << name.c_str() << "\" not found"));
}

ModelRepresentationBase & ModelRepresentationBase::instance()
{
	static ModelRepresentationBase modelRepresentationBase;
	return modelRepresentationBase;
}

void ModelRepresentationBase::load(const Variant & data)
{
	for(Variant::const_iterator itV = data.begin();
		itV != data.end();
		++itV)
	{
		const char * modelName = itV.memberName();
		const Variant & value = *itV;
		modelRepresentations_[modelName].load(value);
	}
}

const ModelRepresentation & ModelRepresentationBase::get(const std::string & name)
{
	ModelRepresentations::const_iterator itMR = modelRepresentations_.find(name);
	if(itMR != modelRepresentations_.end())
		return itMR->second;
	else
		THROW_EXCEPTION(Sim::Exception, OSS("Model type " << name.c_str() << " data not found"));
}

ModelRepresentationBase::ModelRepresentationBase()
{
}