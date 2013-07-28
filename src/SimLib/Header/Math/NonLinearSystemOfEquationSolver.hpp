#ifndef _NonLinearSystemOfEquationSolver_hpp
#define _NonLinearSystemOfEquationSolver_hpp

#include <algorithm>

#include "LinearSystemOfEquationSolver.hpp"

namespace Sim
{

namespace Math
{

template<typename T, size_t dim>
struct NonLinearSystemOfEquations
{
	enum { DIM = dim };
	typedef T Arg[dim];
	typedef T Val[dim];
	typedef T Jacob[dim * dim];

	template<class NonLinearSystemOfEquationsType>
	inline bool eval(	const NonLinearSystemOfEquationsType & nlse,
						typename const NonLinearSystemOfEquationsType::Arg & arg,
						typename NonLinearSystemOfEquationsType::Val & val) const
	{
		return false;
	}

	template<class NonLinearSystemOfEquationsType>
	inline bool fillJacob(	const NonLinearSystemOfEquationsType & nlse,
							typename const NonLinearSystemOfEquationsType::Arg & arg,
							typename NonLinearSystemOfEquationsType::Jacob & J) const
	{
		return false;
	}
};

template<class NonLinearSystemOfEquationsType>
inline bool checkAccuracy(	typename const NonLinearSystemOfEquationsType::Val & val,
							typename const NonLinearSystemOfEquationsType::Arg & accuracy)
{
	for(unsigned int a = 0; a < NonLinearSystemOfEquationsType::DIM; ++a)
		if(fabs(val[a]) > accuracy[a])
			return false;
	return true;
}

template<class NonLinearSystemOfEquationsType>
bool solveNonLinearSystemOfEquations(	const NonLinearSystemOfEquationsType & nlse,
										typename const NonLinearSystemOfEquationsType::Arg & accuracy,
										typename NonLinearSystemOfEquationsType::Arg & arg)
{
	const unsigned int maxIter = 20;
	unsigned int i = 0;
	while(i < maxIter)
	{
		NonLinearSystemOfEquationsType::Val val;
		if(!nlse.eval(arg, val))
			return false;
		if(checkAccuracy<NonLinearSystemOfEquationsType>(val, accuracy))
			return true;

		NonLinearSystemOfEquationsType::Jacob J;
		if(!nlse.fillJacob(arg, J))
			return false;

		for(unsigned int a = 0; a < NonLinearSystemOfEquationsType::DIM; ++a)
			val[a] = -val[a];
		NonLinearSystemOfEquationsType::Arg darg;
		if(!solveLinearSystemOfEquations(J, val, darg))
			return false;
		
		for(unsigned int a = 0; a < NonLinearSystemOfEquationsType::DIM; ++a)
			arg[a] += darg[a];

		++i;
	}
	return false;
}

}

}

#endif _NonLinearSystemOfEquationSolver_hpp