#include "WidgetData/CanvasPicker.hpp"
#include "WidgetData/SeriesDataEditable.hpp"

#include <limits>

#include <qapplication.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_scale_map.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_directpainter.h>

CanvasPicker::CanvasPicker(QwtPlot *plot):
    QObject(plot),
    d_selectedCurve(NULL),
    d_selectedPoint(-1),
	leftPointFixed(false), rightPointFixed(false)
{
    QwtPlotCanvas *canvas = plot->canvas();

    canvas->installEventFilter(this);

    // We want the focus, but no focus rect. The
    // selected point will be highlighted instead.

    canvas->setFocusPolicy(Qt::StrongFocus);
#ifndef QT_NO_CURSOR
    canvas->setCursor(Qt::PointingHandCursor);
#endif
    canvas->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    canvas->setFocus();

    const char *text =
        "All points can be moved using the left mouse button "
        "or with these keys:\n\n"
        "- Up:\t\tSelect next curve\n"
        "- Down:\t\tSelect previous curve\n"
        "- Left, ´-´:\tSelect next point\n"
        "- Right, ´+´:\tSelect previous point\n"
        "- 7, 8, 9, 4, 6, 1, 2, 3:\tMove selected point";
    canvas->setWhatsThis(text);

    shiftCurveCursor(true);
}

CanvasPicker::~CanvasPicker()
{

}

bool CanvasPicker::event(QEvent *e)
{
    if ( e->type() == QEvent::User )
    {
        showCursor(true);
        return true;
    }
    return QObject::event(e);
}

//void CanvasPicker::select(int curve, int point)
//{
//	showCursor(false);
//	d_selectedCurve = NULL;
//	d_selectedPoint = -1;
//	if(point == -1)
//		return;
//	const QwtPlotItemList& itmList = plot()->itemList();
//	int i = 0;
//	for (	QwtPlotItemIterator it = itmList.begin();
//			it != itmList.end();
//			++it)
//	{
//		QwtPlotItem * item = *it;
//		if(item->rtti() == QwtPlotItem::Rtti_PlotCurve)
//		{
//			if(i == curve)
//			{
//				d_selectedCurve = (QwtPlotCurve*)*it;
//				d_selectedPoint = point;
//				showCursor(true);
//				return;
//			}
//			++i;
//		}
//	}
//}

bool CanvasPicker::moveX(double x)
{
	if ( !d_selectedCurve )
		return false;

	SeriesDataEditable * seriesDataEditable = (SeriesDataEditable*)d_selectedCurve->data();
	seriesDataEditable->setSample(d_selectedPoint, QPointF(x, seriesDataEditable->sample(d_selectedPoint).y()));
	d_selectedCurve->itemChanged();	

    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
    */

    plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true);
    plot()->replot();
    plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false);

    showCursor(true);
	return true;
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *e)
{
    if ( object != (QObject *)plot()->canvas() )
        return false;

    switch(e->type())
    {
        case QEvent::FocusIn:
            showCursor(true);
			break;
        case QEvent::FocusOut:
            showCursor(false);
			break;
        case QEvent::Paint:
            QApplication::postEvent(this, new QEvent(QEvent::User));
            break;
        case QEvent::MouseButtonPress:
			{
				QMouseEvent * mouseEvent = (QMouseEvent*)e;
				if(select(mouseEvent->pos()))
				{
					if(mouseEvent->buttons() & Qt::RightButton)
						return remove();
					else
						return true;
				}
			}
            break;
		//case QEvent::MouseButtonRelease:
		//	unselect();
		//	return true;
		case QEvent::MouseButtonDblClick:
			{
				QMouseEvent * mouseEvent = (QMouseEvent*)e;
				if(add(((QMouseEvent*)e)->pos()))
					return true;
			}
			break;
        case QEvent::MouseMove:
        {
			QMouseEvent * mouseEvent = (QMouseEvent*)e;
			if(mouseEvent->modifiers() & (Qt::CTRL | Qt::ALT))
			{
				if(move(mouseEvent->pos(), mouseEvent->modifiers() & Qt::CTRL, mouseEvent->modifiers() & Qt::ALT))
					return true;
			}
			else
			{
				if(move(mouseEvent->pos(), true, true))
					return true;
			}
			break;
        }
        case QEvent::KeyPress:
        {
            const int delta = 5;
            switch(((const QKeyEvent *)e)->key())
            {
				//case Qt::Key_Insert:
				//	return add();
                case Qt::Key_Delete:
                    return remove();
                default:
                    break;
            }
        }
        default:
            break;
    }
    return QObject::eventFilter(object, e);
}

// Select the point at a position. If there is no point
// deselect the selected point

