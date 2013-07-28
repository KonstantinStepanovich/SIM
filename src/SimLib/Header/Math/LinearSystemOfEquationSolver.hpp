#ifndef _LinearSystemOfEquationSolve_hpp_
#define _LinearSystemOfEquationSolve_hpp_

namespace Sim
{

namespace Math
{

bool solveLinearSystemOfEquations(double * A, const double * b, double * x, double * p, size_t dim);

template<typename T, size_t dim>
inline bool solveLinearSystemOfEquations(T (&A)[dim * dim], const T (&b)[dim], T (&x)[dim])
{
	T p[dim];
	return solveLinearSystemOfEquations(A, b, x, p, dim);
}

}

}

#endif _LinearSystemOfEquationSolve_hpp_