#ifndef _ParamIteration_hpp_
#define _ParamIteration_hpp_

#include "Iteration.hpp"

namespace MathModelSFM
{

struct Point
{
	Point(double xIn, double yIn) : x(xIn), y(yIn) {;}
	double x;
	double y;
};

typedef std::vector<Point> Points;

typedef Points Curve;

template<class Func, class Pred>
struct FillCurve
{
	FillCurve(const Func & funcIn, Points & pointsIn, bool argXIn, const Pred & predIn) :
		func(funcIn), points(pointsIn), argX(argXIn), pred(predIn) {;}
	void operator()(typename Func::argument_type arg)
	{
		Func::result_type result = func(arg);
		if(pred(result))
			points.push_back(argX ? Point(arg, result) : Point(result, arg));
	}
	const Func			func;
	Points &			points;
	bool				argX;
	Pred				pred;
};

template<class Func, class Pred>
inline FillCurve<Func, Pred> makeFillCurve(const Func & func, Points & pointsIn, bool argX, const Pred & pred)
{
	return FillCurve<Func, Pred>(func, pointsIn, argX, pred);
}

inline void scaleX(Curve & curve, double coeff)
{
	for(Curve::iterator itC = curve.begin(); itC != curve.end(); ++itC)
		itC->x *= coeff;
}

inline void scaleY(Curve & curve, double coeff)
{
	for(Curve::iterator itC = curve.begin(); itC != curve.end(); ++itC)
		itC->x *= coeff;
}

inline void shiftX(Curve & curve, double dist)
{
	for(Curve::iterator itC = curve.begin(); itC != curve.end(); ++itC)
		itC->x += dist;
}

inline void shiftY(Curve & curve, double dist)
{
	for(Curve::iterator itC = curve.begin(); itC != curve.end(); ++itC)
		itC->y += dist;
}

template<typename T>
struct PredTrue
{
	inline bool operator()(T val) const { return true; }
};

template<typename T>
struct PredInRange : public std::unary_function<T, bool>
{
	PredInRange(const T & min, const T & max) : min_(min), max_(max) {;}
	inline bool operator()(const T & val) const
	{
		return val > min_ && val < max_;
	}
private:
	T min_;
	T max_;
};

template<typename Iteration, class Func, class Pred>
inline void iterateCurve(const Iteration & iteration, const Func & func, Points & pointsIn, bool argX, const Pred & pred)
{
	iterate(iteration, makeFillCurve(func, pointsIn, argX, pred));
}

struct CurveByParam
{
	double parameter;
	Curve curve;
};

typedef std::vector<CurveByParam> Curves;

template<class Func, class Pred, class Iteration>
struct FillCurves
{
	FillCurves(const Iteration & iteration2In, const Func & funcIn, Curves & curvesIn, bool argXIn, const Pred & predIn) :
		iteration2(iteration2In), func(funcIn), curves(curvesIn), argX(argXIn), pred(predIn)		
	{
	}
	void operator()(typename Func::first_argument_type arg)
	{
		curves.push_back(CurveByParam());
		CurveByParam & curveByParam = curves.back();
		curveByParam.parameter = arg;
		iterateCurve(iteration2, std::bind1st(func, arg), curveByParam.curve, argX, pred);
	}
	const Func			func;
	Curves &			curves;
	bool				argX;
	Pred				pred;
	const Iteration &	iteration2;
};

template<class Func, class Pred, class Iteration>
inline FillCurves<Func, Pred, Iteration> makeFillCurves(const Iteration & iteration2, const Func & func, Curves & curves, bool argX, const Pred & pred)
{
	return FillCurves<Func, Pred, Iteration>(iteration2, func, curves, argX, pred);
}

template<class Iteration1, class Iteration2, class Func, class Pred>
inline void iterateCurves(const Iteration1 & iteration1, const Iteration2 & iteration2, const Func & func, Curves & curves, bool argX, const Pred & pred)
{
	iterate(iteration1, makeFillCurves(iteration2, func, curves, argX, pred));
}

}

#endif _ParamIteration_hpp_