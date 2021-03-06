#include "WidgetData/WidgetDataPlot.hpp"
#include "WidgetData/SeriesDataEditable.hpp"
#include "WidgetData/CanvasPicker.hpp"
#include "Data/IValue.hpp"

#include <algorithm>
#include <assert.h>
#include <limits>

#include <QString.h>
#include <QEvent.h>
#include <QBoxLayout.h>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_series_data.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_magnifier.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_math.h>
#include <qwt/qwt_symbol.h>

#include "String/StringStream.hpp"

//-=WidgetDataPlotImpl::Plot=-

struct CurveData
{
	QString caption;
	SeriesDataEditable * data;
	QwtPlotCurve::CurveStyle style;
	QPen pen;
};

typedef QVector<CurveData> CurvesData;

struct PlotData
{
	QString caption;
	QString captionX;
	QString captionY;
	CurvesData curves;
};

class EventChangeScaleX : public QEvent
{
public:
	enum { EVENT = QEvent::User + 4};
	EventChangeScaleX(double factor) : QEvent((QEvent::Type)EVENT), factor_(factor) {;}
	inline double getScale() const { return factor_; }
private:
	double factor_;
};

class EventMoveCanvasX : public QEvent
{
public:
	enum { EVENT = QEvent::User + 5};
	EventMoveCanvasX(int dx) : QEvent((QEvent::Type)EVENT), dx_(dx) {;}
	inline double getDx() const { return dx_; }
private:
	int dx_;
};

