// Math.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Math/Interpolation.hpp"
#include <math.h>

//void testGSLInterpolation()
//{
//	double x[] = {0.0,		1.0,	2.0,	3.0,	4.0};
//	double y[] = {0.123,	0.56,	0.73,	0.94,	1.2};
//	Sim::Math::Interpolator<Sim::Math::FixedInterpolationTable<5> > interpolator(Sim::Math::INTERP_TYPE_LINEAR, x, y);
//	printf("%f", interpolator.interpolate(1.6));
//}

#include "Math/LinearInterpolation.hpp"

void testLinearInterpolation()
{
	printf("Linear interpolation\n");
	Sim::Math::LinearInterpolator<double, double>::Element data[] = 
	{
		{ 0.45, 2.56},
		{ 1.24, 1.78},
		{ 6.22, 12.45}
	};
	Sim::Math::LinearInterpolator<double, double> interpolator(data, data + 3);
	const double arg = 2.0;
	printf("f(%f) = %f\n", arg, interpolator.eval(arg, interpolator.findIndex(arg)));
}

void testLinearInterpolationN()
{
	printf("Linear interpolation N\n");
	struct Data
	{
		double a;
		double b;
		double c;
	};
	Sim::Math::LinearInterpolator<double, Data>::Element data[] = 
	{
		{ 0.45, {2.56, 2.67, 4.23 }},
		{ 1.24, {1.78, 1.9, 2.11 }},
		{ 6.22, {12.45, 20.425, 24.15 }}
	};
	Sim::Math::LinearInterpolator<double, Data> interpolator(data, data + 3);
	const double arg = 2.0;
	printf("fa(%f) = %f\n", arg, interpolator.eval(arg, interpolator.findIndex(arg), Sim::Math::makeStructMember(&Data::a)));
	printf("fb(%f) = %f\n", arg, interpolator.eval(arg, interpolator.findIndex(arg), Sim::Math::makeStructMember(&Data::b)));
	printf("fc(%f) = %f\n", arg, interpolator.eval(arg, interpolator.findIndex(arg), Sim::Math::makeStructMember(&Data::c)));
}

#include "Math/BilinearInterpolation.hpp"

void testBilinearInterpolationScalar()
{
	Sim::Math::BilinearInterpolator<double, double> bilinearInterpolator;
	const size_t rowCount = 3, colCount = 4;
	const double x0 = 0.0f, dx = 1.0f;
	const double y0 = 0.0f, dy = 0.12f;
	
	struct Function
	{
		inline double operator()(double x, double y) const
		{
			return 3.2 * x + pow(y, 2);
		}
	};
	Function function;

	bilinearInterpolator.resize(rowCount, colCount);
	for(size_t i = 0; i < rowCount; i++)
		bilinearInterpolator.setRowArg(i, x0 + dx * i);
	for(size_t i = 0; i < colCount; i++)
		bilinearInterpolator.setColArg(i, y0 + dy * i);
	for(size_t i = 0; i < rowCount; i++)
		for(size_t j = 0; j < colCount; j++)
			bilinearInterpolator.setValue(i, j, function(x0 + dx * i, y0 + dy * j));
	
	printf("Bilinear interpolation\n");
	const size_t testCount = 10;
	for(size_t i = 0; i < testCount; i++)
	{
		const double rowArg = (x0 + dx * (rowCount - 1)) * rand() / RAND_MAX, colArg = (y0 + dy * (colCount - 1)) * rand() / RAND_MAX;
		Sim::Math::BilinearInterpolator<double, double>::Index index = bilinearInterpolator.findIndex(rowArg, colArg);
		double valIntr = bilinearInterpolator.eval(rowArg, colArg, index);
		double valReal = function(rowArg, colArg);
		printf("f(%f, %f) = %f / %f\n", rowArg, colArg, valIntr, valReal);
	}
}

void testBilinearInterpolationStruct()
{
	struct Data
	{
		typedef double Type;
		Type a;
		Type b;
		Type c;
	};
	Sim::Math::BilinearInterpolator<double, Data> bilinearInterpolator;
	const size_t rowCount = 3, colCount = 4;
	const double x0 = 0.0f, dx = 1.0f;
	const double y0 = 0.0f, dy = 0.12f;

	struct Function
	{
		Function(double k) : k_(k) {;}
		inline double operator()(double x, double y) const
		{
			return k_ * x + pow(y, 2);
		}
	private:
		double k_;
	};
	Function function1(3.2);
	Function function2(3.5);
	Function function3(4.0);

	bilinearInterpolator.resize(rowCount, colCount);
	for(size_t i = 0; i < rowCount; i++)
		bilinearInterpolator.setRowArg(i, x0 + dx * i);
	for(size_t i = 0; i < colCount; i++)
		bilinearInterpolator.setColArg(i, y0 + dy * i);
	for(size_t i = 0; i < rowCount; i++)
		for(size_t j = 0; j < colCount; j++)
		{
			Data data;
			data.a = function1(x0 + dx * i, y0 + dy * j);
			data.b = function2(x0 + dx * i, y0 + dy * j);
			data.c = function2(x0 + dx * i, y0 + dy * j);
			bilinearInterpolator.setValue(i, j, data);
		}

	printf("Bilinear interpolation\n");
	const size_t testCount = 10;
	for(size_t i = 0; i < testCount; i++)
	{
		const double rowArg = (x0 + dx * (rowCount - 1)) * rand() / RAND_MAX, colArg = (y0 + dy * (colCount - 1)) * rand() / RAND_MAX;
		Sim::Math::Table2DIndex index(0, 0);
		index = bilinearInterpolator.findIndex(rowArg, colArg, index);
		double valIntr = bilinearInterpolator.eval(rowArg, colArg, index, Sim::Math::makeStructMember(&Data::a) );
		double valReal = function1(rowArg, colArg);
		printf("f(%f, %f) = %f / %f\n", rowArg, colArg, valIntr, valReal);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	testLinearInterpolation();
	testLinearInterpolationN();
	testBilinearInterpolationScalar();
	testBilinearInterpolationStruct();
	return 0;
}

