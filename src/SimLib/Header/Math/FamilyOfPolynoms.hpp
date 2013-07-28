#ifndef _FamilyOfPolynoms_hpp_
#define _FamilyOfPolynoms_hpp_

#include "Math/Polynom.hpp"

namespace Sim
{

namespace Math
{

template<typename T, size_t size1, size_t size2>
T evalFamilyOfPolynoms(const T (&coefs)[size1][size2], const T & arg1, const T & arg2)
{
	T coefs2[size2];
	for(int i = 0; i < size1; i++)
		coefs2[i] = evalPolynom(coefs[i], arg1);
	return evalPolynom(coefs, arg2);
}

template<typename T, size_t order1, size_t order2>
class FamilyOfPolynoms
{
public:
	enum {	COEFFS_COUNT = order1 + 1,
			CHILD_COEFFS_COUNT = order2 + 1 };
	typedef Polynom<T, order2> Polynom1;
	FamilyOfPolynoms()
	{
	}
	FamilyOfPolynoms(const T (&coefs)[COEFFS_COUNT][CHILD_COEFFS_COUNT])
	{
		set(coefs);
	}
	void set(const T (&coefs)[COEFFS_COUNT][CHILD_COEFFS_COUNT])
	{
		for(int i = 0; i < COEFFS_COUNT; i++)
			polynoms_[i].set(coefs[i]);
	}	
	T eval(const T & arg1, const T & arg2) const
	{	
		T coefs2[COEFFS_COUNT];
		for(int i = 0; i < COEFFS_COUNT; i++)
			coefs2[i] = polynoms_[i].eval(arg1);
		return evalPolynom(coefs2, arg2);
	}
	Polynom1 * getPolynoms()
	{
		return polynoms_;
	}
	const Polynom1 * getPolynoms() const
	{
		return polynoms_;
	}
private:
	Polynom1 polynoms_[COEFFS_COUNT];
};

}

}

#endif _FamilyOfPolynoms_hpp_