static QPointF transform(QwtPlot * plot, QwtPlotCurve * curve, const QPointF & point)
{
	return QPointF( plot->transform(curve->xAxis(), point.x()),
					plot->transform(curve->yAxis(), point.y()));
}

static double distToCurve(QwtPlot * plot, QwtPlotCurve * curve, const QPoint & pos)
{
	const QwtSeriesData<QPointF> * data = curve->data();
	double dist2Min = std::numeric_limits<double>::infinity();
	const QPointF posF(pos.x(), pos.y());
	const size_t size = data->size();
	QPointF point1 = transform(plot, curve, data->sample(0));
	for(size_t i = 1; i < size; ++i)
	{
		QPointF point2 = transform(plot, curve, data->sample(i));
		const QPointF dir1(posF.x() - point1.x(), posF.y() - point1.y());
		QPointF dir2(point2.x() - point1.x(), point2.y() - point1.y());
		const double length2 = sqrt(dir2.x() * dir2.x() + dir2.y() * dir2.y());
		dir2.rx() = dir2.x() / length2;
		dir2.ry() = dir2.y() / length2;
		double x = dir1.x() * dir2.x() + dir1.y() * dir2.y();
		double z = dir1.x() * dir2.y() + dir1.y() * -dir2.x();
		point1 = point2;
		double dist2;
		if(x < 0.0f)
			dist2 = pow(dir1.x(), 2) + pow(dir1.y(), 2);
		else if(x > length2)
			dist2 = pow(posF.x() - point2.x(), 2) + pow(posF.y() - point2.y(), 2);
		else
			dist2 = z * z;
		if(dist2 < dist2Min * dist2Min)
			dist2Min = dist2;
	}
	return sqrt(dist2Min);
}

bool CanvasPicker::select(const QPoint &pos)
{
    QwtPlotCurve * curve1 = NULL;
	double dist1 = std::numeric_limits<double>::infinity();
	QwtPlotCurve * curve2 = NULL;
	double dist2 = std::numeric_limits<double>::infinity();
    int index = -1;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = (QwtPlotCurve*)(*it);
			SeriesDataEditable * seriesDataEditable = static_cast<SeriesDataEditable*>(c->data());
			if(!seriesDataEditable->isEditable())
				continue;

            double d1;
            int idx = c->closestPoint(pos, &d1);
            if ( d1 < dist1 )
            {
                curve1 = c;
                index = idx;
                dist1 = d1;
            }
			double d2 = distToCurve(plot(), c, pos);
			if ( d2 < dist2 )
			{
				curve2 = c;
				dist2 = d2;
			}
        }
    }

    showCursor(false);
    d_selectedCurve = NULL;
    d_selectedPoint = -1;

    if ( curve1 && dist1 < 10 ) // 10 pixels tolerance
    {
        d_selectedCurve = curve1;
        d_selectedPoint = index;
        showCursor(true);
    }
	else if ( curve2 && dist2 < 10 )
	{
		d_selectedCurve = curve2;
		d_selectedPoint = -1;
		showCursor(true);
	}
	if(	d_selectedCurve != NULL &&
		d_selectedPoint != -1)
	{
		SeriesDataEditable * seriesDataEditable = (SeriesDataEditable*)d_selectedCurve->data();
		QPointF point = seriesDataEditable->sample(d_selectedPoint);
		parent()->event(&EventPoint(POINT_EVENT_SELECT, d_selectedCurve, d_selectedPoint, point)); //TO DO: curve index
	}
	return d_selectedCurve != NULL;
}

void CanvasPicker::unselect()
{
	showCursor(false);
	d_selectedCurve = NULL;
	d_selectedPoint = -1;
}

// Move the selected point
bool CanvasPicker::move(const QPoint & pos, bool flagX, bool flagY)
{
    if ( !d_selectedCurve )
        return false;

	if ( d_selectedPoint == -1)
		return false;

	SeriesDataEditable * seriesDataEditable = (SeriesDataEditable*)d_selectedCurve->data();
	QPointF point = seriesDataEditable->sample(d_selectedPoint);
	if(flagX)
	{
		if(d_selectedPoint == 0 && leftPointFixed)
			;
		else if(d_selectedPoint == d_selectedCurve->dataSize() - 1 && rightPointFixed)
			;
		else
		{
			point.rx() = plot()->invTransform(d_selectedCurve->xAxis(), pos.x());
			if(d_selectedPoint != 0)
			{
				double minPos = d_selectedCurve->sample(d_selectedPoint - 1).x();
				point.rx() = std::max(minPos, point.rx());
			}
			if(d_selectedPoint != d_selectedCurve->dataSize() - 1)
			{
				double maxPos = d_selectedCurve->sample(d_selectedPoint + 1).x();
				point.rx() = std::min(point.rx(), maxPos);
			}
		}
	}
	if(flagY)
	{
		point.ry() = plot()->invTransform(d_selectedCurve->yAxis(), pos.y());
	}

	seriesDataEditable->setSample(d_selectedPoint, point);
	d_selectedCurve->itemChanged();

    plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true);
    plot()->replot();
    plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false);

	parent()->event(&EventPoint(POINT_EVENT_MOVE, d_selectedCurve, d_selectedPoint, point));

    showCursor(true);
	return true;
}