class WidgetDataPlotImpl::Plot : public QwtPlot
{
	class QwtPlotPannerX : public QwtPlotPanner
	{
	public:
		QwtPlotPannerX(QwtPlotCanvas * canvas, QwtPlot * plot) :	QwtPlotPanner(canvas),
			plot_(plot)
		{
		}
		void forceMoveCanvasX(int dx)
		{
			QwtPlotPanner::moveCanvas(dx, 0);
		}
	private:
		virtual void moveCanvas( int dx, int dy )
		{
			QwtPlotPanner::moveCanvas(dx, dy);
			plot_->event(&EventMoveCanvasX(dx));
		}
		QwtPlot * plot_;
	};
	class QwtPlotMagnifierXBottom : public QwtPlotMagnifier
	{
	public:
		QwtPlotMagnifierXBottom(QwtPlotCanvas * canvas, QwtPlot * plot) :	QwtPlotMagnifier(canvas),
			plot_(plot)
		{
		}
		void forceRescale(double factor)
		{
			QwtPlotMagnifier::rescale(factor);
		}
	private:
		virtual void rescale(double factor)
		{
			QwtPlotMagnifier::rescale(factor);
			plot_->event(&EventChangeScaleX(factor));
		}
		QwtPlot * plot_;
	};
public:
	Plot(QWidget * widget) : QwtPlot(widget)
	{
		// panning with the left mouse button
		QwtPlotPannerX * plotPannerX = new QwtPlotPannerX(canvas(), this);
		plotPannerX_ = plotPannerX;

		// zoom in/out with the wheel
		// X - axis
		QwtPlotMagnifierXBottom * plotMagnifierX = new QwtPlotMagnifierXBottom( canvas(), this );
		plotMagnifierX->setWheelButtonState(Qt::CTRL);
		plotMagnifierX->setAxisEnabled(yLeft, false);
		plotMagnifierX->setAxisEnabled(yRight, false);
		plotMagnifierX->setAxisEnabled(xBottom, true);
		plotMagnifierX->setAxisEnabled(xTop, false);
		plotMagnifierX_ = plotMagnifierX;
		
		// Y - axis
		QwtPlotMagnifier * plotMagnifierY = new QwtPlotMagnifier( canvas() );
		plotMagnifierY->setAxisEnabled(yLeft, true);
		plotMagnifierY->setAxisEnabled(yRight, false);
		plotMagnifierY->setAxisEnabled(xBottom, false);
		plotMagnifierY->setAxisEnabled(xTop, false);
		
		canvasPicker_ = new CanvasPicker(this);

		setAutoFillBackground( true );
		setPalette( QPalette( QColor( 255, 255, 255 ) ) );		

		// canvas
		canvas()->setLineWidth( 1 );
		canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
		canvas()->setBorderRadius( 15 );

		QPalette canvasPalette( Qt::white );
		canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
		canvas()->setPalette( canvasPalette );

		setTitle("");

		// axes 
		setAxisTitle(xBottom, "");
		setAxisTitle(yLeft, "");

		// Insert markers
		QwtPlotMarker * mY = new QwtPlotMarker();
		//mY->setLabel(captionX +" = 0");
		mY->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
		mY->setLineStyle(QwtPlotMarker::HLine);
		mY->setYValue(0.0);
		mY->attach(this);

		QwtPlotMarker * mX = new QwtPlotMarker();
		//mX->setLabel(captionY +" = 0");
		mX->setLabelAlignment(Qt::AlignRight|Qt::AlignTop);
		mX->setLineStyle(QwtPlotMarker::VLine);
		mX->setXValue(0.0);
		mX->attach(this);

		QwtPlotGrid * grid = new QwtPlotGrid();
		grid->attach(this);

		setAutoReplot(true);
		//replot();
	}
	~Plot()
	{
		detachItems();
	}
	void setCaptions(	const QString & caption,
						const QString & captionX,
						const QString & captionY)
	{
		setTitle(caption);
		setAxisTitle(xBottom, captionX);
		setAxisTitle(yLeft, captionY);
	}
	void setCurves(const CurvesData & curvesData)
	{
		if(	curvesData.size() > 1 &&
			legend() == NULL)
			insertLegend(new QwtLegend(), QwtPlot::RightLegend);

		if(curves_.size() > curvesData.size())
		{
			for(Curves::iterator itC = curves_.begin() + curvesData.size();
				itC != curves_.end();
				++itC)
			{
				QwtPlotCurve * plotCurve = *itC;
				plotCurve->detach();
				delete plotCurve;
			}
		}

		if(curves_.size() != curvesData.size())
			curves_.resize(curvesData.size());

		QRectF bound;
		for(CurvesData::const_iterator itCD = curvesData.begin();
			itCD != curvesData.end();
			++itCD)
		{
			const size_t index = itCD - curvesData.begin();
			const CurveData & curveData = *itCD;
			QwtPlotCurve * & plotCurve = curves_[index];
			if(plotCurve == NULL)
			{
				plotCurve = new QwtPlotCurve(curveData.caption);
				plotCurve->setStyle(curveData.style);
				plotCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
				if(curvesData.size() > 1)
					plotCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
				plotCurve->setPen(curveData.pen);
				plotCurve->attach(this);
				//plotCurve->setData(curveData.data);
			}
			else
			{
				plotCurve->setTitle(curveData.caption);
				//plotCurve->itemChanged();
			}
			plotCurve->setData(curveData.data);
			if(curveData.data->isEditable())
			{
				const int symbolSize = 4;
				plotCurve->setSymbol(new QwtSymbol(	QwtSymbol::Ellipse,
													Qt::gray,
													curveData.pen.color(),
													QSize(symbolSize, symbolSize)));
			}
			QRectF curveBound = curveData.data->boundingRect();
			bound = bound | curveBound;
		}
		setAxisScale(xBottom, bound.left(), bound.right());
		setAxisScale(yLeft, bound.top(), bound.bottom());
	}
	void setData(const PlotData & plotData)
	{
		setCurves(plotData.curves);
		setCaptions(plotData.caption, plotData.captionX, plotData.captionY);
	}
	//inline void select(int curve, int point)
	//{
	//	canvasPicker_->select(curve, point);
	//}
	//inline void moveX(int index, double x)
	//{
	//	canvasPicker_->moveX(x);
	//}
	//inline void moveCanvasX(int dx)
	//{
	//	plotPannerX_->forceMoveCanvasX(dx);
	//}
	//inline void rescaleX(double factor)
	//{
	//	plotMagnifierX_->forceRescale(factor);
	//}
	virtual bool event(QEvent *e)
	{
		parent()->eventFilter(this, e);
		return QwtPlot::event(e);
	}
private:
	typedef QVector<QwtPlotCurve*> Curves;
	Curves curves_;

	QwtPlotPannerX * plotPannerX_;
	QwtPlotMagnifierXBottom * plotMagnifierX_;
	CanvasPicker * canvasPicker_;
};

//-=WidgetDataPlotImpl=-

class SeriesValue : public SeriesDataEditable
{
public:
	SeriesValue(ValuePtr valueIn, bool editableIn) : SeriesDataEditable(editableIn), value(valueIn)
	{
	}
	virtual void makePath(Path & path) const = 0;
	virtual void makePath(size_t index, Path & path) const = 0;
protected:
	ValuePtr value;
};

