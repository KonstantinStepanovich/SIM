#ifndef _WidgetDataPlot_hpp_
#define _WidgetDataPlot_hpp_

#include <QString.h>
#include <QVector.h>
#include <QWidget.h>

#include "WidgetData/WidgetData.hpp"
#include "Data/IValue.hpp"
#include "Utils/Units.hpp"

#include <QPen.h>

enum CurveStyle
{
    CurveStyleNo = -1,
    CurveStyleLines,
    CurveStyleSticks,
    CurveStyleSteps,
    CurveStyleDots,
};

struct BaseCurve
{
	BaseCurve() : style(CurveStyleLines) {;}
	CurveStyle style;
	QPen pen;
	Path array;
	Path arg;
	Path val;
	double Xmin;
};

struct SingleCurve : public BaseCurve
{
	QString caption;
};

struct CurveParameterVariant
{
	QString	caption;
	Sim::UnitId	type;
};

struct CurveParameter
{
	Path path;
	QVector<CurveParameterVariant> variants;
};

struct MultiCurve : public BaseCurve
{
	CurveParameter Z;
	QColor colorMin;
	QColor colorMax;
	Path root;
	Path parameter;
};

struct Curves
{
	typedef QVector<SingleCurve> SingleCurves;
	SingleCurves singleCurves;
	typedef QVector<MultiCurve> MultiCurves;
	MultiCurves multiCurves;
	QString caption;
	CurveParameter X;
	CurveParameter Y;
};

//Data Plot

struct DescPlot
{
	DescPlot();
	Curves curves;
};

class CreatePlot : public WidgetDataCreate
{
public:
	typedef DescPlot Desc;
	virtual void load(const Variant & data);
	virtual WidgetData * operator()(QWidget * widget, WidgetDataCallback * callback) const;
	virtual QString getName() const;
	Desc desc;
};

class WidgetDataPlotImpl;

class WidgetDataPlot : public WidgetData
{
public:
	typedef CreatePlot Create;
	typedef WidgetDataPlotImpl Impl;
	WidgetDataPlot(const DescPlot & desc, QWidget * parent, bool editableIn, Callback * callbackIn = NULL);
	~WidgetDataPlot();
	virtual QWidget * widget();
	virtual void setData(const ValuePtr & value);
	virtual void rebuild();
	virtual void onChange(const Path & path);
private:
	Impl * impl_;
};

class WidgetDataPlotImpl : public QWidget
{
public:
	WidgetDataPlotImpl(const DescPlot & descIn, QWidget * parent, WidgetData * widgetDataIn);
	~WidgetDataPlotImpl();
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
	void setData(const ValuePtr & valueIn);
	void rebuild();
	void onChange(const Path & path);
protected:
	void applyData_();
	virtual bool eventFilter(QObject *object, QEvent *e);
	const DescPlot & desc;	
	ValuePtr value;
	class Plot;
	Plot * plot;
	WidgetData *  widgetData;
};

//Data Plot Comp

struct DescPlotComp : public DescPlot
{
	DescPlotComp();
};

class CreatePlotComp : public WidgetDataCompCreate
{
public:
	typedef DescPlotComp Desc;
	virtual void load(const Variant & data);
	virtual WidgetDataComp * operator()(QWidget * widget, WidgetDataCallback * callback) const;
	virtual QString getName() const;
	Desc desc;
};

class WidgetDataCompPlotImpl;

class WidgetDataCompPlot : public WidgetDataComp
{
public:
	typedef CreatePlotComp Create;
	typedef WidgetDataCompPlotImpl Impl;
	WidgetDataCompPlot(const DescPlotComp & desc, QWidget * parent, bool editableIn, bool refEditableIn, Callback * callbackIn = NULL);
	~WidgetDataCompPlot();
	virtual QWidget * widget();
	virtual void setData(const ValuePtr & value);
	virtual void setDataComp(const ValuePtr & valueResult, const ValuePtr & valueReference);
	virtual void resetReferenceData();
	virtual void rebuild();
	virtual void onChange(const Path & path);
private:
	Impl * impl_;
};

class WidgetDataCompPlotImpl : public WidgetDataPlotImpl
{
public:
	WidgetDataCompPlotImpl( const DescPlotComp & desc, QWidget * parent, WidgetData * widgetDataIn = NULL);
	~WidgetDataCompPlotImpl();
	void setData(const ValuePtr & value);
	void setData(const ValuePtr & valueResult, const ValuePtr & valueReference);
	void resetReferenceData();
	void rebuild();
	void onChange(const Path & path);
private:
	void applyDataComp_();
	ValuePtr valueReference_;
};

#endif _WidgetDataPlot_hpp_