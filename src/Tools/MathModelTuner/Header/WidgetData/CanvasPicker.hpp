#include <qevent.h>
#include <qobject.h>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;

enum PointEvent
{
	POINT_EVENT_SELECT = QEvent::User,
	POINT_EVENT_MOVE,
	POINT_EVENT_ADD,
	POINT_EVENT_REMOVE
};

class EventPoint : public QEvent
{
public:
	EventPoint(PointEvent event, QwtPlotCurve * curve, int index, const QPointF & point) : QEvent((QEvent::Type)event), curve_(curve), index_(index), point_(point) {;}
	inline QwtPlotCurve * curve() const { return curve_; }
	inline int index() const { return index_; }
	inline const QPointF & point() const { return point_; }
private:
	QwtPlotCurve * curve_;
	int index_;
	QPointF point_;
};

class CanvasPicker: public QObject
{
    Q_OBJECT
public:;
    CanvasPicker(QwtPlot *plot);
	~CanvasPicker();
    virtual bool eventFilter(QObject *, QEvent *);

    virtual bool event(QEvent *);

	//void select(int curve, int point);
	bool moveX(double x);
	inline int getSelectedPoint() const { return d_selectedPoint; }
private:
    bool select(const QPoint & point);
	void unselect();
    bool move(const QPoint & point, bool flagX, bool flagY);
	bool add(const QPoint & point);
	bool remove();

    void release();

    void showCursor(bool enable);
    void shiftPointCursor(bool up);
    void shiftCurveCursor(bool up);

    QwtPlot *plot() { return (QwtPlot *)parent(); }
    const QwtPlot *plot() const { return (QwtPlot *)parent(); }

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;
	bool leftPointFixed, rightPointFixed;
};