class SeriesDataSinglePlot : public SeriesValue
{
public:
	SeriesDataSinglePlot(	const ValuePtr & valueIn,
							const SingleCurve & singleCurve,
							const QPointF & coeff,
							bool editableIn) :
		SeriesValue(valueIn->index(singleCurve.array), editableIn),
		singleCurve_(singleCurve), coeff_(coeff),
		boundingRectIsValid_(false)
	{
	}
	~SeriesDataSinglePlot()
	{
	}
	virtual size_t size() const
	{
		return value->getSize();
	}
	virtual QPointF sample( size_t i ) const
	{
		QPointF point;
		ValuePtr element = value->indexArray(i);
		point.setX(element->index(singleCurve_.arg)->getReal() * coeff_.x());
		point.setY(element->index(singleCurve_.val)->getReal() * coeff_.y());
		return point;
	}
	virtual void setSample(int index, const QPointF & point)
	{
		const size_t seriesSize = size();
		ValuePtr element = value->indexArray(index);
		element->index(singleCurve_.arg)->setReal(std::max(singleCurve_.Xmin, point.x() / coeff_.x()));
		element->index(singleCurve_.val)->setReal(point.y() / coeff_.y());
		element->notifyChange();
	}
	virtual void addSample(int index, const QPointF & point)
	{
		value->addItem(index);
		setSample(index, point);
	}
	virtual void removeSample(int index)
	{
		value->removeItem(index);
	}
	virtual void makePath(Path & path) const
	{
		path = singleCurve_.array;
	}
	virtual void makePath(size_t index, Path & path) const
	{
		path = singleCurve_.array;
		path.push_back(index);
	}
	virtual QRectF boundingRect() const
	{
		if(!boundingRectIsValid_)
		{
			if(size() > 0)
			{
				boundingRect_ = QRectF(sample(0), sample(size() - 1));
				const size_t arraySize =  value->getSize();
				QPointF prevPoint = sample(0);				
				for(size_t i = 1; i < arraySize - 1; ++i)
				{
					const QPointF point = sample(i);
					boundingRect_ = boundingRect_.unite(QRectF(prevPoint, point));
					prevPoint = point;
				}
			}
			else
			{
				boundingRect_ = QRectF();
			}
			boundingRectIsValid_ = true;
		}
		return boundingRect_;
	}
private:
	const SingleCurve & singleCurve_;
	QPointF coeff_;
	mutable QRectF boundingRect_;
	mutable bool boundingRectIsValid_;
};

class SeriesDataMultiPlot : public SeriesValue
{
public:
	SeriesDataMultiPlot(const ValuePtr & valueIn,
						const MultiCurve & multiCurve, size_t curveIndex,
						const QPointF & coeff,
						bool editableIn) :
		SeriesValue(valueIn->index(multiCurve.root)->index(curveIndex)->index(multiCurve.array),
					editableIn),
		multiCurve_(multiCurve), curveIndex_(curveIndex), boundingRectIsValid_(false), coeff_(coeff)
	{
	}
	~SeriesDataMultiPlot()
	{
	}
	virtual size_t size() const
	{
		return value->getSize();
	}
	virtual QPointF sample( size_t i ) const
	{
		QPointF point;
		ValuePtr element = value->indexArray(i);
			point.setX(element->index(multiCurve_.arg)->getReal() * coeff_.x());
			point.setY(element->index(multiCurve_.val)->getReal() * coeff_.y());
		return point;
	}
	virtual void addSample(int index, const QPointF & point)
	{
		value->addItem(index);
		setSample(index, point);
	}
	virtual void removeSample(int index)
	{
		value->removeItem(index);
	}
	virtual void setSample(int index, const QPointF & point)
	{
		const size_t seriesSize = size();
		ValuePtr element = value->indexArray(index);
		element->index(multiCurve_.arg)->setReal(std::min(multiCurve_.Xmin, point.x() / coeff_.x()));
		element->index(multiCurve_.val)->setReal(point.y() / coeff_.y());
		element->notifyChange();
	}
	virtual void makePath(Path & path) const
	{
		path = multiCurve_.root;
	}
	virtual void makePath(size_t index, Path & path) const
	{
		path = multiCurve_.root;
		path.push_back(curveIndex_);
		path.resize(path.size() + multiCurve_.array.size());
		std::copy(	multiCurve_.array.begin(), multiCurve_.array.end(),
					path.begin() + path.size() - multiCurve_.array.size());
		path.push_back(index);
	}
	virtual QRectF boundingRect() const
	{
		if(!boundingRectIsValid_)
		{
			if(size() > 1)
			{
				boundingRect_ = QRectF(sample(0), sample(size() - 1));
				const size_t arraySize =  value->getSize();
				if(arraySize > 2)
				{
					QPointF prevPoint = sample(1);				
					for(size_t i = 1; i < arraySize - 1; ++i)
					{
						const QPointF point = sample(i);
						boundingRect_ = boundingRect_.unite(QRectF(prevPoint, point));
						prevPoint = point;
					}
				}
			}
			else
			{
				boundingRect_ = QRectF();
			}
			boundingRectIsValid_ = true;
		}
		return boundingRect_;
	}
private:
	const MultiCurve & multiCurve_;
	size_t curveIndex_;
	QPointF coeff_;
	mutable QRectF boundingRect_;
	mutable bool boundingRectIsValid_;
};

