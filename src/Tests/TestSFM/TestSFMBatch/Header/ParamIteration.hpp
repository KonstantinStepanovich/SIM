#ifndef _ParamIteration_hpp_
#define _ParamIteration_hpp_

#include <iostream>
#include <iomanip>

#include "Iteration.hpp"

template<typename T>
inline void read(int argc, const char ** argv, IterationRange<T> & iteration)
{
	sscanf(argv[0], "%f", &iteration.start);
	sscanf(argv[1], "%f", &iteration.end);
	sscanf(argv[2], "%f", &iteration.step);
}

template<typename T>
struct PrintValue
{
	inline void operator()(T val) const
	{
		static const int precision = 6;
		int zeroes = (val == 0.0f) ? 1 : -(int)floor(__min(0, log10(fabs(val))));
		std::cout << std::showpoint << std::setprecision(precision - zeroes) << (val < 0.0f ? "" : " ") << val << ", ";
	}
};

template<typename T, typename M>
struct PrintMember
{
	PrintMember(const M T::*pM) : pM_(pM) {;}
	inline void operator()(const T & val) const
	{
		M m = val.*pM_;
		return PrintValue<M>()(m);
	}
private:
	const M T::*pM_;
};

template<typename T, typename M>
PrintMember<T, M> makePrintMember(const M T::*pM)
{
	return PrintMember<T, M>(pM);
}

template<typename T>
inline void printValues(const IterationRange<T> & iteration)
{
	iterate(iteration, PrintValue<T>());
	std::cout << std::endl;
}

template<class Exp, class Print>
struct PrintExp
{
	PrintExp(const Exp & exp, const Print & print = Print()) : exp_(exp), print_(print) {;}
	inline void operator()(typename Exp::argument_type val) const { print_(exp_(val)); }
private:
	Exp exp_;
	Print print_;
};

template<class Exp, class Printer>
inline PrintExp<Exp, Printer> makePrintExp(const Exp & exp, const Printer & printer)
{
	return PrintExp<Exp, Printer>(exp, printer);
}

template<class Exp>
inline PrintExp<Exp, PrintValue<typename Exp::result_type>> makePrintExp(const Exp & exp)
{
	return PrintExp<Exp, PrintValue<typename Exp::result_type>>(exp);
}

template<typename T, class Exp, class Print>
inline void iteratePrint1(const IterationRange<T> & iteration, const Exp & exp, const Print & print)
{
	printValues(iteration);
	iterate(iteration, PrintExp<Exp, Print>(exp, print));
}

template<typename T, class Exp>
inline void iteratePrint1(const IterationRange<T> & iteration, const Exp & exp)
{
	iteratePrint1(iteration, exp, PrintValue<typename Exp::result_type>());
}

template<typename T1, typename T2, class Exp, class Print>
void iteratePrint2(const IterationRange<T1> & iteration1, const IterationRange<T2> & iteration2, const Exp & exp, const Print & print)
{
	std::cout << "        , ";
	printValues(iteration2);
	struct PrintAndIterate
	{
		PrintAndIterate(const IterationRange<T2> & iteration, const Exp & exp, const Print & print) : iteration_(iteration), exp_(exp), print_(print)
		{
		}
		void operator()(T1 val) const
		{
			PrintValue<T1>()(val);
			iterate(iteration_, makePrintExp(std::bind1st(exp_, val), print_));
			std::cout << std::endl;
		}
	private:
		IterationRange<T2> iteration_;
		Exp exp_;
		Print print_;
	};
	iterate(iteration1, PrintAndIterate(iteration2, exp, print));
}

template<typename T1, typename T2, class Exp>
void iteratePrint2(const IterationRange<T1> & iteration1, const IterationRange<T2> & iteration2, const Exp & exp)
{
	iteratePrint2(iteration1, iteration2, exp, PrintValue<typename Exp::result_type>());
}

#endif _ParamIteration_hpp_