bool CanvasPicker::add(const QPoint & pos)
{
	if ( !d_selectedCurve )
		return false;

	QPointF point;
	point.rx() = plot()->invTransform(d_selectedCurve->xAxis(), pos.x());
	point.ry() = plot()->invTransform(d_selectedCurve->yAxis(), pos.y());

	SeriesDataEditable * seriesDataEditable = (SeriesDataEditable*)d_selectedCurve->data();
	size_t i = 0;
	while(	i < seriesDataEditable->size() &&
			point.x() > seriesDataEditable->sample(i).x())
		++i;			
	seriesDataEditable->addSample(i, point);
	d_selectedCurve->itemChanged();
	d_selectedPoint = i;

    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true);
    plot()->replot();
    plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false);
	showCursor(true);

	parent()->event(&EventPoint(POINT_EVENT_ADD, d_selectedCurve, d_selectedPoint, seriesDataEditable->sample(d_selectedPoint)));
	return true;
}

bool CanvasPicker::remove()
{
	if ( !d_selectedCurve )
		return false;

	if( d_selectedPoint == -1 )
		return false;

	SeriesDataEditable * seriesDataEditable = (SeriesDataEditable*)d_selectedCurve->data();
	if(seriesDataEditable->size() > 2)
	{
		seriesDataEditable->removeSample(d_selectedPoint);

		plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true);
		plot()->replot();
		plot()->canvas()->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false);

		showCursor(false);
		parent()->event(&EventPoint(POINT_EVENT_REMOVE, d_selectedCurve, d_selectedPoint, seriesDataEditable->sample(d_selectedPoint)));
	}
	else
		showCursor(false);

	d_selectedCurve = NULL;
	d_selectedPoint = -1;
	
	return true;
}

// Hightlight the selected point
void CanvasPicker::showCursor(bool showIt)
{
    if ( !d_selectedCurve )
        return;

	//if ( d_selectedPoint == -1 )
	//	return;

    QwtSymbol *symbol = const_cast<QwtSymbol *>( d_selectedCurve->symbol() );

	const QSize size = symbol->size();
    const QBrush brush = symbol->brush();
    if ( showIt )
	{
		const int selectedSymbolSize = 8;
		symbol->setSize(QSize(selectedSymbolSize, selectedSymbolSize));
        symbol->setBrush(symbol->brush().color().dark(180));
	}

    QwtPlotDirectPainter directPainter;
    directPainter.drawSeries(d_selectedCurve, d_selectedPoint, d_selectedPoint);

    if ( showIt )
	{
		symbol->setSize(size);
        symbol->setBrush(brush); // reset brush
	}
}

// Select the next/previous curve 
void CanvasPicker::shiftCurveCursor(bool up)
{
    QwtPlotItemIterator it;

    const QwtPlotItemList &itemList = plot()->itemList();

    QwtPlotItemList curveList;
    for ( it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotCurve )
            curveList += *it;
    }
    if ( curveList.isEmpty() )
        return;

    it = curveList.begin();

    if ( d_selectedCurve )
    {
        for ( it = curveList.begin(); it != curveList.end(); ++it )
        {
            if ( d_selectedCurve == *it )
                break;
        }
        if ( it == curveList.end() ) // not found
            it = curveList.begin();

        if ( up )
        {
            ++it;
            if ( it == curveList.end() )
                it = curveList.begin();
        }
        else
        {
            if ( it == curveList.begin() )
                it = curveList.end();
            --it;
        }
    }
        
    showCursor(false);
    d_selectedPoint = 0;
    d_selectedCurve = (QwtPlotCurve *)*it;
    showCursor(true);
}

// Select the next/previous neighbour of the selected point
void CanvasPicker::shiftPointCursor(bool up)
{
    if ( !d_selectedCurve )
        return;

    int index = d_selectedPoint + (up ? 1 : -1);
    index = (index + d_selectedCurve->dataSize()) % d_selectedCurve->dataSize();

    if ( index != d_selectedPoint )
    {
        showCursor(false);
        d_selectedPoint = index;
        showCursor(true);
    }
}