static void makeСurveData(const SingleCurve & singleCurve, const ValuePtr & value,
						  bool editable, const QPointF & coeff, CurveData & curveData)
{
	curveData.data = new SeriesDataSinglePlot(value, singleCurve, coeff, editable);
	curveData.caption = singleCurve.caption;
	curveData.style = (QwtPlotCurve::CurveStyle)singleCurve.style;
	curveData.pen = singleCurve.pen;
}

static QColor makeColor(const QColor & colorStart, const QColor & colorStep, unsigned int index)
{
	return QColor(	colorStart.red() + colorStep.red() * index,
					colorStart.green() + colorStep.green() * index,
					colorStart.blue() + colorStep.blue() * index);
}

static const CurveParameterVariant & getCurveParameter(	const ValuePtr & value,
														const CurveParameter & curveParameter)
{
	assert(!curveParameter.variants.empty());
	if(curveParameter.variants.size() == 1)
		return curveParameter.variants.front();
	else
	{
		int variantNum = value->index(curveParameter.path)->getInteger();
		return curveParameter.variants[variantNum];
	}
}

static void makeСurveData(const MultiCurve & multiCurve, const ValuePtr & value,
						  bool editable, const QPointF & coeff, CurvesData & curvesData)
{
	const CurveParameterVariant & Z = getCurveParameter(value, multiCurve.Z);
	ValuePtr root = value->index(multiCurve.root);
	const vSize size = root->getSize();
	const QColor colorStep(	(multiCurve.colorMax.red() - multiCurve.colorMin.red()) / size,
							(multiCurve.colorMax.green() - multiCurve.colorMin.green()) / size,
							(multiCurve.colorMax.blue() - multiCurve.colorMin.blue()) / size);
	const vIndex curvesDataOldSize = curvesData.size();
	curvesData.resize(curvesDataOldSize + size);
	for(vIndex i = 0; i < size; ++i)
	{
		CurveData & curveData = curvesData[curvesDataOldSize + i];
		vReal parameter = root->indexArray(i)->index(multiCurve.parameter)->getReal();
		curveData.caption = Z.caption + "=" + OSS(parameter);
		curveData.data = new SeriesDataMultiPlot(value, multiCurve, i, coeff, editable);
		curveData.style = (QwtPlotCurve::CurveStyle)multiCurve.style;
		curveData.pen = multiCurve.pen;
		curveData.pen.setColor(makeColor(multiCurve.colorMin, colorStep, i));
	}
}

static void getPlotParameters(	const ValuePtr & value, const Curves & сurves, bool editable,
								PlotData & plotData)
{
	const CurveParameterVariant & X = getCurveParameter(value, сurves.X);
	const CurveParameterVariant & Y = getCurveParameter(value, сurves.Y);
	const Sim::Unit & unitX = Sim::UnitManager::instance().getUnit(X.type);
	const Sim::Unit & unitY = Sim::UnitManager::instance().getUnit(Y.type);
	QPointF coeff;
	coeff.rx() = unitX.coeff;
	coeff.ry() = unitY.coeff;
	plotData.curves.clear();
	for(Curves::SingleCurves::const_iterator itSP = сurves.singleCurves.begin();
		itSP != сurves.singleCurves.end();
		++itSP)
	{
		plotData.curves.push_back(CurveData());
		const SingleCurve & singleCurve = *itSP;
		QString subCaption;
		makeСurveData(singleCurve, value, editable, coeff, plotData.curves.back());
	}
	for(Curves::MultiCurves::const_iterator itMP = сurves.multiCurves.begin();
		itMP != сurves.multiCurves.end();
		++itMP)
	{
		const MultiCurve & multiCurve = *itMP;
		QString subCaption;
		makeСurveData(multiCurve, value, editable, coeff, plotData.curves);
	}
	plotData.caption = сurves.caption;
	plotData.captionX = X.caption;
	if(!unitX.name.empty())
		plotData.captionX = plotData.captionX + QString(", ") + QString(unitX.name.c_str());
	plotData.captionY = Y.caption;
	if(!unitY.name.empty())
		plotData.captionY = plotData.captionY + QString(", ") + QString(unitY.name.c_str());
}

