#ifndef _Equation_hpp_
#define _Equation_hpp_

namespace Sim
{

namespace Math
{

template<typename T>
unsigned int evalSqrEquation(T a, T b, T c, T (&x)[2])
{
	if(a == (T)0)
	{
		x[0] = -c / b;
		return 1;
	}
	T D = b * b - (T)4 * a * c;
	if(D > 0.0f)
	{
		x[0] = (-b + sqrt(D)) / ((T)2 * a);
		x[1] = (-b - sqrt(D)) / ((T)2 * a);
		return 2;
	}
	else if(D < 0.0f)
		return 0;
	else
	{
		x[0] = -b / ((T)2 * a);
		return 1;
	}
}

template<class Func, class Deriv>
inline typename Func::argument_type findNewton(	typename Func::argument_type x0,
												typename Func::argument_type epsilon,
												const Func & f, const Deriv & d)
{
	assert(epsilon > (typename Func::argument_type)0);
	typename Func::argument_type x = x0;
	Func::argument_type delta;
	do 
	{
		delta = -f(x) / d(x);
		x += delta;
	} while (std::abs(delta) > epsilon);
	return x;
}

template<class Func>
inline typename Func::argument_type evalSecant(	typename Func::argument_type a,
												typename Func::argument_type b,
												typename Func::argument_type epsilon,
												const Func & f)
{
	while(std::abs(b - a) > epsilon)
	{
		a = b - (b - a) * f(b)/(f(b) - f(a));
		b = a - (a - b) * f(a)/(f(a) - f(b));
	}
	return b;
}

}

}

#endif _Equation_hpp_