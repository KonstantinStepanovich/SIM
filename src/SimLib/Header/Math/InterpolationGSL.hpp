#ifndef _Interpolation_hpp_
#define _Interpolation_hpp_

#include "SDK/gsl/interpolation/gsl_interp.h"
#include <vector>

namespace Sim
{

namespace Math
{

struct InterpolationTableBounds
{
	InterpolationTableBounds(const double * xIn, const double * yIn, const size_t sizeIn) : x(xIn), y(yIn), size(sizeIn) {;}
	const double * x;
	const double * y;
	const size_t	size;
};

class InterpolationTable
{
public:
	template<size_t arraySize>
	InterpolationTable(const double (&xIn)[arraySize], const double (&yIn)[arraySize])
	{
		x.resize(arraySize);
		std::copy(x, xIn, xIn + arraySize);
		y.resize(arraySize);
		std::copy(y, yIn, yIn + arraySize);
	}
	inline InterpolationTableBounds getBounds() const { return InterpolationTableBounds(&*x.begin(), &*x.end(), x.size()); }
private:
	std::vector<double> x;
	std::vector<double> y;
};

template<size_t size>
class FixedInterpolationTable
{
public:
	FixedInterpolationTable(const double (&xIn)[size], const double (&yIn)[size])
	{
		std::copy(xIn, xIn + size, x);
		std::copy(yIn, yIn + size, y);
	}
	inline InterpolationTableBounds getBounds() const { return InterpolationTableBounds(x, y, size); }
private:
	double x[size];
	double y[size];
};

enum InterpType
{
	INTERP_TYPE_LINEAR,
	INTERP_TYPE_MAX
};

static const gsl_interp_type * interpTypes[INTERP_TYPE_MAX] = {
	gsl_interp_linear/*,
	gsl_interp_polynomial,
	gsl_interp_cspline,
	gsl_interp_cspline_periodic,
	gsl_interp_akima,
	gsl_interp_akima_periodic*/
};

template<class Table>
class Interpolator
{
public:
	template<size_t arraySize>
	Interpolator(InterpType interpType, double (&xIn)[arraySize], double (&yIn)[arraySize]) : type_(interpTypes[interpType]), table_(xIn, yIn)
	{
		InterpolationTableBounds bounds = table_.getBounds() ;
		if(type_->alloc != NULL)
			data_ = type_->alloc(bounds.size);
		type_->init(data_, bounds.x, bounds.y, bounds.size);
	}
	~Interpolator()
	{
		if(type_->free != NULL)
			type_->free(data_);
	}
	double interpolate(double x) const
	{
		double y;
		InterpolationTableBounds bounds = table_.getBounds() ;
		type_->eval(data_, bounds.x, bounds.y, bounds.size, x, NULL, &y);
		return y;
	}
private:
	const Table				table_;
	const gsl_interp_type * const type_;
	void *					data_;
};

}

}

#endif _Interpolation_hpp_