void load(Path & path, const Variant & data)
{
	path.resize(data.size());
	for(Variant::const_iterator itP = data.begin();
		itP != data.end();
		++itP)
		path[itP.index()] = (*itP).asUInt();
}

inline void load(QString & str, const Variant & data)
{
	str = data.asCString();
}

static void load(CurveParameterVariant & curveParameterVariant, const Variant & data)
{
	if(data.empty())
		return;
	load(curveParameterVariant.caption, data["caption"]);
	if(!data["type"].empty())
		curveParameterVariant.type = Sim::UnitManager::instance().getUnitId(data["type"].asString());
	else
		curveParameterVariant.type = Sim::UNIT_NO;
}

static void load(CurveParameter & curveParameter, const Variant & data)
{
	if(data.empty())
		return;
	const Variant & vPath = data["path"];
	if(!vPath.empty())
	{
		load(curveParameter.path, vPath);
		const Variant & vVariants = data["variants"];
		curveParameter.variants.resize(vVariants.size());
		for(Variant::const_iterator itV = vVariants.begin();
			itV != vVariants.end();
			++itV)
			load(curveParameter.variants[itV.index()], *itV);
	}
	else
	{
		curveParameter.variants.resize(1);
		load(curveParameter.variants.front(), data);
	}
}

static CurveStyle loadCurveStyle(const Variant & data)
{
	if(data.empty())
		return CurveStyleLines;
	std::string style = data.asString();
	if(	style.empty() ||
		style == "lines")
		return CurveStyleLines;
	else if(style == "sticks")
		return CurveStyleSticks;
	else if(style == "steps")
		return CurveStyleSteps;
	else if(style == "dots")
		return CurveStyleDots;
	else
		return CurveStyleNo;
}

static Qt::PenStyle loadPenStyle(const Variant & data)
{
	if(data.empty())
		return Qt::SolidLine;
	std::string style = data.asString();
	if(	style.empty() ||
		style == "solid")
		return Qt::SolidLine;
	else if(style == "dash")
		return Qt::DashLine;
	else if(style == "dot")
		return Qt::DotLine;
	else if(style == "dash-dot")
		return Qt::DashDotLine;
	else if(style == "dash-dot")
		return Qt::DashDotDotLine;
	else if(style == "dash-dot-dot")
		return Qt::DashDotDotLine;
	else
		return Qt::NoPen;
}

static void load(QColor & color, const Variant & data)
{
	color = QColor(Qt::black);
	if(data.empty())
		return;
	color = QColor(	getInt(data, "red", color.red()),
					getInt(data, "green", color.green()),
					getInt(data, "blue", color.blue()),
					getInt(data, "alpha", color.alpha()));
	
}

static void load(QPen & pen, const Variant & data)
{
	if(data.empty())
		return;

	pen.setStyle(loadPenStyle(data["style"]));
	QColor color;
	load(color, data["color"]);
	pen.setColor(color);
	pen.setWidth(getInt(data, "width", pen.width()));
}

static void load(BaseCurve & baseCurve, const Variant & data)
{
	baseCurve.Xmin = !data["Xmin"].empty() ? data["Xmin"].asDouble() : -std::numeric_limits<double>::infinity();
	load(baseCurve.array, data["array"]);
	load(baseCurve.arg, data["arg"]);
	load(baseCurve.val, data["val"]);
	baseCurve.style = loadCurveStyle(data["style"]);
	load(baseCurve.pen, data["pen"]);
}

static void load(SingleCurve & singleCurve, const Variant & data)
{
	load((BaseCurve&)singleCurve, data);
	load(singleCurve.caption, data["caption"]);
}

