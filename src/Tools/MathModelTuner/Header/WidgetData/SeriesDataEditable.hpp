#ifndef _SeriesDataEditable_hpp_
#define _SeriesDataEditable_hpp_

#include <qwt/qwt_series_data.h>

class SeriesDataEditable : public QwtSeriesData<QPointF>
{
public:
	SeriesDataEditable(bool editableIn) : editable(editableIn) {;}
	inline bool isEditable() const { return editable; }
	virtual void setSample(int index, const QPointF & point) = 0;
	virtual void addSample(int index, const QPointF & point) = 0;
	virtual void removeSample(int index) = 0;
protected:
	bool editable;
};

#endif _SeriesDataEditable_hpp_