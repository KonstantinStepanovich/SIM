#ifndef _LoadMath_hpp_
#define _LoadMath_hpp_

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"

#include "Math/Vec3.hpp"
#include "Math/Box3.hpp"
#include "Math/Quat.hpp"
#include "Math/Mat3x3.hpp"

#include "Math/LinearInterpolation.hpp"
#include "Math/BilinearInterpolation.hpp"
#include "Math/Polynom.hpp"
#include "Math/FamilyOfPolynoms.hpp"

namespace Sim
{

//Load

inline void load(const Variant & variant, Math::Vec3 & vec)
{
	load(variant, vec.m_floats, 3);
}

inline void load(const Variant & variant, Math::Box3 & box)
{
	load(variant.key("min"), box.min);
	load(variant.key("max"), box.max);
}

inline void load(const Variant & variant, Math::Quat & quat)
{
	for(size_t i = 0; i < 4; i++)
		quat[i] = variant.index(i);
}

inline bool tryLoadQuat(const Variant & variant, Math::Quat & quat)
{
	if(Variant vQuat = variant.tryKey("quat"))
		load(vQuat, quat);
	else if(Variant vRot = variant.tryKey("rot"))
	{
		Math::Vec3 axis;
		loadFromTable(vRot, "axis", axis);
		btScalar angle = vRot.key("angle");
		quat.setRotation(axis, angle);
	}
	else if(Variant vEuler = variant.tryKey("euler"))
	{
		Math::Vec3 euler;
		load(vEuler, euler);
		quat =	Math::Quat(Math::Vec3(0.0, 1.0, 0.0), euler.y()) * 
				Math::Quat(Math::Vec3(0.0, 0.0, 1.0), euler.z()) *
				Math::Quat(Math::Vec3(1.0, 0.0, 0.0), euler.x());
	}
	else
	{
		return false;
	}
	return true;
}

inline void load(const Variant & variant, Math::Matrix3x3 & mat)
{
	for(size_t i = 0; i < 3; i++)
		load(variant.index(i), mat[i]);
}

template<typename A, typename T, class S>
inline void load(const Variant & variant, Math::LinearInterpolator<A, T, S> & linterp)
{
	size_t size = variant.size();
	linterp.resize(size);
	for(size_t index = 0;
		index < size;
		index++)
	{
		const Variant & element = variant.index(index);
		A arg = element.index(0);
		T val;
		load(element.index(1), val);
		linterp.set(index, arg, val);
	}
}

template<typename A, typename T, class S>
void load(const Variant & variant, Math::BilinearInterpolator<A, T, S> & bilinterp)
{
	size_t rowCount = variant.size();
	size_t colCount;
	Variant row = variant.index(0);
	{
		colCount = row.size();
		bilinterp.resize(rowCount - 1, colCount - 1);
		for(size_t indexArg1 = 1;
			indexArg1 < colCount;
			indexArg1++)
		{
			bilinterp.setColArg(indexArg1 - 1, row.index(indexArg1));
		}
	}

	for(size_t rowIndex = 1;
		rowIndex < rowCount;
		rowIndex++)
	{
		Variant row = variant.index(rowIndex);
		bilinterp.setRowArg(rowIndex - 1, row.index(0));
		for(size_t colIndex = 1;
			colIndex < colCount;
			colIndex++)
		{
			T t;
			load(row.index(colIndex), t);
			bilinterp.setValue(rowIndex - 1, colIndex - 1, t);
		}
	}
}

template<typename T, size_t order>
inline void load(const Variant & variant, Math::Polynom<T, order> & polynom)
{
	const size_t size = order + 1;
	T coeffs[size];
	std::fill(coeffs, coeffs + size, T(0));
	for(size_t index = 0;
		index < size;
		index++)
	{
		coeffs[index] = variant.index(index);
	}
	polynom.set(coeffs);
}

template<typename T, size_t order1, size_t order2>
inline void load(const Variant & variant, Math::FamilyOfPolynoms<T, order1, order2> & familyOfPolynoms)
{
	size_t index = 0;
	const size_t size1 = order1 + 1, size2 = order2 + 1;
	T coeffs[size1][size2];
	std::fill((T*)coeffs, (T*)coeffs + size1 * size2, T(0));
	
	for(size_t index1 = 0;
		index1 < size1;
		index1++)
	{
		const Variant & element = variant.index(index1);
		for(size_t index2 = 0;
			index2 < size2;
			index2++)
		{
			coeffs[index1][index2] = element.index(index2);
		}
	}
	familyOfPolynoms.set(coeffs);
}

//Save

inline void save(Variant & variant,const  Math::Vec3 & vec)
{
	save(variant, vec.m_floats, 3);
}

inline void save(Variant & variant, const Math::Quat & quat)
{
	variant[0u] = quat.x();
	variant[1u] = quat.y();
	variant[2u] = quat.z();
	variant[3u] = quat.w();
}

//inline bool saveQuat(Variant & variant, const Math::Quat & quat)
//{
//	if(Variant element = variant.tryKey("quat"))
//		save(variant, quat);
//	else if(Variant element = variant.tryKey("rot"))
//	{
//		Math::Vec3 axis;
//		saveToTable(variant, "axis", axis);
//		btScalar angle = variant.key("angle");
//		quat.setRotation(axis, angle);
//	}
//	else if(Variant element = variant.tryKey("euler"))
//	{
//		Math::Vec3 euler;
//		save(variant, euler);
//		quat.setEuler(euler.y(), euler.z(), euler.x());
//	}
//	else
//	{
//		return false;
//	}
//	return true;
//}

inline void save(Variant & variant, const Math::Matrix3x3 & mat)
{
	for(size_t i = 0; i < 3; i++)
		save(variant[i], mat[i]);
}

template<typename A, typename T, class S>
inline void save(Variant & variant, const Math::LinearInterpolator<A, T, S> & linterp)
{
	size_t size = linterp.size();
	//variant.resize(size);
	for(size_t index = 0;
		index < size;
		index++)
	{
		Variant vElement = variant[index];
		const Math::LinearInterpolator<A, T, S>::Element & element = linterp.getElement(index);
		vElement[0u] = element.arg;
		save(vElement[1u], element.val);
	}
}

template<typename A, typename T, class S>
void save(Variant & variant, const Math::BilinearInterpolator<A, T, S> & bilinterp)
{
	Math::Size2D size = bilinterp.size();
	if(size.row < 1)
		return;
	Variant row = variant[0u];
	{
		for(size_t indexArg1 = 1;
			indexArg1 < size.col;
			indexArg1++)
		{
			row[indexArg1] = bilinterp.getColArg(indexArg1 - 1);
		}
	}
	for(size_t rowIndex = 1;
		rowIndex < size.row;
		rowIndex++)
	{
		Variant row = variant[rowIndex];
		row[0u] = bilinterp.getRowArg(rowIndex - 1);
		for(size_t colIndex = 1;
			colIndex < size.col;
			colIndex++)
		{
			save(row[colIndex], bilinterp.getValue(rowIndex - 1, colIndex - 1));
		}
	}
}

template<typename T, size_t order>
inline void save(Variant & variant, const Math::Polynom<T, order> & polynom)
{
	const size_t size = order + 1;
	for(size_t index = 0;
		index < size;
		index++)
		variant[index] = polynom.getCoeffs()[index];
}

template<typename T, size_t order1, size_t order2>
inline void save(Variant & variant, const Math::FamilyOfPolynoms<T, order1, order2> & familyOfPolynoms)
{
	//TO DO
	const size_t size1 = order1 + 1, size2 = order2 + 1;
	for(size_t index1 = 0;
		index1 < size1;
		index1++)
	{
		Variant & vPolynom = variant[index1];
		for(size_t index2 = 0;
			index2 < size2;
			index2++)
		{
			vPolynom[index2] = familyOfPolynoms.getPolynoms()[index1].getCoeffs()[index2];
		}
	}
}

}

#endif _LoadMath_hpp_