static void load(MultiCurve & multiCurve, const Variant & data)
{
	load((BaseCurve&)multiCurve, data);
	load(multiCurve.Z, data["Z"]);
	load(multiCurve.colorMin, data["colorMin"]);
	load(multiCurve.colorMax, data["colorMax"]);
	load(multiCurve.root, data["root"]);
	load(multiCurve.parameter, data["parameter"]);
}

static void load(DescPlot & descPlot, const Variant & data)
{
	const Variant & singleCurves = data["singleCurves"];	
	descPlot.curves.singleCurves.resize(singleCurves.size());
	for(Variant::const_iterator itSC = singleCurves.begin();
		itSC != singleCurves.end();
		++itSC)
		load(descPlot.curves.singleCurves[itSC.index()], *itSC);

	const Variant & multiCurves = data["multiCurves"];
	descPlot.curves.multiCurves.resize(multiCurves.size());
	for(Variant::const_iterator itMC = multiCurves.begin();
		itMC != multiCurves.end();
		++itMC)
		load(descPlot.curves.multiCurves[itMC.index()], *itMC);

	load(descPlot.curves.caption, data["caption"]);
	load(descPlot.curves.X, data["X"]);
	load(descPlot.curves.Y, data["Y"]);

	assert(descPlot.curves.singleCurves.size() + descPlot.curves.multiCurves.size() > 0);
}

//Data Plot

DescPlot::DescPlot()
{
}

void CreatePlot::load(const Variant & data)
{
	WidgetDataCreate::load(data);
	::load(desc, data);
}

WidgetData * CreatePlot::operator()(QWidget * widget, WidgetDataCallback * callback) const
{
	return new WidgetDataPlot(desc, widget, editable, callback);
}

QString CreatePlot::getName() const
{
	return QString("Plot ") + desc.curves.caption;
}

WidgetDataPlot::WidgetDataPlot(const DescPlot & desc, QWidget * parent, bool editableIn, Callback * callbackIn) : WidgetData(editableIn, callbackIn), impl_(new Impl(desc, parent, this))
{
}

WidgetDataPlot::~WidgetDataPlot()
{
	//	delete impl_;
}

QWidget * WidgetDataPlot::widget()
{
	return impl_;
}

void WidgetDataPlot::setData(const ValuePtr & value)
{
	impl_->setData(value);
}

void WidgetDataPlot::rebuild()
{
	impl_->rebuild();
}

void WidgetDataPlot::onChange(const Path & path)
{
	impl_->onChange(path);
}

WidgetDataPlotImpl::WidgetDataPlotImpl(const DescPlot & descIn, QWidget * parent, WidgetData * widgetDataIn) :
	QWidget(parent), widgetData(widgetDataIn), desc(descIn), plot(NULL)
{
	QLayout * layout = new QVBoxLayout(this);
	layout->setContentsMargins(5, 5, 5, 5);
	plot = new Plot(this);
	//plot->setMinimumSize(QSize(200, 200));
	layout->addWidget(plot);
	layout->invalidate();
}

WidgetDataPlotImpl::~WidgetDataPlotImpl()
{
	delete plot;
	delete widgetData;
}

QSize WidgetDataPlotImpl::sizeHint() const
{
	return QWidget::sizeHint();
	return QWidget::sizeHint().expandedTo(QSize(200, 100));
}

QSize WidgetDataPlotImpl::minimumSizeHint() const
{
	return QWidget::minimumSizeHint();
	return QWidget::minimumSizeHint().expandedTo(QSize(200, 100));
}

void WidgetDataPlotImpl::setData(const ValuePtr & valueIn)
{
	value = valueIn;
	applyData_();
}

void WidgetDataPlotImpl::rebuild()
{
	applyData_();

	QSize s = size();
	QSize ms = minimumSize();

	QSize sh = sizeHint();
	QSize msh = minimumSizeHint();

	QSize tsh = layout()->totalSizeHint();

	int a = 0;
}

void WidgetDataPlotImpl::onChange(const Path & path)
{
	applyData_();
}

void WidgetDataPlotImpl::applyData_()
{
	QString caption, captionX, captionY;
	PlotData plotData;
	getPlotParameters(value, desc.curves, widgetData->isEditable(), plotData);
	plot->setData(plotData);
	//layout()->invalidate();
}

