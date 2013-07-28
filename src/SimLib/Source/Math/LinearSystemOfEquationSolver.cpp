#include "Math/LinearSystemOfEquationSolver.hpp"

#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>

bool Sim::Math::solveLinearSystemOfEquations(double * A, const double * b, double * x, double * p, size_t dim)
{
	gsl_matrix_view mA = gsl_matrix_view_array(A, dim, dim);
	gsl_vector_const_view vb = gsl_vector_const_view_array(b, dim);
	gsl_vector_view vx = gsl_vector_view_array(x, dim);
	gsl_permutation pp;
	pp.data = (size_t*)p;
	pp.size = dim;
	int signum;
	return	gsl_linalg_LU_decomp(&mA.matrix, &pp, &signum) == GSL_SUCCESS &&
			gsl_linalg_LU_solve(&mA.matrix, &pp, &vb.vector, &vx.vector) == GSL_SUCCESS;
}