bool WidgetDataPlotImpl::eventFilter(QObject * object, QEvent * e)
{
	switch(e->type())
	{
		//case POINT_EVENT_SELECT:
		case POINT_EVENT_MOVE:
		case POINT_EVENT_ADD:
		case POINT_EVENT_REMOVE:
			{
				const EventPoint * eventPoint = (const EventPoint*)e;				
				SeriesValue * seriesValue = static_cast<SeriesValue*>(eventPoint->curve()->data());
				Path path;
				if(e->type() == POINT_EVENT_ADD || e->type() == POINT_EVENT_REMOVE)
					seriesValue->makePath(path);
				else
					seriesValue->makePath(eventPoint->index(), path);
				widgetData->notifyChange(path);
				return true;
			}
			break;
		default:
			break;
	}
	return QWidget::eventFilter(object, e);
}

//Data Plot Comp

DescPlotComp::DescPlotComp() : DescPlot()
{
}

void CreatePlotComp::load(const Variant & data)
{
	WidgetDataCompCreate::load(data);
	::load(desc, data);
}

WidgetDataComp * CreatePlotComp::operator()(QWidget * widget, WidgetDataCallback * callback) const
{
	return new WidgetDataCompPlot(desc, widget, editable, refEditable, callback);
}

QString CreatePlotComp::getName() const
{
	return QString("Plot ") + desc.curves.caption;
}

WidgetDataCompPlot::WidgetDataCompPlot(const DescPlotComp & desc, QWidget * parent, bool editableIn, bool refEditableIn, Callback * callbackIn) : WidgetDataComp(editableIn, refEditableIn, callbackIn), impl_(new Impl(desc, parent, this))
{
}

WidgetDataCompPlot::~WidgetDataCompPlot()
{
	//delete impl_;
}

QWidget * WidgetDataCompPlot::widget()
{
	return impl_;
}

void WidgetDataCompPlot::setData(const ValuePtr & value)
{
	impl_->setData(value);
}

void WidgetDataCompPlot::rebuild()
{
	impl_->rebuild();
}

void WidgetDataCompPlot::onChange(const Path & path)
{
	impl_->onChange(path);
}

void WidgetDataCompPlot::setDataComp(const ValuePtr & valueResult, const ValuePtr & valueReference)
{
	impl_->setData(valueResult, valueReference);
}

void WidgetDataCompPlot::resetReferenceData()
{
	impl_->resetReferenceData();
}

WidgetDataCompPlotImpl::WidgetDataCompPlotImpl(const DescPlotComp & desc, QWidget * parent, WidgetData * widgetData) : WidgetDataPlotImpl(desc, parent, widgetData)
{
}

WidgetDataCompPlotImpl::~WidgetDataCompPlotImpl()
{
}

void WidgetDataCompPlotImpl::setData(const ValuePtr & value)
{
	if(valueReference_ != NULL)
		valueReference_.reset();
	WidgetDataPlotImpl::setData(value);
}

void WidgetDataCompPlotImpl::setData(const ValuePtr & valueResult, const ValuePtr & valueReference)
{
	value = valueResult;
	valueReference_ = valueReference;
	applyDataComp_();
}

void WidgetDataCompPlotImpl::resetReferenceData()
{
	if(valueReference_ != NULL)
		valueReference_.reset();
	applyData_();
}

void WidgetDataCompPlotImpl::rebuild()
{
	applyData_();

	QSize s = size();
	QSize ms = minimumSize();

	QSize sh = sizeHint();
	QSize msh = minimumSizeHint();

	QSize tsh = layout()->totalSizeHint();
	QSize tms = layout()->totalMinimumSize();

	QSize ps = plot->size();
	QSize pms = plot->minimumSize();

	int a = 0;
}

void WidgetDataCompPlotImpl::onChange(const Path & path)
{
	if(valueReference_ != NULL)
		applyDataComp_();
	else
		applyData_();
}

void WidgetDataCompPlotImpl::applyDataComp_()
{
	PlotData plotData, plotDataReference;
	getPlotParameters(	value, desc.curves, widgetData->isEditable(),
						plotData);
	getPlotParameters(	valueReference_, desc.curves,
						((WidgetDataComp*)(widgetData))->isRefEditable(),
						plotDataReference);
	plotData.curves.resize(plotData.curves.size() + plotDataReference.curves.size());
	std::copy(	plotDataReference.curves.begin(), plotDataReference.curves.end(),
				plotData.curves.begin() + plotData.curves.size() - plotDataReference.curves.size());
	plot->setData(plotData);
}
